#include "game_icons.h"

#include <stdexcept>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

#include "controls/choice_popup.h"
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
    if (entry.isEmpty() && specialization == Specialization::TaiXu) {
        for (const auto &buff : rt::tai_xu::team::Definitions) {
            if (name != QString::fromUtf8(buff.name.data(), buff.name.size())) continue;
            // Team effects without a verified game icon use a named tile.
            QPixmap tile(32, 32);
            tile.fill(QColor(46, 64, 72));
            QPainter painter(&tile);
            painter.setPen(QColor(225, 233, 235));
            painter.drawText(tile.rect(), Qt::AlignCenter, name.left(1));
            painter.end();
            return QIcon(tile);
        }
    }
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
    m_popup = new ChoicePopup(this);
    auto *layout = new QVBoxLayout(m_popup);
    layout->setContentsMargins(2, 2, 2, 2);
    auto *choices = m_choices = new QListView(m_popup);
    choices->setObjectName(QStringLiteral("iconChoices"));
    choices->setViewMode(QListView::IconMode);
    choices->setMovement(QListView::Static);
    choices->setResizeMode(QListView::Adjust);
    choices->setGridSize({ 48, 48 });
    choices->setUniformItemSizes(true);
    choices->setFrameShape(QFrame::NoFrame);
    choices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    choices->setItemDelegate(new IconChoiceDelegate(choices));
    // Keep QComboBox::view() compatible for keyboard/configuration callers;
    // the view itself is still hosted by our pre-laid-out popup.
    setView(choices);
    choices->setParent(m_popup);
    layout->addWidget(choices);
    m_popup->Watch(choices);
    m_popup->acceptChoice = [this] { AcceptChoice(); };
    connect(choices, &QListView::clicked, this, [this] { AcceptChoice(); });
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
    if (!isEnabled() || count() == 0 || m_popup->isVisible()) return;
    m_choices->setModel(model());
    m_choices->setRootIndex(rootModelIndex());
    m_choices->setModelColumn(modelColumn());
    const auto selected = model()->index(currentIndex(), modelColumn(), rootModelIndex());
    m_choices->setCurrentIndex(selected);
    m_popup->Open(this, { 260, std::min(6, std::max(1, (count() + 4) / 5)) * 48 + 4 }, m_choices);
    m_choices->scrollTo(selected);
}

void IconComboBox::hidePopup()
{
    m_popup->hide();
}

void IconComboBox::AcceptChoice()
{
    const auto index = m_choices->currentIndex();
    if (!index.isValid() || !(index.flags() & Qt::ItemIsEnabled) || !(index.flags() & Qt::ItemIsSelectable)) return;
    setCurrentIndex(index.row());
    hidePopup();
    emit activated(currentIndex());
    emit textActivated(currentText());
}

void IconComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setFocus(Qt::MouseFocusReason);
        if (m_popup->isVisible()) hidePopup();
        else showPopup();
        event->accept();
    } else QComboBox::mousePressEvent(event);
}

void IconComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) event->accept();
    else QComboBox::mouseReleaseEvent(event);
}

void IconComboBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_F4 ||
        ((event->modifiers() & Qt::AltModifier) && (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up))) {
        if (m_popup->isVisible()) hidePopup();
        else showPopup();
        event->accept();
    } else QComboBox::keyPressEvent(event);
}
} // namespace desktop
