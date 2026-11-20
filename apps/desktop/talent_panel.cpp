#include "talent_panel.h"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include <QApplication>
#include <QCheckBox>
#include <QFile>
#include <QGridLayout>
#include <QHideEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "controls/choice_popup.h"
#include "controls/theme/accent_color.h"

#include "game_icons.h"

static void InitTalentData()
{
    Q_INIT_RESOURCE(desktop_data);
}

namespace desktop {
namespace {
const QJsonObject &TalentCatalog()
{
    static const auto data = [] {
        InitTalentData();
        QFile file(QStringLiteral(":/desktop/talents.json"));
        if (!file.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Missing current talent catalog");
        }
        return QJsonDocument::fromJson(file.readAll()).object();
    }();
    return data;
}

class TalentButton final : public QToolButton
{
public:
    explicit TalentButton(QWidget *parent) : QToolButton(parent)
    {
        setFixedSize(56, 60);
        setFocusPolicy(Qt::StrongFocus);
        setCursor(Qt::PointingHandCursor);
    }

    QIcon   artwork;
    QString caption;
    int     tier          = 0;
    int     selectedCount = 0;
    bool    candidate     = false;

    QSize sizeHint() const override { return { 56, 60 }; }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (!isEnabled()) {
            painter.setOpacity(0.35);
        }
        if (underMouse() || isDown()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(isDown() ? "#253e54" : "#333943"));
            painter.drawRoundedRect(rect(), 6, 6);
        }
        const QRect CIRCLE((width() - 40) / 2, 2, 40, 40);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#181c24"));
        painter.drawEllipse(CIRCLE);
        painter.save();
        QPainterPath clip;
        clip.addEllipse(QRectF(CIRCLE.adjusted(1, 1, -1, -1)));
        painter.setClipPath(clip);
        if (!candidate && (selectedCount == 0)) {
            painter.setOpacity(painter.opacity() * 0.38);
        }
        PaintGameIcon(painter, CIRCLE.adjusted(1, 1, -1, -1), artwork, !candidate && (selectedCount == 0));
        painter.restore();
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen((selectedCount != 0) ? theme::AccentColor(this) : QColor("#56565f"), (selectedCount != 0) ? 1.5 : 1));
        painter.drawEllipse(CIRCLE.adjusted(0, 0, -1, -1));
        if (hasFocus()) {
            painter.setPen(QPen(theme::AccentColor(this).lighter(115), 1));
            painter.drawEllipse(CIRCLE.adjusted(-2, -2, 1, 1));
        }
        auto font = painter.font();
        font.setPixelSize(12);
        painter.setFont(font);
        painter.setPen(QColor((selectedCount != 0) || candidate ? "#f4f5f7" : "#a3a6ad"));
        painter.drawText(QRect(0, 43, width(), 17),
                         Qt::AlignCenter,
                         painter.fontMetrics().elidedText(caption, Qt::ElideRight, width() - 2));
        if (tier != 0) {
            font.setPixelSize(10);
            painter.setFont(font);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor("#20242c"));
            const QRect BADGE(CIRCLE.left() - 7, 1, 16, 16);
            painter.drawEllipse(BADGE);
            painter.setPen(QColor("#b6c3d8"));
            painter.drawText(BADGE, Qt::AlignCenter, QString::number(tier));
        }
        if (selectedCount > 1) {
            painter.setPen(QColor("#ffb23e"));
            painter.drawText(QRect(CIRCLE.right() - 6, 0, 15, 16), Qt::AlignCenter, QString::number(selectedCount));
        }
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            click();
            event->accept();
        } else if (candidate && (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)) {
            focusNextPrevChild(event->key() == Qt::Key_Down);
            event->accept();
        } else {
            QToolButton::keyPressEvent(event);
        }
    }
};
} // namespace

struct TalentPanel::Impl
{
    struct Slot
    {
        TalentButton *button;
        int           group;
        QString       selected;
    };

    TalentPanel       *panel;
    Specialization     specialization;
    QJsonArray         groups;
    std::vector<Slot>  talentSlots;
    ChoicePopup       *popup;
    QScrollArea       *scroll;
    QWidget           *choices;
    QVBoxLayout       *choiceLayout;
    QWidget           *legacy;
    QVBoxLayout       *legacyLayout;
    QToolButton       *legacyToggle;
    QList<QCheckBox *> legacyInputs;
    QWidget           *headerWidget;
    QGridLayout       *slotsLayout;
    int                activeTier = -1;

