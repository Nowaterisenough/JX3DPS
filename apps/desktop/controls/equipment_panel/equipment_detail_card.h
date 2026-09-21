#ifndef DESKTOP_EQUIPMENT_DETAIL_CARD_H
#define DESKTOP_EQUIPMENT_DETAIL_CARD_H

#include <QDesktopServices>
#include <QJsonArray>
#include <QJsonObject>
#include <QPainter>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

#include <algorithm>
#include <functional>

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
        QFont textFont(QStringLiteral("SimHei"));
        textFont.setPixelSize(22);
        textFont.setWeight(QFont::Normal);
        setFont(textFont);
        setViewportMargins(0, 8, 0, 0);
        document()->setDocumentMargin(15);
        connect(this, &QTextBrowser::anchorClicked, this, [](const QUrl &url) {
            if (url.scheme() == "https" && url.host() == "www.jx3box.com" && url.path().startsWith("/item/view/"))
                QDesktopServices::openUrl(url);
        });
    }

    std::function<QPixmap(int)> icon;

    void setHtml(const QString &html)
    {
        QTextBrowser::setHtml(html);
        // Qt's percentage line height scales image rows differently from browser CSS.
        for (auto block = document()->begin(); block.isValid(); block = block.next()) {
            QTextCursor cursor(block);
            auto format = block.blockFormat();
            int height = 33;
            for (auto fragment = block.begin(); !fragment.atEnd(); ++fragment)
                if (fragment.fragment().charFormat().isImageFormat() &&
                    fragment.fragment().charFormat().toImageFormat().height() >= 30) height = 38;
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
        auto matches = gameText.globalMatch(text);
        QStringList parts;
        while (matches.hasNext()) parts << matches.next().captured(1);
        QString result = parts.isEmpty() ? text : parts.join(QStringLiteral("，"));
        result.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
        return result;
    }

    static QString Html(const QJsonObject &item, const QJsonObject &loadout, const QString &slot)
    {
        const QString start = QStringLiteral("<html><head><style>"
            "body { font-family:'SimHei'; font-size:22px; font-weight:normal; color:#ffffff; }"
            "p { margin:0; line-height:125%; }"
            "a { color:#00dfff; text-decoration:none; }"
            "</style></head><body>");
        if (item.isEmpty()) return start + Row(slot.toHtmlEscaped() + QStringLiteral(" · 未装备")) +
            Row(QStringLiteral("点击部位图标选择装备"), "#acaeb1") + "</body></html>";

        const int maximum = std::clamp(Value(item, {"max_refine", "MaxStrengthLevel"}).toInt(), 0, 12);
        const auto current = Value(item, {"refine_level", "refineLevel"});
        const int refined = std::clamp(current.toInt(), 0, maximum);
        const QString stars = QStringLiteral("<span style='font-family:Arial; font-size:20px'>") +
                              Span(QString(refined, QChar(0x2605)), "#ffff00") +
                              Span(QString(maximum - refined, QChar(0x2606)), "#acaeb1") + "</span>";
        const QString refine = current.isUndefined() ? QStringLiteral("精炼上限: %1").arg(maximum) :
            QStringLiteral("精炼等级: %1 / %2").arg(refined).arg(maximum);
        QString html = start + QStringLiteral("<table width='100%' cellspacing='0' cellpadding='0'><tr><td>%1 %2</td>"
            "<td align='right' valign='top' style='font-family:Microsoft YaHei UI; font-size:20px'>%3</td></tr></table>")
            .arg(Span(Value(item, {"name", "Name"}).toString().toHtmlEscaped(), QualityColor(Value(item, {"quality", "Quality"}).toInt())),
                 stars, Span(refine, "#7ee3a3"));
        html += Row(Value(item, {"type", "TypeLabel"}).toString().toHtmlEscaped());
        const auto attributes = item.value("attributes").toArray();
        for (const auto &value : attributes) {
            const auto attribute = value.toObject();
            QString label = PlainDescription(attribute.value("label").toString());
            if (attribute.value("color").toString() == "white" &&
                (label.startsWith(QStringLiteral("体质")) || label.startsWith(QStringLiteral("身法")) ||
                 label.startsWith(QStringLiteral("力道")) || label.startsWith(QStringLiteral("元气")) || label.startsWith(QStringLiteral("根骨"))))
                label.replace(QStringLiteral("提高"), QStringLiteral("+"));
            html += Row(label.toHtmlEscaped() + Increment(attribute.value("refine_bonus")),
                        attribute.value("color").toString() == "green" ? "#00c848" : "#ffffff");
        }
        if (attributes.isEmpty()) html += Row(QStringLiteral("暂无属性数据"), "#acaeb1");
        for (const auto &value : Entries(Value(item, {"diamonds", "Diamonds"})))
            html += Effect(value, QStringLiteral("镶嵌孔："), "#00c848", 24, true);

        const auto required = Value(item, {"require_level", "RequireLevel"});
        if (!required.isUndefined() && !required.isNull()) html += Row(QStringLiteral("需要等级%1").arg(required.toInt()));
        const auto durability = Value(item, {"durability", "Durability"});
        const auto maxDurability = Value(item, {"max_durability", "MaxDurability"});
        if (maxDurability.toInt() > 0)
            html += QStringLiteral("<p style='margin-top:6px'>%1</p>").arg(durability.isUndefined() || durability.isNull() ?
                QStringLiteral("耐久度上限：%1").arg(maxDurability.toInt()) :
                QStringLiteral("耐久度：%1/%2").arg(durability.toInt()).arg(maxDurability.toInt()));
        for (const auto &value : Entries(Value(item, {"enchantments", "enchant"})))
            html += Effect(value, {}, "#7e7eff", 32, false);
        for (const auto &value : Entries(item.value("effects")))
            html += Effect(value, {}, "#7e7eff", 32, false);

        const auto set = Value(item, {"set", "Set"}).toObject();
        const QString setName = set.value("name").toString();
        if (!setName.isEmpty()) {
            QJsonObject prospective = loadout;
            prospective.insert(slot, item);
            int count = 0;
            for (const auto &equipped : prospective)
                if (Value(equipped.toObject(), {"set", "Set"}).toObject().value("name").toString() == setName) ++count;
            const auto siblings = set.value("siblings").toArray();
            html += Gap() + Row(setName.toHtmlEscaped() + (siblings.isEmpty() ? QStringLiteral("(%1件)").arg(count) :
                        QStringLiteral("(%1/%2)").arg(count).arg(siblings.size())), "#ffff00");
            for (const auto &sibling : siblings) html += Row(sibling.toString().toHtmlEscaped(), "#acaeb1");
            html += Gap();
            const auto bonuses = set.value("attributes").toObject();
            QList<QString> thresholds = bonuses.keys();
            std::sort(thresholds.begin(), thresholds.end(), [](const QString &a, const QString &b) { return a.toInt() < b.toInt(); });
            for (const auto &threshold : thresholds)
                html += Row(QStringLiteral("[%1] %2").arg(threshold.toHtmlEscaped(), PlainDescription(bonuses.value(threshold).toString()).toHtmlEscaped()),
                            threshold.toInt() > 0 && count >= threshold.toInt() ? "#00c848" : "#acaeb1");
        }
        html += Gap();
        const auto level = Value(item, {"level", "Level"});
        if (!level.isUndefined()) html += Row(QStringLiteral("品质等级 %1").arg(level.toInt()) + Increment(item.value("level_refine_bonus")), "#ffff00");
        const auto score = Value(item, {"score", "Score"});
        if (!score.isUndefined()) html += Row(QStringLiteral("装备分数 %1").arg(score.toInt()) +
                                             Increment(item.value("score_refine_bonus"), item.value("score_enchant_bonus")), "#ff9600");
        const QString source = Source(Value(item, {"source", "GetSource"}));
        if (!source.isEmpty()) html += Row(QStringLiteral("<span style=\"font-family:'Microsoft YaHei UI'; font-size:20px\">装备来源：%1</span>").arg(source.toHtmlEscaped()));
        const QString id = item.value("id").toString();
        static const QRegularExpression itemId(QStringLiteral("^\\d+_\\d+$"));
        if (itemId.match(id).hasMatch())
            html += Row(QStringLiteral("<a href='https://www.jx3box.com/item/view/%1'>↗ 查看装备百科词条</a>").arg(id));
        return html + "</body></html>";
    }

protected:
    QVariant loadResource(int type, const QUrl &url) override
    {
        if (type == QTextDocument::ImageResource && url.scheme() == "jx3icon")
            return icon ? icon(url.path().toInt()) : QVariant();
        if (type == QTextDocument::ImageResource && url.toString() == "socket:empty") {
            QPixmap pixmap(24, 24);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            painter.setPen(QColor("#bd943e"));
            painter.setBrush(QColor("#493818"));
            painter.drawRect(1, 1, 21, 21);
            painter.drawPolygon(QPolygon{QPoint(12, 4), QPoint(19, 12), QPoint(12, 20), QPoint(5, 12)});
            return pixmap;
        }
        return {};
    }

private:
    static QJsonValue Value(const QJsonObject &item, std::initializer_list<const char *> keys)
    {
        for (const auto *key : keys) if (item.contains(key)) return item.value(key);
        return QJsonValue(QJsonValue::Undefined);
    }
    static QJsonArray Entries(const QJsonValue &value)
    {
        if (value.isArray()) return value.toArray();
        if (value.isObject() || value.isString()) return {value};
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
        for (const auto &bonus : {value, additional}) if (bonus.isDouble() && bonus.toInt())
            numbers << (bonus.toInt() > 0 ? QStringLiteral("+") : QString()) + QString::number(bonus.toInt());
        return numbers.isEmpty() ? QString() : QStringLiteral(" ") + Span("(" + numbers.join(QString()) + ")", "#7ee3a3");
    }
    static QString Effect(const QJsonValue &value, const QString &prefix, const QString &color, int size, bool socket)
    {
        const auto object = value.toObject();
        const int id = Value(object, {"icon_id", "icon", "IconID"}).toInt();
        const QString label = value.isString() ? value.toString() : Value(object, {"label", "description", "name"}).toString();
        if (label.isEmpty()) return {};
        QString image;
        if (id > 0 || socket) image = QStringLiteral("<img src='%1' width='%2' height='%2' style='vertical-align:middle'> ")
            .arg(id > 0 ? QStringLiteral("jx3icon:%1").arg(id) : QStringLiteral("socket:empty")).arg(size);
        return Row(image + prefix + PlainDescription(label).toHtmlEscaped(), color);
    }
    static QString Source(const QJsonValue &value)
    {
        if (value.isString()) return value.toString();
        QStringList sources;
        for (const auto &entry : Entries(value)) {
            if (entry.isString()) { sources << entry.toString(); continue; }
            const auto source = entry.toObject();
            QString text = source.value("label").toString();
            QStringList children;
            for (const auto &child : source.value("children").toArray()) {
                const auto label = child.isObject() ? child.toObject().value("label").toString() : child.toString();
                if (!label.isEmpty()) children << label;
            }
            if (!children.isEmpty()) text += QStringLiteral("（%1）").arg(children.join(QStringLiteral("，")));
            if (!text.isEmpty()) sources << text;
        }
        return sources.join(QStringLiteral(" / "));
    }
};

} // namespace desktop
#endif
