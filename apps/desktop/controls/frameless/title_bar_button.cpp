#include "title_bar_button.h"

#include <QFontDatabase>
#include <QOperatingSystemVersion>
#include <QPainter>
#include <QPaintEvent>


// 添加字体定义
struct IconFont
{
    QString fontFamily;
    QString minimize;
    QString maximize;
    QString restore;
    QString close;
    qreal   fontSize;
};

// 获取当前平台的图标字体配置
IconFont getIconFont()
{
    IconFont font;

#if defined(Q_OS_LINUX)
    auto font_index = QFontDatabase::addApplicationFont(":/resources/fonts/MaterialIcons-Regular.ttf");
    font.fontFamily = QFontDatabase::applicationFontFamilies(font_index).at(0);

    font.minimize = QChar(0xE15B); // remove
    font.maximize = QChar(0xE3C6); // fullscreen
    font.restore  = QChar(0xE8F1); // fullscreen_exit
    font.close    = QChar(0xE5CD); // close
    font.fontSize = 14;

#elif defined(Q_OS_WIN)
    font.fontFamily = "Segoe MDL2 Assets";
    font.minimize   = QChar(0xE949);
    font.maximize   = QChar(0xE739);
    font.restore    = QChar(0xE923);
    font.close      = QChar(0xE8BB);
    font.fontSize   = 7.5;

#elif defined(Q_OS_MAC)
    font.fontFamily = ".AppleSystemUIFont";
    font.minimize   = "−";
    font.maximize   = "+";
    font.restore    = "−";
    font.close      = "×";
    font.fontSize   = 14;
#endif

    return font;
}

TitleBarButton::TitleBarButton(ButtonType type, QWidget *parent) : QPushButton(parent)
{
    _type            = type;
    _backgroundColor = Qt::transparent;
    _textColor       = QColor(140, 140, 140);
    _isCloseButton   = (type == ButtonType::CLOSE);

    setFixedWidth(45);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    _backgroundAnimation = new QVariantAnimation(this);
    _backgroundAnimation->setDuration(100);

    _textAnimation = new QVariantAnimation(this);
    _textAnimation->setDuration(100);

    connect(_backgroundAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        _backgroundColor = value.value<QColor>();
        update();
    });

    connect(_textAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        _textColor = value.value<QColor>();
        update();
    });

#ifdef Q_OS_LINUX
    _hoverColor = _isCloseButton ? QColor(0xe94242) : QColor(0xe4e4e4);
    _pressColor = _isCloseButton ? QColor(0xbf2d3a) : QColor(0xd4d4d4);
#else
    _hoverColor = _isCloseButton ? QColor(0xc42b1c) : QColor(0xf2f2f2);
    _pressColor = _isCloseButton ? QColor(0xc42b1c) : QColor(0xe6e6e6);
#endif
}

void TitleBarButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    if (_backgroundColor != Qt::transparent) {
        painter.fillRect(rect(), _backgroundColor);
    }

    static const IconFont iconFont = getIconFont();

    QFont font(iconFont.fontFamily);
    font.setPointSizeF(iconFont.fontSize);
    painter.setFont(font);

    painter.setPen(_textColor);

    QString icon;
    switch (_type) {
        case ButtonType::MINIMIZE: icon = iconFont.minimize; break;
        case ButtonType::MAXIMIZE: icon = iconFont.maximize; break;
        case ButtonType::RESTORE: icon = iconFont.restore; break;
        case ButtonType::CLOSE: icon = iconFont.close; break;
        default: break;
    }

    // 使用 tightBoundingRect 获得更精确的文本边界
    QFontMetrics fm(font);
    // QRect        textRect = fm.tightBoundingRect(icon);

    // 计算绘制位置
    int x = (width() - fm.horizontalAdvance(icon)) / 2;
    int y = (height() + fm.ascent() - fm.descent()) / 2;

    // 绘制文本
    painter.drawText(x, y, icon);
}

TitleBarButton::~TitleBarButton()
{
    delete _backgroundAnimation;
    delete _textAnimation;
}

void TitleBarButton::SetBackgroundColor(const QColor &color)
{
    if (_backgroundColor != color) {
        _backgroundColor = color;
        update();
    }
}

void TitleBarButton::StartAnimation(const QColor &endBackgroundColor, const QColor &endTextColor)
{
    if (_backgroundAnimation->state() == QVariantAnimation::Running) {
        _backgroundAnimation->stop();
    }
    if (_textAnimation->state() == QVariantAnimation::Running) {
        _textAnimation->stop();
    }

    _backgroundAnimation->setStartValue(_backgroundColor);
    _backgroundAnimation->setEndValue(endBackgroundColor);
    _backgroundAnimation->start();

    _textAnimation->setStartValue(_textColor);
    _textAnimation->setEndValue(endTextColor);
    _textAnimation->start();
}

void TitleBarButton::SetHovered(bool hovered)
{
    if (hovered) {
        StartAnimation(QColor(0x3d3d3d), QColor(0xffffff));
    } else {
        StartAnimation(Qt::transparent, QColor(0x8c8c8c));
    }
}

void TitleBarButton::enterEvent(QEnterEvent *event)
{
    QColor hoverBgColor   = _isCloseButton ? QColor(0xe81123) : QColor(0x3d3d3d);
    QColor hoverTextColor = _isCloseButton ? Qt::white : QColor(0xffffff);
    StartAnimation(hoverBgColor, hoverTextColor);
    QPushButton::enterEvent(event);
}

void TitleBarButton::leaveEvent(QEvent *event)
{
    StartAnimation(Qt::transparent, QColor(0x8c8c8c));
    QPushButton::leaveEvent(event);
}

void TitleBarButton::mousePressEvent(QMouseEvent *event)
{
    QColor pressBgColor   = _isCloseButton ? QColor(0xd71123) : QColor(0x545454);
    QColor pressTextColor = _isCloseButton ? Qt::white : QColor(0xffffff);
    StartAnimation(pressBgColor, pressTextColor);
    QPushButton::mousePressEvent(event);
}

void TitleBarButton::mouseReleaseEvent(QMouseEvent *event)
{
    QColor hoverBgColor   = _isCloseButton ? QColor(0xe81123) : QColor(0x545454);
    QColor hoverTextColor = _isCloseButton ? Qt::white : QColor(0xffffff);
    StartAnimation(hoverBgColor, hoverTextColor);
    QPushButton::mouseReleaseEvent(event);
}
