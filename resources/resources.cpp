#include "Resources.h"

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
        static const int FONT_ID  = QFontDatabase::addApplicationFont(":/resources/fonts/Microsoft-YaHei-Regular.ttc");
        QStringList      families = QFontDatabase::applicationFontFamilies(FONT_ID);
        static QFont     font     = QFont(QFontDatabase::applicationFontFamilies(FONT_ID).front());
        font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
        font.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);
        return font;
    }();
    return font;
}

QFont &Resources::MonoFont()
{
    static QFont font = []() {
        // 加载霞鹜文楷等宽字体（用于中文）
        static const int LXGW_FONT_ID = QFontDatabase::addApplicationFont(":/resources/fonts/LXGWWenKaiMonoScreen.ttf");

        // 加载 Monaco 字体（用于英文）
        static const int MONACO_FONT_ID = QFontDatabase::addApplicationFont(":/resources/fonts/Monaco.ttf");

        QStringList lxgwFamilies   = QFontDatabase::applicationFontFamilies(LXGW_FONT_ID);
        QStringList monacoFamilies = QFontDatabase::applicationFontFamilies(MONACO_FONT_ID);

        // 创建字体，优先使用 Monaco，中文回退到 LXGW
        static QFont font;
        if (!monacoFamilies.isEmpty()) {
            font = QFont(monacoFamilies.front());
        } else if (!lxgwFamilies.isEmpty()) {
            font = QFont(lxgwFamilies.front());
        } else {
            font = QFont("Consolas");
        }

        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);
        font.setPointSize(11);
        font.setHintingPreference(QFont::HintingPreference::PreferNoHinting);
        font.setStyleStrategy(QFont::StyleStrategy::PreferAntialias);

        return font;
    }();
    return font;
}