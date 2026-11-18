#include "talent_panel.h"

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>
#include "controls/choice_popup.h"
#include "game_icons.h"
#include "controls/theme/accent_color.h"

static void InitTalentData() { Q_INIT_RESOURCE(desktop_data); }

namespace desktop {
namespace {
const QJsonObject &TalentCatalog() {
    static const auto data = [] {
        InitTalentData();
        QFile file(QStringLiteral(":/desktop/talents.json"));
        if (!file.open(QIODevice::ReadOnly)) throw std::runtime_error("Missing current talent catalog");
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    return data;
}

class TalentButton final : public QToolButton {
public:
    explicit TalentButton(QWidget *parent) : QToolButton(parent) {
        setFixedSize(56, 60);
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::PointingHandCursor);
    }

    QIcon artwork;
    QString caption;
    int tier = 0;
    int selectedCount = 0;
    bool candidate = false;
    QSize sizeHint() const override { return {56, 60}; }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!isEnabled()) painter.setOpacity(0.35);
        if (underMouse() || isDown()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(isDown() ? "#253e54" : "#333943"));
            painter.drawRoundedRect(rect(), 6, 6);
        }
        const QRect circle((width() - 40) / 2, 2, 40, 40);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#181c24"));
        painter.drawEllipse(circle);
        painter.save();
        QPainterPath clip;
        clip.addEllipse(QRectF(circle.adjusted(1, 1, -1, -1)));
        painter.setClipPath(clip);
        if (!candidate && !selectedCount) painter.setOpacity(painter.opacity() * 0.38);
        PaintGameIcon(painter, circle.adjusted(1, 1, -1, -1), artwork, !candidate && !selectedCount);
        painter.restore();
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(selectedCount ? theme::AccentColor(this) : QColor("#56565f"), selectedCount ? 1.5 : 1));
        painter.drawEllipse(circle.adjusted(0, 0, -1, -1));
        if (hasFocus()) {
            painter.setPen(QPen(theme::AccentColor(this).lighter(115), 1));
            painter.drawEllipse(circle.adjusted(-2, -2, 1, 1));
        }
        auto font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.setPen(QColor(selectedCount || candidate ? "#f4f5f7" : "#a3a6ad"));
        painter.drawText(QRect(0, 43, width(), 17), Qt::AlignCenter,
                         painter.fontMetrics().elidedText(caption, Qt::ElideRight, width() - 2));
        if (tier) {
            font.setPixelSize(10);
            painter.setFont(font);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#20242c"));
            const QRect badge(circle.left() - 7, 1, 16, 16);
            painter.drawEllipse(badge);
            painter.setPen(QColor("#b6c3d8"));
            painter.drawText(badge, Qt::AlignCenter, QString::number(tier));
        }
        if (selectedCount > 1) {
            painter.setPen(QColor("#ffb23e"));
            painter.drawText(QRect(circle.right() - 6, 0, 15, 16), Qt::AlignCenter, QString::number(selectedCount));
        }
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            click();
            event->accept();
        } else if (candidate && (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)) {
            focusNextPrevChild(event->key() == Qt::Key_Down);
            event->accept();
        } else QToolButton::keyPressEvent(event);
    }
};
} // namespace

struct TalentPanel::Impl {
    struct Slot { TalentButton *button; int group; QString selected; };
    TalentPanel *panel;
    Specialization specialization;
    QJsonArray groups;
    std::vector<Slot> talentSlots;
    ChoicePopup *popup;
    QScrollArea *scroll;
    QWidget *choices;
    QVBoxLayout *choiceLayout;
    QWidget *legacy;
    QVBoxLayout *legacyLayout;
    QToolButton *legacyToggle;
    QList<QCheckBox *> legacyInputs;
    QWidget *headerWidget;
    QGridLayout *slotsLayout;
    int activeTier = -1;

