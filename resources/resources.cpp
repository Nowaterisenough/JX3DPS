#include "resources.h"

#include <QFont>
#include <QFontDatabase>
#include <QResource>

int Resources::InitResources()
{
    Q_INIT_RESOURCE(resources);
    return 0;
}

QFont &Resources::Font()
{
    static QFont font = []() {
        static const int FONT_ID = QFontDatabase::addApplicationFont(":/resources/fonts/Microsoft-YaHei-Semibold.ttc");
        QStringList families = QFontDatabase::applicationFontFamilies(FONT_ID);
        QFont font;
        if (!families.isEmpty()) {
            font = QFont(families.front());
        }
        font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
        font.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);
        return font;
    }();
    return font;
}

QFont &Resources::MonoFont()
{
    static QFont font = []() {
        // 加载 Monaco 字体（用于英文）
        static const int MONACO_FONT_ID = QFontDatabase::addApplicationFont(":/resources/fonts/Monaco.ttf");

        // 加载霞鹜文楷等宽字体（用于中文）
        static const int LXGW_FONT_ID = QFontDatabase::addApplicationFont(":/resources/fonts/LXGWWenKaiMonoScreen.ttf");

        QStringList monacoFamilies = QFontDatabase::applicationFontFamilies(MONACO_FONT_ID);
        QStringList lxgwFamilies   = QFontDatabase::applicationFontFamilies(LXGW_FONT_ID);

        // 创建字体回退列表：英文优先 Monaco，中文回退到霞鹜文楷等宽
        QStringList fontFamilies;
        if (!monacoFamilies.isEmpty()) {
            fontFamilies << monacoFamilies.front();
        }
        if (!lxgwFamilies.isEmpty()) {
            fontFamilies << lxgwFamilies.front();
        }
        fontFamilies << "Consolas"; // 最后的回退字体

        static QFont font;
        font.setFamilies(fontFamilies);
        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);
        font.setPointSize(11);
        font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
        font.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);

        return font;
    }();
    return font;
}