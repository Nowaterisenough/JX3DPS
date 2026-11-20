#ifndef DESKTOP_EQUIPMENT_DETAIL_CARD_H
#define DESKTOP_EQUIPMENT_DETAIL_CARD_H

#include <algorithm>
#include <functional>

#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>

namespace desktop {

class EquipmentDetailCard final : public QTextBrowser
{
public:
    explicit EquipmentDetailCard(QWidget *parent = nullptr) : QTextBrowser(parent)
    {
        setFrameShape(QFrame::NoFrame);
        setOpenLinks(false);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setStyleSheet(QStringLiteral(
            "QTextBrowser { background: #132221; color: white; border: none; border-radius: 4px; }"
            "QScrollBar:vertical { background: #132221; width: 8px; margin: 0; }"
            "QScrollBar::handle:vertical { background: #405b53; min-height: 24px; border-radius: 4px; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"));
        QFont text_font(QStringLiteral("SimHei"));
        text_font.setPixelSize(22);
        text_font.setWeight(QFont::Normal);
        setFont(text_font);
        setViewportMargins(0, 8, 0, 0);
        document()->setDocumentMargin(15);
        connect(this, &QTextBrowser::anchorClicked, this, [](const QUrl &url) {
            if (url.scheme() == "https" && url.host() == "www.jx3box.com" && url.path().startsWith("/item/view/")) {
                QDesktopServices::openUrl(url);
            }
        });
    }

    std::function<QPixmap(int)> icon;

    void SetContentHtml(const QString &html)
    {
        QTextBrowser::setHtml(html);
        // Qt's percentage line height scales image rows differently from browser CSS.
        for (auto block = document()->begin(); block.isValid(); block = block.next()) {
            QTextCursor cursor(block);
            auto        format = block.blockFormat();
            int         height = 33;
            for (auto fragment = block.begin(); !fragment.atEnd(); ++fragment) {
                if (fragment.fragment().charFormat().isImageFormat() &&
                    fragment.fragment().charFormat().toImageFormat().height() >= 30)
                {
                    height = 38;
                }
            }
            format.setLineHeight(height, QTextBlockFormat::FixedHeight);
            cursor.setBlockFormat(format);
        }
    }

    void RefreshIcon(int id, const QPixmap &pixmap)
    {
        document()->addResource(QTextDocument::ImageResource, QUrl(QStringLiteral("jx3icon:%1").arg(id)), pixmap);
        document()->markContentsDirty(0, document()->characterCount());
        viewport()->update();
    }

    static QString QualityColor(int quality)
    {
        switch (quality) {
            case 5: return QStringLiteral("#ff9600");
            case 4: return QStringLiteral("#ff00ff");
            case 3: return QStringLiteral("#0070ff");
            case 2: return QStringLiteral("#00c848");
            default: return QStringLiteral("#ffffff");
        }
    }

    static QString PlainDescription(const QString &text)
    {
        // Game descriptions use <text text="..." font=...>, not HTML.
        static const QRegularExpression gameText(QStringLiteral("text\\s*=\\s*\"([^\"]*)\""), QRegularExpression::CaseInsensitiveOption);
        auto        matches = gameText.globalMatch(text);
        QStringList parts;
        while (matches.hasNext()) {
            parts << matches.next().captured(1);
        }
        QString result = parts.isEmpty() ? text : parts.join(QStringLiteral("，"));
        result.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
        return result;
    }

