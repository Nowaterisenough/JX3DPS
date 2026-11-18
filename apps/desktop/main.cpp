#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>
#include "controls/theme/dark_style.h"
#include "resources.h"
#include "workbench.h"

#ifdef JX3DPS_DESKTOP_WORKLOAD
#include <string_view>
int RunDesktopWorkload(int argc, char *argv[]);
#endif

int main(int argc, char *argv[])
{
#ifdef JX3DPS_DESKTOP_WORKLOAD
    if (argc > 1 && std::string_view(argv[1]) == "--pgo-workload") return RunDesktopWorkload(argc, argv);
#endif
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("JX3DPS"));
    QCoreApplication::setApplicationName(QStringLiteral("MacroWorkbench"));
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    QTranslator qtChinese;
    if (qtChinese.load(QStringLiteral("qt_zh_CN"), QCoreApplication::applicationDirPath() + QStringLiteral("/translations")) ||
        qtChinese.load(QStringLiteral("qtbase_zh_CN"), QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtChinese);
    Resources::InitResources();
    QApplication::setFont(Resources::Font());
    QApplication::setStyle(new DarkStyle());
    Workbench window;
    window.show();
    return QApplication::exec();
}
