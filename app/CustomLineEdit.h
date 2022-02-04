#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H


#include <QObject>
#include <QWidget>
#include <QLineEdit>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(double rate READ GetRate WRITE SetRate)
    Q_PROPERTY(double opacity READ GetOpacity WRITE SetOpacity)
public:
    explicit CustomLineEdit(QWidget *parent = nullptr);

    static void SetValueMax(const double max);
    static void SetValueMin(const double min);

public slots:
    double GetValue() const;
    void   SetValue(double value = -1.0);

protected:
    virtual void paintEvent(QPaintEvent *) override;

    virtual bool event(QEvent *event) override;

signals:
    void ChangeEnable(bool enable);

private:
    double GetRate() const;
    void   SetRate(double rate);

    double GetOpacity() const;
    void   SetOpacity(double opacity);

    QColor GetColor(const double rate);
    int    GetWidth(const double rate, const int width);

    static double s_valueMax;
    static double s_valueMin;

    double m_value = 0.0;
    double m_rate = 0.0;
    double m_opacity = 0.0;
    bool   m_isPressed = false;
    QColor m_color;
};



#endif // CUSTOMLINEEDIT_H
