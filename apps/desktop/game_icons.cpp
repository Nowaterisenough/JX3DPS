#include "game_icons.h"

#include <algorithm>
#include <stdexcept>

#include <QFile>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QScreen>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

#include "controls/choice_popup.h"
#include "controls/theme/accent_color.h"
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

void Frame(QPainter &painter, const QRect &rect, bool selected, bool focus, const QColor &accent, bool showCheck = true)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(selected ? accent : QColor(72, 77, 88), selected ? 1.5 : 1));
    painter.drawRoundedRect(rect.adjusted(1, 1, -2, -2), 5, 5);
    if (selected && showCheck) {
        painter.fillRect(QRect(rect.right() - 12, rect.bottom() - 12, 11, 11), accent);
        painter.setPen(QPen(QColor(30, 32, 36), 2));
        painter.drawPolyline(QPolygon{ QPoint(rect.right() - 10, rect.bottom() - 7),
                                       QPoint(rect.right() - 7, rect.bottom() - 4),
                                       QPoint(rect.right() - 3, rect.bottom() - 10) });
    }
    if (focus) {
        painter.setPen(QPen(accent.lighter(115), 1));
        painter.drawRoundedRect(rect.adjusted(3, 3, -4, -4), 3, 3);
    }
}

} // namespace

void PaintGameIcon(QPainter &painter, const QRect &bounds, const QIcon &icon, bool grayscale)
{
    auto pixmap = icon.pixmap(QSize(64, 64), painter.device()->devicePixelRatioF());
    if (pixmap.isNull()) return;
    if (grayscale) {
        const auto key = QStringLiteral("jx3dps-grayscale-%1").arg(pixmap.cacheKey());
        QPixmap cached;
        if (!QPixmapCache::find(key, &cached)) {
            auto image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
            // Preserve transparency and scaling when desaturating the game artwork.
            for (int y = 0; y < image.height(); ++y) {
                auto *pixels = reinterpret_cast<QRgb *>(image.scanLine(y));
                for (int x = 0; x < image.width(); ++x) {
                    const int gray = qGray(pixels[x]);
                    pixels[x] = qRgba(gray, gray, gray, qAlpha(pixels[x]));
                }
            }
            cached = QPixmap::fromImage(image);
            cached.setDevicePixelRatio(pixmap.devicePixelRatioF());
            QPixmapCache::insert(key, cached);
        }
        pixmap = cached;
    }
    // Trim the frame baked into the artwork before fitting the control's rounded interior.
    const qreal horizontal = pixmap.width() / 16.0;
    const qreal vertical = pixmap.height() / 16.0;
    const auto source = QRectF(pixmap.rect()).adjusted(horizontal, vertical, -horizontal, -vertical);
    QPainterPath clip;
    clip.addRoundedRect(QRectF(bounds), 3, 3);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(clip, Qt::IntersectClip);
    painter.drawPixmap(QRectF(bounds), pixmap, source);
    painter.restore();
}

namespace {
constexpr int ChoiceRowHeight = 44;
constexpr int ChoiceIconSize = 32;

class IconChoiceDelegate : public QStyledItemDelegate
{
public:
    IconChoiceDelegate(QComboBox *combo, QObject *parent) : QStyledItemDelegate(parent), combo_(combo) {}

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override { return { 0, ChoiceRowHeight }; }

    void paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setFont(option.font);
        if (!(option.state & QStyle::State_Enabled) || !(index.flags() & Qt::ItemIsEnabled)) p->setOpacity(.45);
        if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver)) {
            p->setPen(Qt::NoPen);
            p->setBrush(option.palette.highlight());
            p->drawRoundedRect(option.rect.adjusted(4, 2, -4, -2), 6, 6);
        }
        const QRect iconBounds(option.rect.left() + 10, option.rect.top() + (option.rect.height() - ChoiceIconSize) / 2,
                               ChoiceIconSize, ChoiceIconSize);
        const auto icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            PaintGameIcon(*p, iconBounds, icon);
        } else {
            p->setPen(Qt::NoPen);
            p->setBrush(option.palette.alternateBase());
            p->drawRoundedRect(iconBounds, 4, 4);
            p->setPen(QPen(option.palette.placeholderText().color(), 1.5, Qt::SolidLine, Qt::RoundCap));
            p->drawLine(iconBounds.center() - QPoint(6, 0), iconBounds.center() + QPoint(6, 0));
        }
        const auto textBounds = option.rect.adjusted(52, 0, -34, 0);
        p->setPen(option.palette.text().color());
        p->drawText(textBounds, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                    option.fontMetrics.elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight, textBounds.width()));
        const auto committed = combo_->model()->index(combo_->currentIndex(), combo_->modelColumn(), combo_->rootModelIndex());
        if (index == committed) {
            p->setPen(QPen(theme::AccentColor(combo_), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            const QPoint center(option.rect.right() - 18, option.rect.center().y());
            p->drawPolyline(QPolygon{center + QPoint(-5, 0), center + QPoint(-1, 4), center + QPoint(6, -4)});
        }
        p->restore();
    }
private:
    QComboBox *combo_;
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
    PaintGameIcon(painter, rect().adjusted(4, 4, -4, -4), icon(), !isChecked());
    if (!isChecked()) {
        painter.fillRect(rect().adjusted(4, 4, -4, -4), QColor(0, 0, 0, 45));
    }
    Frame(painter, rect(), isChecked(), hasFocus(), theme::AccentColor(this));
}

