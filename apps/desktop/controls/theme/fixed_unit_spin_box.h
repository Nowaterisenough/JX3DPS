#pragma once

#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QResizeEvent>
#include <QStyleOptionFrame>

#include "workbench_theme.h"

namespace desktop {
class FixedUnitDoubleSpinBox final : public QDoubleSpinBox
{
    class Editor final : public QLineEdit
    {
    public:
        explicit Editor(QWidget *parent) : QLineEdit(parent)
        {
            unit_ = new QLabel(this);
            setObjectName(QStringLiteral("qt_spinbox_lineedit"));
            unit_->setObjectName(QStringLiteral("spinBoxUnit"));
            unit_->setAttribute(Qt::WA_TransparentForMouseEvents);
            unit_->setFocusPolicy(Qt::NoFocus);
            unit_->setTextInteractionFlags(Qt::NoTextInteraction);
            unit_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            unit_->setStyleSheet(
                QStringLiteral("QLabel { color: %1; background: transparent; } QLabel:disabled { color: %2; }")
                    .arg(QString::fromLatin1(theme::Muted), QColor(theme::Muted).darker(140).name()));
        }

        void SetUnit(const QString &unit)
        {
            unit_->setText(unit);
            unit_->setVisible(!unit.isEmpty());
            UpdateUnitGeometry();
        }

    protected:
        void resizeEvent(QResizeEvent *event) override
        {
            QLineEdit::resizeEvent(event);
            UpdateUnitGeometry();
        }

        void changeEvent(QEvent *event) override
        {
            QLineEdit::changeEvent(event);
            if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
                UpdateUnitGeometry();
            }
        }

    private:
        void UpdateUnitGeometry()
        {
            if (unit_ == nullptr) {
                return;
            }
            unit_->setFont(font());
            const int WIDTH = unit_->fontMetrics().horizontalAdvance(unit_->text());
            // The unit lives outside the editable text, including its selection and IME area.
            setTextMargins(0, 0, (WIDTH != 0) ? WIDTH + 6 : 0, 0);
            QStyleOptionFrame frame;
            initStyleOption(&frame);
            const auto CONTENTS = style()->subElementRect(QStyle::SE_LineEditContents, &frame, this).adjusted(2, 0, -2, 0);
            unit_->setGeometry(CONTENTS.right() - WIDTH + 1, CONTENTS.top(), WIDTH, CONTENTS.height());
        }

        QLabel *unit_ = nullptr;
    };

public:
    explicit FixedUnitDoubleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent), editor_(new Editor(this))
    {
        setLineEdit(editor_);
    }

    void SetUnit(const QString &unit) { editor_->SetUnit(unit); }

private:
    Editor *editor_;
};
} // namespace desktop
