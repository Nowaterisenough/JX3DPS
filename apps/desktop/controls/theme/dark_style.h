#ifndef DARK_STYLE_H
#define DARK_STYLE_H

#include <QProxyStyle>

#include "../export.h"

class CONTROLS_API DarkStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit DarkStyle(QStyle *style = nullptr);

    void polish(QPalette &palette) override;
    void polish(QWidget *widget) override;
    void polish(QApplication *app) override;
};

#endif // DARK_STYLE_H