    Impl(TalentPanel *host, Specialization selected) : panel(host), specialization(selected) {
        const auto profile = TalentCatalog().value("profiles").toObject().value(QString::number(static_cast<int>(selected))).toObject();
        groups = profile.value("groups").toArray();
        auto *layout = new QVBoxLayout(panel);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(4);
        headerWidget = new QWidget(panel);
        headerWidget->setObjectName(QStringLiteral("talentHeader%1").arg(static_cast<int>(selected)));
        auto *header = new QHBoxLayout(headerWidget);
        header->setContentsMargins(0, 0, 0, 0);
        auto *title = new QLabel(QStringLiteral("9\u91cd\u5947\u7a74 \u00b7 \u9884\u89c8"), panel);
        title->setStyleSheet(QStringLiteral("font-size: 11px; color: #a3a6ad;"));
        title->setToolTip(TalentCatalog().value("title").toString() + " / " + TalentPanel::DataVersion());
        header->addWidget(title, 1);
        auto *clear = new QToolButton(panel);
        clear->setObjectName(QStringLiteral("clearTalentPreview"));
        clear->setText(QStringLiteral("\u6e05\u7a7a"));
        clear->setToolTip(QStringLiteral("\u6e05\u7a7a\u65b0\u7248\u5947\u7a74\u914d\u88c5"));
        QObject::connect(clear, &QToolButton::clicked, panel, [host] { host->ClearSelection(); });
        header->addWidget(clear);
        layout->addWidget(headerWidget);
        auto *grid = slotsLayout = new QGridLayout;
        grid->setSpacing(4);
        grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        layout->addLayout(grid);
        const auto slotGroups = profile.value("slot_groups").toArray();
        for (int index = 0; index < slotGroups.size(); ++index) {
            auto *button = new TalentButton(panel);
            button->tier = index + 1;
            button->setObjectName(QStringLiteral("talentTier%1").arg(index + 1));
            talentSlots.push_back({button, slotGroups[index].toInt(), {}});
            grid->addWidget(button, index / 3, index % 3);
            QObject::connect(button, &QToolButton::clicked, panel, [this, index] { Open(index); });
        }
        legacyToggle = new QToolButton(panel);
        legacyToggle->setObjectName(QStringLiteral("legacyTalentToggle"));
        legacyToggle->setCheckable(true);
        header->insertWidget(1, legacyToggle);
        legacy = new QWidget(panel);
        legacyLayout = new QVBoxLayout(legacy);
        legacyLayout->setContentsMargins(2, 2, 2, 2);
        legacyLayout->setSpacing(2);
        legacy->hide();
        layout->addWidget(legacy);
        QObject::connect(legacyToggle, &QToolButton::toggled, legacy, &QWidget::setVisible);
        layout->addStretch();
        UpdateLegacy();

        popup = new ChoicePopup(panel);
        popup->setObjectName(QStringLiteral("talentPopup"));
        popup->setStyleSheet(QStringLiteral(
            "QFrame#talentPopup { background: #232327; border: 1px solid #333339; border-radius: 8px; }"
            "QScrollArea { background: transparent; border: 0; }"
            "QWidget#talentChoices { background: #232327; }"));
        auto *popupLayout = new QVBoxLayout(popup);
        popupLayout->setContentsMargins(4, 4, 4, 4);
        scroll = new QScrollArea(popup);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidgetResizable(true);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setFocusPolicy(Qt::NoFocus);
        choices = new QWidget(scroll);
        choices->setObjectName(QStringLiteral("talentChoices"));
        choiceLayout = new QVBoxLayout(choices);
        choiceLayout->setContentsMargins(4, 4, 4, 4);
        choiceLayout->setSpacing(4);
        choiceLayout->setAlignment(Qt::AlignTop);
        scroll->setWidget(choices);
        popupLayout->addWidget(scroll);
        popup->acceptChoice = [this] {
            auto *button = qobject_cast<QToolButton *>(QApplication::focusWidget());
            if (button && popup->isAncestorOf(button)) button->click();
        };
        Refresh();
    }