    Impl(TalentPanel *host, Specialization selected) : panel(host), specialization(selected)
    {
        const auto PROFILE =
            TalentCatalog().value("profiles").toObject().value(QString::number(static_cast<int>(selected))).toObject();
        groups       = PROFILE.value("groups").toArray();
        auto *layout = new QVBoxLayout(panel);
        layout->setContentsMargins(4, 4, 4, 4);
        layout->setSpacing(4);
        headerWidget = new QWidget(panel);
        headerWidget->setObjectName(QStringLiteral("talentHeader%1").arg(static_cast<int>(selected)));
        auto *header = new QHBoxLayout(headerWidget);
        header->setContentsMargins(0, 0, 0, 0);
        auto *title = new QLabel(QStringLiteral("9重奇穴 · 预览"), panel);
        title->setStyleSheet(QStringLiteral("font-size: 11px; color: #a3a6ad;"));
        title->setToolTip(TalentCatalog().value("title").toString() + " / " + TalentPanel::DataVersion());
        header->addWidget(title, 1);
        auto *clear = new QToolButton(panel);
        clear->setObjectName(QStringLiteral("clearTalentPreview"));
        clear->setText(QStringLiteral("清空"));
        clear->setToolTip(QStringLiteral("清空新版奇穴配装"));
        QObject::connect(clear, &QToolButton::clicked, panel, [host] { host->ClearSelection(); });
        header->addWidget(clear);
        layout->addWidget(headerWidget);
        auto *grid = slotsLayout = new QGridLayout;
        grid->setSpacing(4);
        grid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        layout->addLayout(grid);
        const auto SLOT_GROUPS = PROFILE.value("slot_groups").toArray();
        for (int index = 0; index < SLOT_GROUPS.size(); ++index) {
            auto *button = new TalentButton(panel);
            button->tier = index + 1;
            button->setObjectName(QStringLiteral("talentTier%1").arg(index + 1));
            talentSlots.push_back({ button, SLOT_GROUPS[index].toInt(), {} });
            grid->addWidget(button, index / 3, index % 3);
            QObject::connect(button, &QToolButton::clicked, panel, [this, index] { Open(index); });
        }
        legacyToggle = new QToolButton(panel);
        legacyToggle->setObjectName(QStringLiteral("legacyTalentToggle"));
        legacyToggle->setCheckable(true);
        header->insertWidget(1, legacyToggle);
        legacy       = new QWidget(panel);
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
        auto *popup_layout = new QVBoxLayout(popup);
        popup_layout->setContentsMargins(4, 4, 4, 4);
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
        popup_layout->addWidget(scroll);
        popup->acceptChoice = [this] {
            auto *button = qobject_cast<QToolButton *>(QApplication::focusWidget());
            if (button && popup->isAncestorOf(button)) {
                button->click();
            }
        };
        Refresh();
    }

    QJsonArray Candidates(int index) const
    {
        return groups[talentSlots[index].group].toObject().value("choices").toArray();
    }

    bool Mixed(int index) const
    {
        return groups[talentSlots[index].group].toObject().value("kind") == QJsonValue("mixed");
    }

    QJsonObject Entry(int index, const QString &id) const
    {
        for (const auto &value : Candidates(index)) {
            const auto ENTRY = value.toObject();
            if (ENTRY.value("id").toString() == id) {
                return ENTRY;
            }
        }
        return {};
    }

    bool UsedElsewhere(int index, const QString &id) const
    {
        for (int other = 0; std::cmp_less(other, talentSlots.size()); ++other) {
            if (other != index && talentSlots[other].group == talentSlots[index].group && talentSlots[other].selected == id)
            {
                return true;
            }
        }
        return false;
    }

    void UpdateLegacy()
    {
        int count = 0;
        for (const auto *input : legacyInputs) {
            if (input->isChecked()) {
                ++count;
            }
        }
        legacyToggle->setText(QStringLiteral("旧规则(%1)").arg(count));
        legacyToggle->setToolTip(QStringLiteral("旧版战斗规则的奇穴模拟选项"));
    }

