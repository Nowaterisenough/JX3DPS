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

#include "resources.h"

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
    static const auto current = [] {
        QFile file(":/desktop/skill_icons50.json");
        file.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    auto ENTRIES = current.value(Text(Info(specialization).code)).toObject();
    if (ENTRIES.isEmpty() && (Info(specialization).code == "mw" || Info(specialization).code == "tx")) {
        ENTRIES = data.value(Info(specialization).code == "mw" ? "1" : "0").toObject();
    }
    if (name.endsWith("(DOT)")) {
        name.chop(5);
    }
    if (name.endsWith("持续伤害")) {
        name.chop(4);
    }
    if (name.endsWith("()")) {
        name.chop(2);
    }
    name               = name.section(QStringLiteral("（"), 0, 0);
    if (ENTRIES.contains(name)) {
        return ENTRIES.value(name).toObject();
    }
    name.replace(QStringLiteral("持续伤害"), QStringLiteral("Dot"));
    if (ENTRIES.contains(name)) {
        return ENTRIES.value(name).toObject();
    }
    const auto parts = name.split(QStringLiteral("·"));
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (ENTRIES.contains(*it)) {
            return ENTRIES.value(*it).toObject();
        }
    }
    return {};
}

void Frame(QPainter &painter, const QRect &rect, bool selected, bool focus, const QColor &accent, bool show_check = true)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(selected ? accent : QColor(72, 77, 88), selected ? 1.5 : 1));
    painter.drawRoundedRect(rect.adjusted(1, 1, -2, -2), 5, 5);
    if (selected && show_check) {
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
    if (pixmap.isNull()) {
        return;
    }
    if (grayscale) {
        const auto KEY = QStringLiteral("jx3dps-grayscale-%1").arg(pixmap.cacheKey());
        QPixmap    cached;
        if (!QPixmapCache::find(KEY, &cached)) {
            auto image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
            // Preserve transparency and scaling when desaturating the game artwork.
            for (int y = 0; y < image.height(); ++y) {
                auto *pixels = reinterpret_cast<QRgb *>(image.scanLine(y));
                for (int x = 0; x < image.width(); ++x) {
                    const int GRAY = qGray(pixels[x]);
                    pixels[x]      = qRgba(GRAY, GRAY, GRAY, qAlpha(pixels[x]));
                }
            }
            cached = QPixmap::fromImage(image);
            cached.setDevicePixelRatio(pixmap.devicePixelRatioF());
            QPixmapCache::insert(KEY, cached);
        }
        pixmap = cached;
    }
    // Trim the frame baked into the artwork before fitting the control's rounded interior.
    const qreal  HORIZONTAL = pixmap.width() / 16.0;
    const qreal  VERTICAL   = pixmap.height() / 16.0;
    const auto   SOURCE     = QRectF(pixmap.rect()).adjusted(HORIZONTAL, VERTICAL, -HORIZONTAL, -VERTICAL);
    QPainterPath clip;
    clip.addRoundedRect(QRectF(bounds), 3, 3);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setClipPath(clip, Qt::IntersectClip);
    painter.drawPixmap(QRectF(bounds), pixmap, SOURCE);
    painter.restore();
}

namespace {
constexpr int ChoiceRowHeight = 44;
constexpr int ChoiceIconSize  = 32;

class IconChoiceDelegate : public QStyledItemDelegate
{
public:
    IconChoiceDelegate(QComboBox *combo, QObject *parent) : QStyledItemDelegate(parent), combo_(combo) { }

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const override { return { 0, ChoiceRowHeight }; }

