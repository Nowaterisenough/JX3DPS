#ifndef CUSTOMGROUPBOX_H
#define CUSTOMGROUPBOX_H


#include <QGroupBox>

class CustomGroupBox : public QGroupBox
{
public:
    CustomGroupBox(QWidget *parent = nullptr);

protected:
    virtual void paintEvent(QPaintEvent *) override;
};

#endif // CUSTOMGROUPBOX_H