    QJsonArray Candidates(int index) const { return groups[talentSlots[index].group].toObject().value("choices").toArray(); }
    bool Mixed(int index) const { return groups[talentSlots[index].group].toObject().value("kind") == QJsonValue("mixed"); }
    QJsonObject Entry(int index, const QString &id) const {
        for (const auto &value : Candidates(index)) {
            const auto entry = value.toObject();
            if (entry.value("id").toString() == id) return entry;
        }
        return {};
    }
    bool UsedElsewhere(int index, const QString &id) const {
        for (int other = 0; other < static_cast<int>(talentSlots.size()); ++other)
            if (other != index && talentSlots[other].group == talentSlots[index].group && talentSlots[other].selected == id) return true;
        return false;
    }
    void UpdateLegacy() {
        int count = 0;
        for (auto *input : legacyInputs) if (input->isChecked()) ++count;
        legacyToggle->setText(QStringLiteral("\u65e7\u89c4\u5219(%1)").arg(count));
        legacyToggle->setToolTip(QStringLiteral("\u65e7\u7248\u6218\u6597\u89c4\u5219\u7684\u5947\u7a74\u6a21\u62df\u9009\u9879"));
    }
    void Refresh() {
        for (int index = 0; index < static_cast<int>(talentSlots.size()); ++index) {
            auto &slot = talentSlots[index];
            const auto selected = Entry(index, slot.selected);
            const auto preview = selected.isEmpty() ? Candidates(index)[0].toObject() : selected;
            auto *button = slot.button;
            button->artwork = GameIcon(preview.value("icon").toInt());
            button->selectedCount = slot.selected.isEmpty() ? 0 : 1;
            button->caption = slot.selected.isEmpty() ? QStringLiteral("\u672a\u9009\u62e9") : selected.value("name").toString();
            button->setAccessibleName(QStringLiteral("\u7b2c %1 \u91cd\u5947\u7a74\uff1a%2").arg(index + 1).arg(button->caption));
            button->setToolTip(button->accessibleName() + '\n' + selected.value("desc").toString() +
                (Mixed(index) ? QStringLiteral("\n\u6df7\u9009\u6c60\uff1a12\u90093\uff0c\u4e0d\u53ef\u91cd\u590d") : QString{}));
            button->update();
        }
    }
    void Select(int index, const QString &id) {
        if (!id.isEmpty() && (Entry(index, id).isEmpty() || UsedElsewhere(index, id))) return;
        talentSlots[index].selected = id;
        popup->hide();
        Refresh();
    }
    void Open(int index) {
        if (!panel->isEnabled()) return;
        if (popup->isVisible() && activeTier == index) { popup->hide(); return; }
        popup->hide();
        activeTier = index;
        while (auto *item = choiceLayout->takeAt(0)) {
            if (item->widget()) delete item->widget();
            else if (item->layout()) {
                while (auto *child = item->layout()->takeAt(0)) { delete child->widget(); delete child; }
            }
            delete item;
        }
        auto *heading = new QLabel(Mixed(index) ? QStringLiteral("\u6df7\u9009\u5947\u7a74 \u00b7 12\u90093")
            : QStringLiteral("\u7b2c %1 \u91cd").arg(index + 1), choices);
        heading->setAlignment(Qt::AlignCenter);
        choiceLayout->addWidget(heading);
        auto *grid = new QGridLayout;
        grid->setSpacing(4);
        choiceLayout->addLayout(grid);
        const int columns = Mixed(index) ? 3 : 1;
        int position = 0;
        QWidget *focus = nullptr;
        for (const auto &value : Candidates(index)) {
            const auto entry = value.toObject();
            const auto id = entry.value("id").toString();
            auto *button = new TalentButton(choices);
            button->setFixedWidth(56);
            button->candidate = true;
            button->artwork = GameIcon(entry.value("icon").toInt());
            button->caption = entry.value("name").toString();
            button->selectedCount = talentSlots[index].selected == id ? 1 : 0;
            button->setObjectName(QStringLiteral("chooseTalent") + id);
            button->setAccessibleName(button->caption);
            button->setToolTip(button->caption + '\n' + entry.value("desc").toString());
            button->setEnabled(!UsedElsewhere(index, id));
            if (!button->isEnabled()) button->setToolTip(button->toolTip() + QStringLiteral("\n\u5df2\u5728\u5176\u4ed6\u6df7\u9009\u69fd\u9009\u62e9"));
            grid->addWidget(button, position / columns, position % columns, Qt::AlignHCenter);
            ++position;
            QObject::connect(button, &QToolButton::clicked, panel, [this, index, id] { Select(index, id); });
            popup->Watch(button);
            if (button->isEnabled() && (!focus || button->selectedCount)) focus = button;
        }
        auto *clear = new QToolButton(choices);
        clear->setFocusPolicy(Qt::StrongFocus);
        clear->setText(QStringLiteral("\u6e05\u7a7a\u672c\u91cd"));
        clear->setObjectName(QStringLiteral("clearTalentTier"));
        choiceLayout->addWidget(clear);
        QObject::connect(clear, &QToolButton::clicked, panel, [this, index] { Select(index, {}); });
        popup->Watch(clear);
        const auto size = choiceLayout->sizeHint() + QSize(24, 20);
        popup->Open(talentSlots[index].button, {std::max(Mixed(index) ? 210 : 100, size.width()), size.height()},
                    focus ? focus : clear, ChoicePopup::Below, ChoicePopup::Expand);
        if (focus) scroll->ensureWidgetVisible(focus);
    }
};