    void Refresh()
    {
        for (int index = 0; std::cmp_less(index, talentSlots.size()); ++index) {
            auto      &slot       = talentSlots[index];
            const auto SELECTED   = Entry(index, slot.selected);
            const auto PREVIEW    = SELECTED.isEmpty() ? Candidates(index)[0].toObject() : SELECTED;
            auto      *button     = slot.button;
            button->artwork       = GameIcon(PREVIEW.value("icon").toInt());
            button->selectedCount = slot.selected.isEmpty() ? 0 : 1;
            button->caption = slot.selected.isEmpty() ? QStringLiteral("未选择") : SELECTED.value("name").toString();
            button->setAccessibleName(QStringLiteral("第 %1 重奇穴：%2").arg(index + 1).arg(button->caption));
            button->setToolTip(button->accessibleName() + '\n' + SELECTED.value("desc").toString() +
                               (Mixed(index) ? QStringLiteral("\n混选池：12选3，不可重复") : QString{}));
            button->update();
        }
    }

    void Select(int index, const QString &id)
    {
        if (!id.isEmpty() && (Entry(index, id).isEmpty() || UsedElsewhere(index, id))) {
            return;
        }
        talentSlots[index].selected = id;
        popup->hide();
        Refresh();
    }

    void Open(int index)
    {
        if (!panel->isEnabled()) {
            return;
        }
        if (popup->isVisible() && activeTier == index) {
            popup->hide();
            return;
        }
        popup->hide();
        activeTier = index;
        while (auto *item = choiceLayout->takeAt(0)) {
            if (item->widget() != nullptr) {
                delete item->widget();
            } else if (item->layout() != nullptr) {
                while (const auto *child = item->layout()->takeAt(0)) {
                    delete child->widget();
                    delete child;
                }
            }
            delete item;
        }
        auto *heading =
            new QLabel(Mixed(index) ? QStringLiteral("混选奇穴 · 12选3") : QStringLiteral("第 %1 重").arg(index + 1), choices);
        heading->setAlignment(Qt::AlignCenter);
        choiceLayout->addWidget(heading);
        auto *grid = new QGridLayout;
        grid->setSpacing(4);
        choiceLayout->addLayout(grid);
        const int COLUMNS  = Mixed(index) ? 3 : 1;
        int       position = 0;
        QWidget  *focus    = nullptr;
        for (const auto &value : Candidates(index)) {
            const auto ENTRY  = value.toObject();
            const auto ID     = ENTRY.value("id").toString();
            auto      *button = new TalentButton(choices);
            button->setFixedWidth(56);
            button->candidate     = true;
            button->artwork       = GameIcon(ENTRY.value("icon").toInt());
            button->caption       = ENTRY.value("name").toString();
            button->selectedCount = talentSlots[index].selected == ID ? 1 : 0;
            button->setObjectName(QStringLiteral("chooseTalent") + ID);
            button->setAccessibleName(button->caption);
            button->setToolTip(button->caption + '\n' + ENTRY.value("desc").toString());
            button->setEnabled(!UsedElsewhere(index, ID));
            if (!button->isEnabled()) {
                button->setToolTip(button->toolTip() + QStringLiteral("\n已在其他混选槽选择"));
            }
            grid->addWidget(button, position / COLUMNS, position % COLUMNS, Qt::AlignHCenter);
            ++position;
            QObject::connect(button, &QToolButton::clicked, panel, [this, index, ID] { Select(index, ID); });
            popup->Watch(button);
            if (button->isEnabled() && ((focus == nullptr) || (button->selectedCount != 0))) {
                focus = button;
            }
        }
        auto *clear = new QToolButton(choices);
        clear->setFocusPolicy(Qt::StrongFocus);
        clear->setText(QStringLiteral("清空本重"));
        clear->setObjectName(QStringLiteral("clearTalentTier"));
        choiceLayout->addWidget(clear);
        QObject::connect(clear, &QToolButton::clicked, panel, [this, index] { Select(index, {}); });
        popup->Watch(clear);
        const auto SIZE = choiceLayout->sizeHint() + QSize(24, 20);
        popup->Open(talentSlots[index].button,
                    { std::max(Mixed(index) ? 210 : 100, SIZE.width()), SIZE.height() },
                    (focus != nullptr) ? focus : clear,
                    ChoicePopup::BELOW,
                    ChoicePopup::EXPAND);
        if (focus != nullptr) {
            scroll->ensureWidgetVisible(focus);
        }
    }
};