    static QString Html(const QJsonObject &item, const QJsonObject &loadout, const QString &slot)
    {
        const QString start =
            QStringLiteral("<html><head><style>"
                           "body { font-family:'SimHei'; font-size:22px; font-weight:normal; color:#ffffff; }"
                           "p { margin:0; line-height:125%; }"
                           "a { color:#00dfff; text-decoration:none; }"
                           "</style></head><body>");
        if (item.isEmpty()) {
            return start + Row(slot.toHtmlEscaped() + QStringLiteral(" · 未装备")) +
                   Row(QStringLiteral("点击部位图标选择装备"), "#acaeb1") + "</body></html>";
        }

        const int     MAXIMUM = std::clamp(Value(item, { "max_refine", "MaxStrengthLevel" }).toInt(), 0, 12);
        const auto    current = Value(item, { "refine_level", "refineLevel" });
        const int     REFINED = std::clamp(current.toInt(), 0, MAXIMUM);
        const QString STARS =
            QStringLiteral("<span style='font-family:Arial; font-size:20px'>") + Span(QString(REFINED, QChar(0x2605)), "#ffff00") +
            Span(QString(MAXIMUM - REFINED, QChar(0x2606)), "#acaeb1") + "</span>";
        const QString REFINE = current.isUndefined() ? QStringLiteral("精炼上限: %1").arg(MAXIMUM)
                                                     : QStringLiteral("精炼等级: %1 / %2").arg(REFINED).arg(MAXIMUM);
        QString       html =
            start + QStringLiteral("<table width='100%' cellspacing='0' cellpadding='0'><tr><td>%1 %2</td>"
                                   "<td align='right' valign='top' style='font-family:Microsoft YaHei UI; "
                                   "font-size:20px'>%3</td></tr></table>")
                        .arg(Span(Value(item, { "name", "Name" }).toString().toHtmlEscaped(),
                                  QualityColor(Value(item, { "quality", "Quality" }).toInt())),
                             STARS,
                             Span(REFINE, "#7ee3a3"));
        html                  += Row(Value(item, { "type", "TypeLabel" }).toString().toHtmlEscaped());
        const auto ATTRIBUTES  = item.value("attributes").toArray();
        for (const auto &value : ATTRIBUTES) {
            const auto ATTRIBUTE = value.toObject();
            QString    label     = PlainDescription(ATTRIBUTE.value("label").toString());
            if (ATTRIBUTE.value("color").toString() == "white" &&
                (label.startsWith(QStringLiteral("体质")) || label.startsWith(QStringLiteral("身法")) ||
                 label.startsWith(QStringLiteral("力道")) || label.startsWith(QStringLiteral("元气")) ||
                 label.startsWith(QStringLiteral("根骨"))))
            {
                label.replace(QStringLiteral("提高"), QStringLiteral("+"));
            }
            html += Row(label.toHtmlEscaped() + Increment(ATTRIBUTE.value("refine_bonus")),
                        ATTRIBUTE.value("color").toString() == "green" ? "#00c848" : "#ffffff");
        }
        if (ATTRIBUTES.isEmpty()) {
            html += Row(QStringLiteral("暂无属性数据"), "#acaeb1");
        }
        for (const auto &value : Entries(Value(item, { "diamonds", "Diamonds" }))) {
            html += Effect(value, QStringLiteral("镶嵌孔："), "#00c848", 24, true);
        }

        const auto REQUIRED = Value(item, { "require_level", "RequireLevel" });
        if (!REQUIRED.isUndefined() && !REQUIRED.isNull()) {
            html += Row(QStringLiteral("需要等级%1").arg(REQUIRED.toInt()));
        }
        const auto DURABILITY     = Value(item, { "durability", "Durability" });
        const auto MAX_DURABILITY = Value(item, { "max_durability", "MaxDurability" });
        if (MAX_DURABILITY.toInt() > 0) {
            html += QStringLiteral("<p style='margin-top:6px'>%1</p>")
                        .arg(DURABILITY.isUndefined() || DURABILITY.isNull()
                                 ? QStringLiteral("耐久度上限：%1").arg(MAX_DURABILITY.toInt())
                                 : QStringLiteral("耐久度：%1/%2").arg(DURABILITY.toInt()).arg(MAX_DURABILITY.toInt()));
        }
        for (const auto &value : Entries(Value(item, { "enchantments", "enchant" }))) {
            html += Effect(value, {}, "#7e7eff", 32, false);
        }
        for (const auto &value : Entries(item.value("effects"))) {
            html += Effect(value, {}, "#7e7eff", 32, false);
        }

        const auto    set      = Value(item, { "set", "Set" }).toObject();
        const QString SET_NAME = set.value("name").toString();
        if (!SET_NAME.isEmpty()) {
            QJsonObject prospective = loadout;
            prospective.insert(slot, item);
            int count = 0;
            for (const auto &equipped : prospective) {
                if (Value(equipped.toObject(), { "set", "Set" }).toObject().value("name").toString() == SET_NAME) {
                    ++count;
                }
            }
            const auto SIBLINGS = set.value("siblings").toArray();
            html += Gap() + Row(SET_NAME.toHtmlEscaped() +
                                    (SIBLINGS.isEmpty() ? QStringLiteral("(%1件)").arg(count)
                                                        : QStringLiteral("(%1/%2)").arg(count).arg(SIBLINGS.size())),
                                "#ffff00");
            for (const auto &sibling : SIBLINGS) {
                html += Row(sibling.toString().toHtmlEscaped(), "#acaeb1");
            }
            html                      += Gap();
            const auto     BONUSES     = set.value("attributes").toObject();
            QList<QString> thresholds  = BONUSES.keys();
            std::sort(thresholds.begin(), thresholds.end(), [](const QString &a, const QString &b) {
                return a.toInt() < b.toInt();
            });
            for (const auto &threshold : thresholds) {
                html += Row(QStringLiteral("[%1] %2").arg(threshold.toHtmlEscaped(),
                                                          PlainDescription(BONUSES.value(threshold).toString()).toHtmlEscaped()),
                            threshold.toInt() > 0 && count >= threshold.toInt() ? "#00c848" : "#acaeb1");
            }
        }
        html             += Gap();
        const auto LEVEL  = Value(item, { "level", "Level" });
        if (!LEVEL.isUndefined()) {
            html += Row(QStringLiteral("品质等级 %1").arg(LEVEL.toInt()) + Increment(item.value("level_refine_bonus")), "#ffff00");
        }
        const auto SCORE = Value(item, { "score", "Score" });
        if (!SCORE.isUndefined()) {
            html += Row(QStringLiteral("装备分数 %1").arg(SCORE.toInt()) +
                            Increment(item.value("score_refine_bonus"), item.value("score_enchant_bonus")),
                        "#ff9600");
        }
        const QString SOURCE = Source(Value(item, { "source", "GetSource" }));
        if (!SOURCE.isEmpty()) {
            html += Row(
                QStringLiteral("<span style=\"font-family:'Microsoft YaHei UI'; font-size:20px\">装备来源：%1</span>")
                    .arg(SOURCE.toHtmlEscaped()));
        }
        const QString                   ID = item.value("id").toString();
        static const QRegularExpression itemId(QStringLiteral("^\\d+_\\d+$"));
        if (itemId.match(ID).hasMatch()) {
            html += Row(QStringLiteral("<a href='https://www.jx3box.com/item/view/%1'>↗ 查看装备百科词条</a>").arg(ID));
        }
        return html + "</body></html>";
    }

protected:
    QVariant loadResource(int type, const QUrl &url) override
    {
        if (type == QTextDocument::ImageResource && url.scheme() == "jx3icon") {
            return icon ? icon(url.path().toInt()) : QVariant();
        }
        if (type == QTextDocument::ImageResource && url.toString() == "socket:empty") {
            QPixmap pixmap(24, 24);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(QColor("#bd943e"));
            painter.setBrush(QColor("#493818"));
            painter.drawRect(1, 1, 21, 21);
            painter.drawPolygon(QPolygon{ QPoint(12, 4), QPoint(19, 12), QPoint(12, 20), QPoint(5, 12) });
            return pixmap;
        }
        return {};
    }

private:
    static QJsonValue Value(const QJsonObject &item, std::initializer_list<const char *> keys)
    {
        for (const auto *key : keys) {
            if (item.contains(key)) {
                return item.value(key);
            }
        }
        return QJsonValue(QJsonValue::Undefined);
    }

