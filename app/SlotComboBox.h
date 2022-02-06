#ifndef SLOTCOMBOBOX_H
#define SLOTCOMBOBOX_H


#include <QComboBox>
#include <QLabel>

class SlotComboBoxWidget : public QLabel
{
public:
    SlotComboBoxWidget(QWidget *parent = nullptr);

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

    void SetSize(int size);
    void SetType(int type);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

private:
    QPixmap m_pixmap;
    QPixmap m_bgPixmap;
    bool m_isColor = false;
    bool m_isBlank = true;
    int m_size = 30;
    int m_type = 0;
    double m_opacity = 0.5;
};

class SlotComboBox : public QComboBox
{
public:
    SlotComboBox(QWidget *parent = nullptr);

    void SetType(int type);

private:
    SlotComboBoxWidget *m_slotComboBoxWidget = new SlotComboBoxWidget(this);
};

#endif // SLOTCOMBOBOX_H