    void paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        p->save();
        p->setRenderHint(QPainter::Antialiasing);
        p->setFont(option.font);
        if (!(option.state & QStyle::State_Enabled) || !(index.flags() & Qt::ItemIsEnabled)) {
            p->setOpacity(.45);
        }
        if (option.state & (QStyle::State_Selected | QStyle::State_MouseOver) != 0u) {
            p->setPen(Qt::NoPen);
            p->setBrush(option.palette.highlight());
            p->drawRoundedRect(option.rect.adjusted(4, 2, -4, -2), 6, 6);
        }
        const QRect ICON_BOUNDS(option.rect.left() + 10, option.rect.top() + ((option.rect.height() - ChoiceIconSize) / 2), ChoiceIconSize, ChoiceIconSize);
        const auto ICON = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!ICON.isNull()) {
            PaintGameIcon(*p, ICON_BOUNDS, ICON);
        } else {
            p->setPen(Qt::NoPen);
            p->setBrush(option.palette.alternateBase());
            p->drawRoundedRect(ICON_BOUNDS, 4, 4);
            p->setPen(QPen(option.palette.placeholderText().color(), 1.5, Qt::SolidLine, Qt::RoundCap));
            p->drawLine(ICON_BOUNDS.center() - QPoint(6, 0), ICON_BOUNDS.center() + QPoint(6, 0));
        }
        const auto TEXT_BOUNDS = option.rect.adjusted(52, 0, -34, 0);
        p->setPen(option.palette.text().color());
        p->drawText(TEXT_BOUNDS,
                    Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
                    option.fontMetrics.elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight, TEXT_BOUNDS.width()));
        const auto COMMITTED = combo_->model()->index(combo_->currentIndex(), combo_->modelColumn(), combo_->rootModelIndex());
        if (index == COMMITTED) {
            p->setPen(QPen(theme::AccentColor(combo_), 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            const QPoint CENTER(option.rect.right() - 18, option.rect.center().y());
            p->drawPolyline(QPolygon{ CENTER + QPoint(-5, 0), CENTER + QPoint(-1, 4), CENTER + QPoint(6, -4) });
        }
        p->restore();
    }

private:
    QComboBox *combo_;
};
} // namespace

static QIcon AtlasIcon(const QString &key)
{
    static const auto index = [] {
        QFile f(":/desktop/season50_icons.json");
        f.open(QIODevice::ReadOnly);
        return QJsonDocument::fromJson(f.readAll()).object();
    }();
    static const QPixmap sheet(":/desktop/season50_icons.png");
    const int            cell = index.value(key).toInt(-1);
    return cell < 0 ? QIcon{} : QIcon(sheet.copy((cell % 24) * 48, (cell / 24) * 48, 48, 48));
}
QIcon GameIcon(int id)
{
    static const int initialized = Resources::InitResources();
    Q_UNUSED(initialized);
    const auto path = QStringLiteral(":/resources/images/JX3/Icons/%1.png").arg(id);
    return QFile::exists(path) ? QIcon(path) : AtlasIcon(QString::number(id));
}

