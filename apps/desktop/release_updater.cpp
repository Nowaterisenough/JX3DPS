#include "release_updater.h"
#include "portable_update.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLockFile>
#include <QMessageBox>
#include <QNetworkReply>
#include <QProcess>
#include <QProgressDialog>
#include <QPushButton>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>

namespace desktop {
namespace {
const QString releases = QStringLiteral("https://github.com/Nowaterisenough/JX3DPS/releases");

QNetworkRequest Request(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "JX3DPS-Desktop/" + ReleaseUpdater::Version().toLatin1());
    request.setTransferTimeout(30000);
    return request;
}

// Pump Qt events while fetching the archive so cancellation and the simulation remain responsive.
bool Download(QNetworkAccessManager &network, const DesktopRelease &release, QFile &file, QProgressDialog &progress, QString &error)
{
    auto *reply = network.get(Request(release.package));
    reply->setReadBufferSize(1024 * 1024);
    QEventLoop loop;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    auto consume = [&] {
        const auto data = reply->readAll();
        if (data.isEmpty() || !error.isEmpty()) return;
        if (file.pos() + data.size() > release.bytes || file.write(data) != data.size()) {
            error = QStringLiteral("更新包大小异常或无法写入磁盘。");
            reply->abort();
            return;
        }
        hash.addData(data);
        progress.setValue(static_cast<int>(file.pos() * 100 / release.bytes));
    };
    QObject::connect(reply, &QIODevice::readyRead, &loop, consume);
    QObject::connect(&progress, &QProgressDialog::canceled, reply, &QNetworkReply::abort);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    progress.show();
    if (!reply->isFinished()) loop.exec();
    if (reply->error() == QNetworkReply::NoError) consume();
    else if (error.isEmpty()) error = reply->errorString();
    reply->deleteLater();
    if (progress.wasCanceled()) { error.clear(); return false; }
    if (error.isEmpty() && (file.pos() != release.bytes || hash.result().toHex() != release.sha256 || !file.flush()))
        error = QStringLiteral("更新包校验失败，请重新下载。");
    file.close();
    return error.isEmpty();
}

bool RunStep(const QString &program, const QStringList &args, QProgressDialog &progress, QString &error, QByteArray *output = nullptr)
{
    QProcess process;
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    QObject::connect(&timeout, &QTimer::timeout, &process, &QProcess::kill);
    QObject::connect(&progress, &QProgressDialog::canceled, &process, &QProcess::kill);
    QObject::connect(&process, &QProcess::finished, &loop, &QEventLoop::quit);
    QObject::connect(&process, &QProcess::errorOccurred, &loop, [&](QProcess::ProcessError e) {
        if (e == QProcess::FailedToStart) loop.quit();
    });
    process.start(program, args);
    timeout.start(180000);
    if (process.state() != QProcess::NotRunning) loop.exec();
    if (progress.wasCanceled()) return false;
    if (process.error() == QProcess::FailedToStart || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        error = QStringLiteral("无法准备新版：%1").arg(process.readAllStandardError().trimmed().left(500));
        if (process.error() == QProcess::FailedToStart) error += process.errorString();
        return false;
    }
    if (output) *output = process.readAllStandardOutput().trimmed();
    return true;
}

bool PrepareUpdate(const DesktopRelease &release, QNetworkAccessManager &network, QWidget *parent,
                   const QString &payload, QString &error)
{
    QProgressDialog progress(QStringLiteral("正在下载 %1…").arg(release.tag), QStringLiteral("取消"), 0, 100, parent);
    progress.setWindowTitle(QStringLiteral("软件更新"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setAutoClose(false);
    progress.setAutoReset(false);
    progress.setMinimumDuration(0);
    QTemporaryFile archive;
    if (!archive.open()) { error = archive.errorString(); return false; }
    if (!Download(network, release, archive, progress, error)) return false;
    progress.setRange(0, 0);
    progress.setLabelText(QStringLiteral("正在解压已校验的更新包…"));
#ifdef Q_OS_MACOS
    const QString tool = QStringLiteral("/usr/bin/ditto");
    const QStringList args{"-x", "-k", archive.fileName(), payload};
#else
#ifdef Q_OS_WIN
    const QString tool = QDir(qEnvironmentVariable("SystemRoot")).filePath("System32/tar.exe");
#else
    const QString tool = QStringLiteral("/usr/bin/tar");
#endif
    const QStringList args{"-xf", archive.fileName(), "-C", payload};
#endif
    if (!RunStep(tool, args, progress, error)) return false;
    progress.setLabelText(QStringLiteral("正在验证新版能否启动…"));
    QByteArray identity;
    if (!RunStep(PortableExecutable(payload), {"--update-identity"}, progress, error, &identity)) return false;
    if (identity != (release.tag + " " + ReleaseUpdater::BuildId()).toLatin1()) {
        error = QStringLiteral("压缩包中的版本或平台与发布信息不一致。"); return false;
    }
    return true;
}
} // namespace

ReleaseUpdater::ReleaseUpdater(QWidget *window) : QObject(window), window_(window) {}
QString ReleaseUpdater::Version() { return QStringLiteral(JX3DPS_RELEASE_TAG); }
QString ReleaseUpdater::BuildId() { return QStringLiteral(JX3DPS_BUILD_ID); }

QVersionNumber ReleaseUpdater::NumericVersion(QString tag)
{
    static const QRegularExpression pattern(QStringLiteral("^v?[0-9]{1,9}(?:\\.[0-9]{1,9}){1,3}$"));
    if (!pattern.match(tag).hasMatch()) return {};
    if (tag.startsWith('v')) tag.remove(0, 1);
    return QVersionNumber::fromString(tag).normalized();
}

DesktopRelease ReleaseUpdater::ReadRelease(const QJsonObject &json, const QString &build)
{
    DesktopRelease release;
    const auto tag = json.value("tag_name").toString();
    if (json.value("draft").toBool() || json.value("prerelease").toBool() || NumericVersion(tag).isNull()) return release;
    release.tag = tag;
    release.notes = json.value("body").toString().section(QStringLiteral("\n## 各构建版本性能对照"), 0, 0).trimmed();
    const QString ext = build.startsWith("linux-") ? "tar.gz" : "zip";
    const QRegularExpression namePattern("^JX3DPS-" + QRegularExpression::escape(build) + "-[0-9]+(?:\\.[0-9]+)+\\." + QRegularExpression::escape(ext) + "$");
    static const QRegularExpression digestPattern("^sha256:[a-fA-F0-9]{64}$");
    for (const auto &entry : json.value("assets").toArray()) {
        const auto asset = entry.toObject();
        const auto name = asset.value("name").toString();
        const auto digest = asset.value("digest").toString();
        const QUrl url(asset.value("browser_download_url").toString());
        if (!namePattern.match(name).hasMatch() || url != QUrl(releases + "/download/" + tag + '/' + name) ||
            !digestPattern.match(digest).hasMatch() || asset.value("size").toInteger() <= 0) continue;
        release.package = url;
        release.sha256 = digest.mid(7).toLatin1().toLower();
        release.bytes = asset.value("size").toInteger();
        break;
    }
    return release;
}

void ReleaseUpdater::StartAutomaticChecks()
{
    QTimer::singleShot(2000, this, [this] { Check(false); });
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this] { Check(false); });
    timer->start(std::chrono::hours(6));
}

