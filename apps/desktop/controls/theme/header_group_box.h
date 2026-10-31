#pragma once

#include <QGroupBox>
#include <QHBoxLayout>
#include <QResizeEvent>

namespace desktop {
class HeaderGroupBox final : public QGroupBox {
public:
    using QGroupBox::QGroupBox;
    QHBoxLayout *HeaderActions(int height) {
        headerHeight_ = height;
        if (!header_) {
            header_ = new QWidget(this);
            header_->setObjectName(QStringLiteral("groupHeaderActions"));
            auto *row = new QHBoxLayout(header_);
            row->setContentsMargins(2, 0, 2, 0);
            row->setSpacing(6);
            row->addStretch();
        }
        header_->setGeometry(0, 0, width(), headerHeight_);
        return static_cast<QHBoxLayout *>(header_->layout());
    }
protected:
    void resizeEvent(QResizeEvent *event) override {
        QGroupBox::resizeEvent(event);
        if (header_) header_->setGeometry(0, 0, width(), headerHeight_);
    }
private:
    QWidget *header_ = nullptr;
    int headerHeight_ = 0;
};
} // namespace desktop
