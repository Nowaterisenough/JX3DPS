/**
 * @Description : 
 * @Author      : NoWats
 * @Date        : 2022-02-07 01:11:57
 * @Update      : NoWats
 * @LastTime    : 2022-02-12 00:02:14
 * @FilePath    : \JX3DPS\app\JX3DPS Application\Widget.h
 */
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int height READ GetHeight WRITE SetHeight)
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    int  GetHeight() const;
    void SetHeight(int h);
    void ChangeHeight(int index);
};
#endif // WIDGET_H
