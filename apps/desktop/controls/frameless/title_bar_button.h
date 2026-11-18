#ifndef TFA_UI_CONTROLS_TITLE_BAR_BUTTON_H
#define TFA_UI_CONTROLS_TITLE_BAR_BUTTON_H

#include <QPushButton>
#include <QVariantAnimation>


class TitleBarButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor BackgroundColor READ BackgroundColor WRITE SetBackgroundColor)

public:
    enum class ButtonType
    {
        MINIMIZE,
        MAXIMIZE,
        RESTORE,
        CLOSE
    };

    explicit TitleBarButton(ButtonType type, QWidget *parent = nullptr);
    virtual ~TitleBarButton() override;

    QColor BackgroundColor() const { return _backgroundColor; }

    void SetBackgroundColor(const QColor &color);
    void StartAnimation(const QColor &endBackgroundColor, const QColor &endTextColor);

    void SetHovered(bool hovered);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QVariantAnimation *_backgroundAnimation = nullptr;
    QVariantAnimation *_textAnimation       = nullptr;
    bool               _isCloseButton       = false;
    ButtonType         _type                = ButtonType::MINIMIZE;
    QColor             _hoverColor;
    QColor             _pressColor;
    QColor             _backgroundColor;
    QColor             _textColor;
};


#endif // TFA_UI_CONTROLS_TITLE_BAR_BUTTON_H