IconComboBox::IconComboBox(const QString &category, QWidget *parent, bool hasEmptyChoice) :
    QComboBox(parent), m_hasEmptyChoice(hasEmptyChoice)
{
    setAccessibleName(category);
    setFixedSize(sizeHint());
    // Host combo-box styles must not shrink the icon in a tab corner.
    if (!hasEmptyChoice)
        setStyleSheet(QStringLiteral("QComboBox { min-height: 38px; max-height: 38px; padding: 0; border: 0; }"));
    setCursor(Qt::PointingHandCursor);
    m_popup = new ChoicePopup(this);
    auto *layout = new QVBoxLayout(m_popup);
    layout->setContentsMargins(2, 2, 2, 2);
    auto *choices = m_choices = new QListView(m_popup);
    choices->setObjectName(QStringLiteral("iconChoices"));
    choices->setViewMode(QListView::ListMode);
    choices->setFlow(QListView::TopToBottom);
    choices->setWrapping(false);
    choices->setMovement(QListView::Static);
    choices->setResizeMode(QListView::Adjust);
    choices->setSpacing(0);
    choices->setWordWrap(false);
    choices->setUniformItemSizes(true);
    choices->setFrameShape(QFrame::NoFrame);
    choices->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    choices->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    choices->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    choices->setMouseTracking(true);
    choices->setAccessibleName(category + QStringLiteral("选项"));
    choices->setStyleSheet(QStringLiteral("QListView#iconChoices { background: transparent; border: 0; padding: 0; outline: 0; }"));
    // Keep QComboBox::view() compatible for keyboard/configuration callers;
    // the view itself is still hosted by our pre-laid-out popup.
    setView(choices);
    choices->setParent(m_popup);
    choices->setItemDelegate(new IconChoiceDelegate(this, choices));
    layout->addWidget(choices);
    m_popup->Watch(choices);
    m_popup->acceptChoice = [this] { AcceptChoice(); };
    connect(choices, &QListView::clicked, this, [this] { AcceptChoice(); });
    connect(this, &QComboBox::currentIndexChanged, this, [this, category] {
        const auto description = currentData(Qt::ToolTipRole).toString();
        setToolTip(category + QStringLiteral("：") +
                   (description.startsWith(currentText()) ? description : currentText() + '\n' + description));
    });
}

void IconComboBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }
    const bool empty = m_hasEmptyChoice && currentIndex() == 0;
    const auto icon = itemIcon(empty ? 1 : currentIndex());
    if (empty) {
        painter.setOpacity(isEnabled() ? 0.38 : 0.2);
    }
    PaintGameIcon(painter, rect().adjusted(4, 4, -4, -4), icon, empty);
    painter.setOpacity(isEnabled() ? 1 : 0.4);
    Frame(painter, rect(), currentIndex() >= 0 && !empty, hasFocus(),
          theme::AccentColor(this), false);
}

void IconComboBox::showPopup()
{
    if (!isEnabled() || count() == 0 || m_popup->isVisible()) return;
    m_choices->setModel(model());
    m_choices->setRootIndex(rootModelIndex());
    m_choices->setModelColumn(modelColumn());
    const auto selected = model()->index(currentIndex(), modelColumn(), rootModelIndex());
    m_choices->setCurrentIndex(selected);
    m_popup->ensurePolished();
    m_choices->ensurePolished();
    int textWidth = 0;
    for (int index = 0; index < count(); ++index)
        textWidth = std::max(textWidth, m_choices->fontMetrics().horizontalAdvance(itemText(index)));
    const int width = std::clamp(textWidth + 108, 220, 360);
    const auto margins = m_popup->layout()->contentsMargins();
    const int chrome = 2 * (m_popup->frameWidth() + m_choices->frameWidth()) + margins.top() + margins.bottom();
    m_popup->Open(this, { width, std::min(8, count()) * ChoiceRowHeight + chrome },
                  m_choices, ChoicePopup::Below, ChoicePopup::Expand);
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