    static QJsonArray Entries(const QJsonValue &value)
    {
        if (value.isArray()) {
            return value.toArray();
        }
        if (value.isObject() || value.isString()) {
            return { value };
        }
        return {};
    }

    static QString Span(const QString &html, const QString &color)
    {
        return QStringLiteral("<span style='color:%1'>%2</span>").arg(color, html);
    }

    static QString Row(const QString &html, const QString &color = QStringLiteral("#ffffff"))
    {
        return QStringLiteral("<p style='color:%1'>%2</p>").arg(color, html);
    }

    static QString Gap() { return QStringLiteral("<p style='font-size:20px; line-height:125%'>&nbsp;</p>"); }

    static QString Increment(const QJsonValue &value, const QJsonValue &additional = {})
    {
        QStringList numbers;
        for (const auto &bonus : { value, additional }) {
            if (bonus.isDouble() && (bonus.toInt() != 0)) {
                numbers << (bonus.toInt() > 0 ? QStringLiteral("+") : QString()) + QString::number(bonus.toInt());
            }
        }
        return numbers.isEmpty() ? QString() : QStringLiteral(" ") + Span("(" + numbers.join(QString()) + ")", "#7ee3a3");
    }

    static QString Effect(const QJsonValue &value, const QString &prefix, const QString &color, int size, bool socket)
    {
        const auto object = value.toObject();
        const int  ID     = Value(object, { "icon_id", "icon", "IconID" }).toInt();
        const QString LABEL = value.isString() ? value.toString() : Value(object, { "label", "description", "name" }).toString();
        if (LABEL.isEmpty()) {
            return {};
        }
        QString image;
        if (ID > 0 || socket) {
            image = QStringLiteral("<img src='%1' width='%2' height='%2' style='vertical-align:middle'> ")
                        .arg(ID > 0 ? QStringLiteral("jx3icon:%1").arg(ID) : QStringLiteral("socket:empty"))
                        .arg(size);
        }
        return Row(image + prefix + PlainDescription(LABEL).toHtmlEscaped(), color);
    }

    static QString Source(const QJsonValue &value)
    {
        if (value.isString()) {
            return value.toString();
        }
        QStringList sources;
        for (const auto &entry : Entries(value)) {
            if (entry.isString()) {
                sources << entry.toString();
                continue;
            }
            const auto  SOURCE = entry.toObject();
            QString     text   = SOURCE.value("label").toString();
            QStringList children;
            for (const auto &child : SOURCE.value("children").toArray()) {
                const auto LABEL = child.isObject() ? child.toObject().value("label").toString() : child.toString();
                if (!LABEL.isEmpty()) {
                    children << LABEL;
                }
            }
            if (!children.isEmpty()) {
                text += QStringLiteral("（%1）").arg(children.join(QStringLiteral("，")));
            }
            if (!text.isEmpty()) {
                sources << text;
            }
        }
        return sources.join(QStringLiteral(" / "));
    }
};

} // namespace desktop
#endif
