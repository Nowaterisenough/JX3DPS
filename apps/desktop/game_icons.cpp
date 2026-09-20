#include "game_icons.h"

#include <stdexcept>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListView>
#include <QPainter>
#include <QScreen>
#include <QStyledItemDelegate>

#include "resources/resources.h"

static void InitIconData()
{
    Q_INIT_RESOURCE(desktop_data);
    Resources::InitResources();
}

namespace desktop {
namespace {
QJsonObject Entry(Specialization specialization, QString name)
{
    static const auto data = [] {
        InitIconData();
        QFile file(QStringLiteral(":/desktop/icons.json"));
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("missing desktop icon catalog");
        }
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    const auto entries = data.value(QString::number(static_cast<int>(specialization))).toObject();
    name               = name.section(QStringLiteral("（"), 0, 0);
    if (entries.contains(name)) {
        return entries.value(name).toObject();
    }
    name.replace(QStringLiteral("持续伤害"), QStringLiteral("Dot"));
    if (entries.contains(name)) {
        return entries.value(name).toObject();
    }
    const auto parts = name.split(QStringLiteral("·"));
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (entries.contains(*it)) {
            return entries.value(*it).toObject();
        }
    }
    return {};
}

void Frame(QPainter &painter, const QRect &rect, bool selected, bool focus)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(selected ? QColor(239, 191, 95) : QColor(93, 104, 118), selected ? 2 : 1));
    painter.drawRoundedRect(rect.adjusted(1, 1, -2, -2), 3, 3);
    if (selected) {
        painter.fillRect(QRect(rect.right() - 12, rect.bottom() - 12, 11, 11), QColor(239, 191, 95));
        painter.setPen(QPen(QColor(30, 32, 36), 2));
        painter.drawPolyline(QPolygon{ QPoint(rect.right() - 10, rect.bottom() - 7),
                                       QPoint(rect.right() - 7, rect.bottom() - 4),
                                       QPoint(rect.right() - 3, rect.bottom() - 10) });
    }
    if (focus) {
        painter.setPen(QPen(Qt::white, 1, Qt::DotLine));
        painter.drawRect(rect.adjusted(3, 3, -4, -4));
    }
}

class IconChoiceDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override { return { 48, 48 }; }

    void paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        p->save();
        if (option.state & QStyle::State_Selected) {
            p->fillRect(option.rect, option.palette.highlight());
        }
        const auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            icon.paint(p, option.rect.adjusted(5, 5, -5, -5));
        } else {
            p->setPen(option.palette.text().color());
            p->drawText(option.rect, Qt::AlignCenter, QStringLiteral("无"));
        }
        p->restore();
    }
};
} // namespace

QIcon GameIcon(int id)
{
    static const int initialized = Resources::InitResources();
    Q_UNUSED(initialized);
    return QIcon(QStringLiteral(":/resources/images/JX3/Icons/%1.png").arg(id));
}

QIcon GameIcon(Specialization specialization, const QString &name)
{
    const auto entry = Entry(specialization, name);
    return entry.isEmpty() ? QIcon{} : GameIcon(entry.value("Icon").toInt());
}

QString GameDescription(Specialization specialization, const QString &name)
{
    return Entry(specialization, name).value("Description").toString();
}

IconCheckBox::IconCheckBox(const QString &name, Specialization specialization, QWidget *parent) :
    QCheckBox(name, parent)
{
    setIcon(GameIcon(specialization, name));
    setAccessibleName(name);
    setToolTip(name + '\n' + GameDescription(specialization, name));
    setFixedSize(sizeHint());
    setCursor(Qt::PointingHandCursor);
}

void IconCheckBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }
    icon().paint(&painter, rect().adjusted(4, 4, -4, -4));
    if (!isChecked()) {
        painter.fillRect(rect().adjusted(4, 4, -4, -4), QColor(0, 0, 0, 45));
    }
    Frame(painter, rect(), isChecked(), hasFocus());
}

IconComboBox::IconComboBox(const QString &category, QWidget *parent) : QComboBox(parent)
{
    setAccessibleName(category);
    setFixedSize(sizeHint());
    setCursor(Qt::PointingHandCursor);
    auto *choices = new QListView(this);
    choices->setViewMode(QListView::IconMode);
    choices->setMovement(QListView::Static);
    choices->setResizeMode(QListView::Adjust);
    choices->setGridSize({ 48, 48 });
    choices->setUniformItemSizes(true);
    choices->setMinimumWidth(250);
    choices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    choices->setItemDelegate(new IconChoiceDelegate(choices));
    setView(choices);
    setMaxVisibleItems(12);
    connect(this, &QComboBox::currentIndexChanged, this, [this, category] {
        setToolTip(category + QStringLiteral("：") + currentText() + '\n' + currentData(Qt::ToolTipRole).toString());
    });
}

void IconComboBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }
    const auto icon = itemIcon(currentIndex() > 0 ? currentIndex() : 1);
    if (currentIndex() == 0) {
        painter.setOpacity(isEnabled() ? 0.38 : 0.2);
    }
    icon.paint(&painter, QRect(4, 4, 36, 36));
    painter.setOpacity(isEnabled() ? 1 : 0.4);
    Frame(painter, rect(), currentIndex() > 0, hasFocus());
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(227, 232, 240));
    painter.drawPolygon(QPolygon{ QPoint(40, 17), QPoint(46, 17), QPoint(43, 22) });
}

void IconComboBox::showPopup()
{
    view()->setFixedHeight(std::max(1, (count() + 4) / 5) * 48 + 2);
    QComboBox::showPopup();
    auto *popup = view()->window();
    popup->setFixedHeight(view()->height() + 2);
    const auto available = screen()->availableGeometry();
    auto       origin    = mapToGlobal(QPoint(0, height()));
    if (origin.y() + popup->height() > available.bottom()) {
        origin.setY(mapToGlobal(QPoint()).y() - popup->height());
    }
    origin.setX(std::clamp(origin.x(), available.left(), std::max(available.left(), available.right() - popup->width() + 1)));
    popup->move(origin);
    view()->scrollTo(model()->index(currentIndex(), modelColumn()));
}
} // namespace desktop