TalentPanel::TalentPanel(Specialization specialization, QWidget *parent) :
    QWidget(parent), d(std::make_unique<Impl>(this, specialization))
{
}

TalentPanel::~TalentPanel() = default;

void TalentPanel::SetColumns(int columns)
{
    columns = std::max(1, columns);
    for (const auto &slot : d->talentSlots) {
        d->slotsLayout->removeWidget(slot.button);
    }
    d->slotsLayout->setAlignment(columns == 9 ? Qt::AlignTop : Qt::AlignTop | Qt::AlignLeft);
    for (int column = 0; column < std::max(columns, d->slotsLayout->columnCount()); ++column) {
        d->slotsLayout->setColumnStretch(column, columns == 9 && column < columns ? 1 : 0);
    }
    for (int index = 0; std::cmp_less(index, d->talentSlots.size()); ++index) {
        d->slotsLayout->addWidget(d->talentSlots[index].button, index / columns, index % columns, Qt::AlignHCenter);
    }
    updateGeometry();
}

QWidget *TalentPanel::HeaderWidget() const
{
    return d->headerWidget;
}

QWidget *TalentPanel::TakeHeader()
{
    layout()->removeWidget(d->headerWidget);
    return d->headerWidget;
}

QCheckBox *TalentPanel::AddTalent(const QString &name, const QString &key)
{
    auto *input = new QCheckBox(name, d->legacy);
    input->setObjectName(key);
    input->setToolTip(name + '\n' + GameDescription(d->specialization, name));
    d->legacyInputs << input;
    d->legacyLayout->addWidget(input);
    connect(input, &QCheckBox::toggled, this, [this] { d->UpdateLegacy(); });
    return input;
}

QString TalentPanel::DataVersion()
{
    return TalentCatalog().value("version").toString();
}

QJsonArray TalentPanel::Selection() const
{
    QJsonArray result;
    for (const auto &slot : d->talentSlots) {
        result.append(slot.selected);
    }
    return result;
}

bool TalentPanel::HasSelection() const
{
    return std::any_of(d->talentSlots.begin(), d->talentSlots.end(), [](const auto &slot) {
        return !slot.selected.isEmpty();
    });
}

bool TalentPanel::ValidateSelection(const QJsonArray &selection, QString &error) const
{
    if (selection.size() != static_cast<int>(d->talentSlots.size())) {
        error = QStringLiteral("奇穴配装必须为9重");
        return false;
    }
    for (int index = 0; index < selection.size(); ++index) {
        if (!selection[index].isString() ||
            (!selection[index].toString().isEmpty() && d->Entry(index, selection[index].toString()).isEmpty()))
        {
            error = QStringLiteral("第 %1 重奇穴无效").arg(index + 1);
            return false;
        }
        if (selection[index].toString().isEmpty()) {
            continue;
        }
        for (int previous = 0; previous < index; ++previous) {
            if (d->talentSlots[previous].group == d->talentSlots[index].group && selection[previous] == selection[index])
            {
                error = QStringLiteral("混选奇穴不能重复选择");
                return false;
            }
        }
    }
    return true;
}

void TalentPanel::RestoreSelection(const QJsonArray &selection)
{
    QString error;
    if (!ValidateSelection(selection, error)) {
        throw std::invalid_argument(error.toStdString());
    }
    d->popup->hide();
    for (int index = 0; index < selection.size(); ++index) {
        d->talentSlots[index].selected = selection[index].toString();
    }
    d->Refresh();
}

void TalentPanel::ClearSelection()
{
    d->popup->hide();
    for (auto &slot : d->talentSlots) {
        slot.selected.clear();
    }
    d->Refresh();
}

void TalentPanel::hideEvent(QHideEvent *event)
{
    d->popup->hide();
    QWidget::hideEvent(event);
}

void TalentPanel::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange && !isEnabled()) {
        d->popup->hide();
    }
    QWidget::changeEvent(event);
}
} // namespace desktop