QIcon GameIcon(Specialization specialization, const QString &name)
{
    const auto ENTRY = Entry(specialization, name);
    if (name == ClassName(specialization) && static_cast<int>(specialization) >= 0) {
        return AtlasIcon(Text(School(specialization).code));
    }
    return ENTRY.isEmpty() ? QIcon{} : GameIcon(ENTRY.value("Icon").toInt());
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
    setFixedSize(IconCheckBox::sizeHint());
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

IconComboBox::IconComboBox(const QString &category, QWidget *parent, bool has_empty_choice) :
    QComboBox(parent), m_hasEmptyChoice(has_empty_choice)
{
    setAccessibleName(category);
    setFixedSize(IconComboBox::sizeHint());
    // Host combo-box styles must not shrink the icon in a tab corner.
    if (!has_empty_choice) {
        setStyleSheet(QStringLiteral("QComboBox { min-height: 38px; max-height: 38px; padding: 0; border: 0; }"));
    }
    setCursor(Qt::PointingHandCursor);
    m_popup      = new ChoicePopup(this);
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
    choices->setStyleSheet(
        QStringLiteral("QListView#iconChoices { background: transparent; border: 0; padding: 0; outline: 0; }"));
    // Keep QComboBox::view() compatible for keyboard/configuration callers;
    // the view itself is still hosted by our pre-laid-out popup.
    setView(choices);
    choices->setParent(m_popup);
    choices->setItemDelegate(new IconChoiceDelegate(this, choices));
    layout->addWidget(choices);
    m_popup->Watch(choices);
    m_popup->acceptChoice = [this] {
        AcceptChoice();
    };
    connect(choices, &QListView::clicked, this, [this] { AcceptChoice(); });
    connect(this, &QComboBox::currentIndexChanged, this, [this, category] {
        const auto DESCRIPTION = currentData(Qt::ToolTipRole).toString();
        setToolTip(category + QStringLiteral("：") +
                   (DESCRIPTION.startsWith(currentText()) ? DESCRIPTION : currentText() + '\n' + DESCRIPTION));
    });
}

void IconComboBox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!isEnabled()) {
        painter.setOpacity(0.4);
    }
    const bool EMPTY = m_hasEmptyChoice && currentIndex() == 0;
    const auto ICON  = itemIcon(EMPTY ? 1 : currentIndex());
    if (EMPTY) {
        painter.setOpacity(isEnabled() ? 0.38 : 0.2);
    }
    PaintGameIcon(painter, rect().adjusted(4, 4, -4, -4), ICON, EMPTY);
    painter.setOpacity(isEnabled() ? 1 : 0.4);
    Frame(painter, rect(), currentIndex() >= 0 && !EMPTY, hasFocus(), theme::AccentColor(this), false);
}

void IconComboBox::showPopup()
{
    if (!isEnabled() || count() == 0 || m_popup->isVisible()) {
        return;
    }
    m_choices->setModel(model());
    m_choices->setRootIndex(rootModelIndex());
    m_choices->setModelColumn(modelColumn());
    const auto SELECTED = model()->index(currentIndex(), modelColumn(), rootModelIndex());
    m_choices->setCurrentIndex(SELECTED);
    m_popup->ensurePolished();
    m_choices->ensurePolished();
    int text_width = 0;
    for (int index = 0; index < count(); ++index) {
        text_width = std::max(text_width, m_choices->fontMetrics().horizontalAdvance(itemText(index)));
    }
    const int  WIDTH   = std::clamp(text_width + 108, 220, 360);
    const auto MARGINS = m_popup->layout()->contentsMargins();
    const int  CHROME  = (2 * (m_popup->frameWidth() + m_choices->frameWidth())) + MARGINS.top() + MARGINS.bottom();
    m_popup->Open(this, { WIDTH, (std::min(8, count()) * ChoiceRowHeight) + CHROME }, m_choices, ChoicePopup::BELOW, ChoicePopup::EXPAND);
    m_choices->scrollTo(SELECTED);
}

void IconComboBox::hidePopup()
{
    m_popup->hide();
}

void IconComboBox::AcceptChoice()
{
    const auto INDEX = m_choices->currentIndex();
    if (!INDEX.isValid() || !(INDEX.flags() & Qt::ItemIsEnabled) || !(INDEX.flags() & Qt::ItemIsSelectable)) {
        return;
    }
    setCurrentIndex(INDEX.row());
    hidePopup();
    emit activated(currentIndex());
    emit textActivated(currentText());
}

void IconComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setFocus(Qt::MouseFocusReason);
        if (m_popup->isVisible()) {
            hidePopup();
        } else {
            showPopup();
        }
        event->accept();
    } else {
        QComboBox::mousePressEvent(event);
    }
}

void IconComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        event->accept();
    } else {
        QComboBox::mouseReleaseEvent(event);
    }
}

void IconComboBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_F4 ||
        (((event->modifiers() & Qt::AltModifier) != 0u) && (event->key() == Qt::Key_Down || event->key() == Qt::Key_Up)))
    {
        if (m_popup->isVisible()) {
            hidePopup();
        } else {
            showPopup();
        }
        event->accept();
    } else {
        QComboBox::keyPressEvent(event);
    }
}
} // namespace desktop
