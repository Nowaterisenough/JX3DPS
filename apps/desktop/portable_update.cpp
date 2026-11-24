#include "portable_update.h"

#include <filesystem>
#include <vector>

#include <QCoreApplication>
#include <QFileInfo>
#include <QLockFile>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QThread>
#include <QTimer>
#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <cerrno>
#include <csignal>
#endif

namespace desktop {
namespace fs = std::filesystem;
namespace {
const QRegularExpression stageName(QStringLiteral("^\\.jx3dps-update-[A-Za-z0-9]{6}$"));

bool ProcessEnded(qint64 pid, int waitMs)
{
    if (pid <= 0 || pid > 0x7fffffff) return false;
#ifdef Q_OS_WIN
    HANDLE handle = OpenProcess(SYNCHRONIZE, FALSE, static_cast<DWORD>(pid));
    if (!handle) return GetLastError() == ERROR_INVALID_PARAMETER;
    const bool ended = WaitForSingleObject(handle, waitMs) == WAIT_OBJECT_0;
    CloseHandle(handle);
    return ended;
#else
    for (int elapsed = 0; ; elapsed += 100) {
        if (::kill(static_cast<pid_t>(pid), 0) < 0 && errno == ESRCH) return true;
        if (elapsed >= waitMs) return false;
        QThread::msleep(100);
    }
#endif
}

struct Replacement {
    fs::path file, backup;
    bool backedUp = false, copied = false;
};

// Preserve files that were not shipped in the archive. Never follow an existing
// directory symlink while overlaying a portable folder. Linux packages can contain library symlinks.
void Overlay(const fs::path &source, const fs::path &target, const fs::path &backup,
             std::vector<Replacement> &changes, std::vector<fs::path> &createdDirs)
{
    for (const auto &entry : fs::directory_iterator(source)) {
        auto name = entry.path().filename();
        const auto file = target / name;
        const auto old = backup / name;
        const auto status = entry.symlink_status();
        const auto prior = fs::symlink_status(file);
        if (fs::is_directory(status)) {
            if (fs::exists(prior) && !fs::is_directory(prior)) throw std::runtime_error("directory conflict");
            if (fs::create_directory(file)) createdDirs.push_back(file);
            Overlay(entry.path(), file, old, changes, createdDirs);
            continue;
        }
        if ((!fs::is_regular_file(status) && !fs::is_symlink(status)) || fs::is_directory(prior))
            throw std::runtime_error("unexpected file type");
        changes.push_back({file, old});
        auto &item = changes.back();
        if (fs::exists(prior)) {
            fs::create_directories(old.parent_path());
            fs::rename(file, old);
            item.backedUp = true;
        }
        item.copied = true; // A disk-full error can leave a partial new file.
        if (fs::is_symlink(status)) fs::copy_symlink(entry.path(), file);
        else fs::copy_file(entry.path(), file);
    }
}

bool Restore(const std::vector<Replacement> &changes, const std::vector<fs::path> &dirs)
{
    bool complete = true;
    std::error_code ec;
    for (auto it = changes.rbegin(); it != changes.rend(); ++it) {
        if (it->copied) { fs::remove(it->file, ec); if (ec) complete = false; }
        if (it->backedUp) { fs::rename(it->backup, it->file, ec); if (ec) complete = false; }
    }
    for (auto it = dirs.rbegin(); it != dirs.rend(); ++it) fs::remove(*it, ec);
    return complete;
}
} // namespace

QDir PortableRoot()
{
    QDir dir(QCoreApplication::applicationDirPath());
#ifdef Q_OS_MACOS
    dir.cdUp(); dir.cdUp(); dir.cdUp(); // Parent of the .app bundle.
#endif
    return dir;
}

QString PortableExecutable(const QString &root)
{
#ifdef Q_OS_MACOS
    return root + "/JX3DPSDesktop.app/Contents/MacOS/JX3DPSDesktop";
#elif defined(Q_OS_WIN)
    return root + "/JX3DPS Desktop.exe";
#else
    return root + "/JX3DPS Desktop";
#endif
}

int ReplacePortable(const QStringList &args)
{
    const QDir payload = PortableRoot();
    QDir stage = payload;
    if (args.size() != 4 || payload.dirName() != "payload" || !stage.cdUp() || !stageName.match(stage.dirName()).hasMatch()) return 2;
    QDir root = stage;
    if (!root.cdUp() || QFileInfo(stage.path()).isSymLink() || QFileInfo(payload.path()).isSymLink()) return 2;
    const QString original = PortableExecutable(root.absolutePath());
    if (QFileInfo(original) != QFileInfo(args[2]) || !QFileInfo(original).isExecutable()) return 2;
    if (!ProcessEnded(args[3].toLongLong(), 120000)) {
        QMessageBox::warning(nullptr, QStringLiteral("更新已停止"), QStringLiteral("原工作台仍在运行，程序文件尚未替换。"));
        return 1;
    }
    QLockFile lock(root.filePath(".jx3dps-update.lock"));
    if (!lock.tryLock(5000)) {
        QMessageBox::warning(nullptr, QStringLiteral("更新已停止"), QStringLiteral("其他进程正在更新此目录。"));
        return 1;
    }
    std::vector<Replacement> changes;
    std::vector<fs::path> dirs;
    const QString backup = stage.filePath("backup");
    const QStringList restartArgs{"--update-complete", stage.dirName(), QString::number(QCoreApplication::applicationPid())};
    try {
        if (!stage.mkdir("backup")) throw std::runtime_error("cannot create backup");
#ifdef Q_OS_MACOS
        // Replace the signed bundle as a unit; do not edit individual signed resources.
        const auto bundle = root.filesystemPath() / "JX3DPSDesktop.app";
        const auto saved = stage.filesystemPath() / "backup" / "JX3DPSDesktop.app";
        fs::rename(bundle, saved);
        changes.push_back({bundle, saved, true, false});
        fs::rename(payload.filesystemPath() / "JX3DPSDesktop.app", bundle);
        changes.back().copied = true;
#else
        Overlay(payload.filesystemPath(), root.filesystemPath(), QDir(backup).filesystemPath(), changes, dirs);
#endif
        if (!QProcess::startDetached(original, restartArgs, root.absolutePath())) throw std::runtime_error("restart failed");
    } catch (const std::exception &e) {
#ifdef Q_OS_MACOS
        if (!changes.empty() && changes.front().copied) {
            std::error_code ec;
            fs::rename(changes.front().file, payload.filesystemPath() / "JX3DPSDesktop.app", ec);
            changes.front().copied = false;
        }
#endif
        const bool restored = Restore(changes, dirs);
        QMessageBox::warning(nullptr, QStringLiteral("更新失败"),
            (restored ? QStringLiteral("已恢复原程序。") : QStringLiteral("请从备份目录恢复：%1").arg(backup)) + "\n" + QString::fromLocal8Bit(e.what()));
        if (restored) QProcess::startDetached(original, restartArgs, root.absolutePath());
        return 1;
    }
    return 0;
}

void FinishPortableUpdate(const QStringList &args)
{
    if (args.size() != 4 || args[1] != "--update-complete" || !stageName.match(args[2]).hasMatch()) return;
    const QDir root = PortableRoot();
    const QFileInfo stage(root.filePath(args[2]));
    if (!stage.isDir() || stage.isSymLink() || stage.canonicalPath() != root.canonicalPath() ||
        !QFileInfo::exists(stage.filePath() + "/backup")) return;
    const auto pid = args[3].toLongLong();
    auto *timer = new QTimer(qApp);
    QObject::connect(timer, &QTimer::timeout, timer, [timer, stage, pid] {
        if (!ProcessEnded(pid, 0)) return;
        timer->stop();
        QDir(stage.filePath()).removeRecursively();
        timer->deleteLater();
    });
    timer->start(500);
}
} // namespace desktop