TalentPanel::TalentPanel(Specialization specialization, QWidget *parent) : QWidget(parent), d(std::make_unique<Impl>(this, specialization)) {}
TalentPanel::~TalentPanel() = default;

void TalentPanel::SetColumns(int columns) {
    columns = std::max(1, columns);
    for (const auto &slot : d->talentSlots) d->slotsLayout->removeWidget(slot.button);
    d->slotsLayout->setAlignment(columns == 9 ? Qt::AlignTop : Qt::AlignTop | Qt::AlignLeft);
    for (int column = 0; column < std::max(columns, d->slotsLayout->columnCount()); ++column)
        d->slotsLayout->setColumnStretch(column, columns == 9 && column < columns ? 1 : 0);
    for (int index = 0; index < static_cast<int>(d->talentSlots.size()); ++index)
        d->slotsLayout->addWidget(d->talentSlots[index].button, index / columns, index % columns, Qt::AlignHCenter);
    updateGeometry();
}
QWidget *TalentPanel::HeaderWidget() const { return d->headerWidget; }
QWidget *TalentPanel::TakeHeader() {
    layout()->removeWidget(d->headerWidget);
    return d->headerWidget;
}

QCheckBox *TalentPanel::AddTalent(const QString &name, const QString &key) {
    auto *input = new QCheckBox(name, d->legacy);
    input->setObjectName(key);
    input->setToolTip(name + '\n' + GameDescription(d->specialization, name));
    d->legacyInputs << input;
    d->legacyLayout->addWidget(input);
    connect(input, &QCheckBox::toggled, this, [this] { d->UpdateLegacy(); });
    return input;
}

QString TalentPanel::DataVersion() { return TalentCatalog().value("version").toString(); }
QJsonArray TalentPanel::Selection() const {
    QJsonArray result;
    for (const auto &slot : d->talentSlots) result.append(slot.selected);
    return result;
}
bool TalentPanel::HasSelection() const {
    return std::any_of(d->talentSlots.begin(), d->talentSlots.end(), [](const auto &slot) { return !slot.selected.isEmpty(); });
}
bool TalentPanel::ValidateSelection(const QJsonArray &selection, QString &error) const {
    if (selection.size() != static_cast<int>(d->talentSlots.size())) {
        error = QStringLiteral("\u5947\u7a74\u914d\u88c5\u5fc5\u987b\u4e3a9\u91cd"); return false;
    }
    for (int index = 0; index < selection.size(); ++index) {
        if (!selection[index].isString() || (!selection[index].toString().isEmpty() && d->Entry(index, selection[index].toString()).isEmpty())) {
            error = QStringLiteral("\u7b2c %1 \u91cd\u5947\u7a74\u65e0\u6548").arg(index + 1); return false;
        }
        if (selection[index].toString().isEmpty()) continue;
        for (int previous = 0; previous < index; ++previous)
            if (d->talentSlots[previous].group == d->talentSlots[index].group && selection[previous] == selection[index]) {
                error = QStringLiteral("\u6df7\u9009\u5947\u7a74\u4e0d\u80fd\u91cd\u590d\u9009\u62e9"); return false;
            }
    }
    return true;
}
void TalentPanel::RestoreSelection(const QJsonArray &selection) {
    QString error;
    if (!ValidateSelection(selection, error)) throw std::invalid_argument(error.toStdString());
    d->popup->hide();
    for (int index = 0; index < selection.size(); ++index) d->talentSlots[index].selected = selection[index].toString();
    d->Refresh();
}
void TalentPanel::ClearSelection() {
    d->popup->hide();
    for (auto &slot : d->talentSlots) slot.selected.clear();
    d->Refresh();
}

void TalentPanel::hideEvent(QHideEvent *event) { d->popup->hide(); QWidget::hideEvent(event); }
void TalentPanel::changeEvent(QEvent *event) {
    if (event->type() == QEvent::EnabledChange && !isEnabled()) d->popup->hide();
    QWidget::changeEvent(event);
}
} // namespace desktop