void ReleaseUpdater::Check(bool manual)
{
    manual_ |= manual;
    if (busy_) return;
    busy_ = true;
    auto request = Request(QUrl(QStringLiteral("https://api.github.com/repos/Nowaterisenough/JX3DPS/releases/latest")));
    request.setRawHeader("Accept", "application/vnd.github+json");
    auto *reply = network_.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        const auto release = ReadRelease(QJsonDocument::fromJson(reply->readAll()).object(), BuildId());
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError || release.tag.isEmpty()) {
            if (manual_) QMessageBox::warning(window_, QStringLiteral("检查更新"), reply->error() == QNetworkReply::NoError ? QStringLiteral("没有可识别的稳定版本。") : reply->errorString());
        } else if (NumericVersion(release.tag) <= NumericVersion(Version())) {
            if (manual_) QMessageBox::information(window_, QStringLiteral("检查更新"), QStringLiteral("当前版本 %1 已是最新。").arg(Version()));
        } else if (manual_ || offered_ != release.tag) {
            offered_ = release.tag;
            Offer(release);
        }
        busy_ = manual_ = false;
    });
}

void ReleaseUpdater::Offer(const DesktopRelease &release)
{
    QDialog dialog(window_);
    dialog.setObjectName("releaseUpdateDialog");
    dialog.setWindowTitle(QStringLiteral("发现新版 JX3DPS"));
    dialog.resize(570, 400);
    auto *layout = new QVBoxLayout(&dialog);
    layout->addWidget(new QLabel(QStringLiteral("当前版本 %1　→　新版本 %2").arg(Version(), release.tag)));
    class NotesBrowser final : public QTextBrowser {
        QVariant loadResource(int, const QUrl &) override { return {}; }
    };
    auto *notes = new NotesBrowser;
    notes->setObjectName("releaseNotes");
    notes->setOpenLinks(false);
    notes->document()->setMarkdown(release.notes.isEmpty() ? QStringLiteral("此版本未附更新说明。") : release.notes, QTextDocument::MarkdownNoHTML);
    layout->addWidget(notes);
    auto *buttons = new QDialogButtonBox;
    const bool packaged = !release.package.isEmpty() && QFileInfo(PortableExecutable(PortableRoot().absolutePath())) == QFileInfo(QCoreApplication::applicationFilePath());
    auto *update = buttons->addButton(packaged ? QStringLiteral("下载更新包") : QStringLiteral("查看发布页"), QDialogButtonBox::AcceptRole);
    auto *later = buttons->addButton(QStringLiteral("稍后"), QDialogButtonBox::RejectRole);
    later->setDefault(true);
    connect(update, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(later, &QPushButton::clicked, &dialog, &QDialog::reject);
    layout->addWidget(buttons);
    if (dialog.exec() != QDialog::Accepted) return;
    if (!packaged) { QDesktopServices::openUrl(QUrl(releases + "/tag/" + release.tag)); return; }
    const auto root = PortableRoot();
    QLockFile lock(root.filePath(".jx3dps-update.lock"));
    if (!lock.tryLock()) {
        QMessageBox::warning(window_, QStringLiteral("无法更新"), QStringLiteral("请确认程序目录可写，且其他工作台没有正在更新。"));
        return;
    }
    QTemporaryDir stage(root.filePath(".jx3dps-update-XXXXXX"));
    if (!stage.isValid() || !QDir(stage.path()).mkdir("payload")) {
        QMessageBox::warning(window_, QStringLiteral("无法更新"), QStringLiteral("无法在程序目录创建临时文件夹。"));
        return;
    }
    const QString payload = stage.filePath("payload");
    QString error;
    if (!PrepareUpdate(release, network_, window_, payload, error)) {
        if (!error.isEmpty()) QMessageBox::warning(window_, QStringLiteral("更新失败"), error);
        return;
    }
    QMessageBox ready(QMessageBox::Question, QStringLiteral("更新包已就绪"),
                      QStringLiteral("退出后将用 %1 覆盖原目录中的程序文件并重新启动。正在运行的模拟将停止。").arg(release.tag),
                      QMessageBox::NoButton, window_);
    auto *restart = ready.addButton(QStringLiteral("替换并重启"), QMessageBox::AcceptRole);
    ready.setDefaultButton(ready.addButton(QStringLiteral("取消"), QMessageBox::RejectRole));
    ready.exec();
    if (ready.clickedButton() != restart) return;
    if (!QProcess::startDetached(PortableExecutable(payload), {"--replace-portable", QCoreApplication::applicationFilePath(),
                                                              QString::number(QCoreApplication::applicationPid())}, stage.path())) {
        QMessageBox::warning(window_, QStringLiteral("更新失败"), QStringLiteral("无法启动压缩包中的新版程序。"));
        return;
    }
    stage.setAutoRemove(false);
    window_->close();
}
} // namespace desktop
