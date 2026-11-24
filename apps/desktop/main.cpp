#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include <cstdio>

#include "controls/theme/dark_style.h"

#include "resources.h"
#include "release_updater.h"
#include "portable_update.h"
#include "workbench.h"

#ifdef JX3DPS_DESKTOP_WORKLOAD
#    include <string_view>
int RunDesktopWorkload(int argc, char *argv[]);
#endif

int main(int argc, char *argv[])
{
#ifdef JX3DPS_DESKTOP_WORKLOAD
    if (argc > 1 && std::string_view(argv[1]) == "--pgo-workload") {
        return RunDesktopWorkload(argc, argv);
    }
#endif
    const QApplication APP(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("JX3DPS"));
    QCoreApplication::setApplicationName(QStringLiteral("MacroWorkbench"));
    QCoreApplication::setApplicationVersion(desktop::ReleaseUpdater::Version());
    if (QCoreApplication::arguments().value(1) == QStringLiteral("--update-identity")) {
        const auto identity = (desktop::ReleaseUpdater::Version() + ' ' + desktop::ReleaseUpdater::BuildId()).toLatin1();
        std::puts(identity.constData());
        return 0;
    }
    if (QCoreApplication::arguments().value(1) == QStringLiteral("--replace-portable"))
        return desktop::ReplacePortable(QCoreApplication::arguments());
    desktop::FinishPortableUpdate(QCoreApplication::arguments());
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    QTranslator qt_chinese;
    if (qt_chinese.load(QStringLiteral("qt_zh_CN"), QCoreApplication::applicationDirPath() + QStringLiteral("/translations")) ||
        qt_chinese.load(QStringLiteral("qtbase_zh_CN"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
    {
        QApplication::installTranslator(&qt_chinese);
    }
    Resources::InitResources();
    QApplication::setFont(Resources::Font());
    QApplication::setStyle(new DarkStyle());
    Workbench window;
    window.show();
    window.StartUpdateChecks();
    return QApplication::exec();
}
