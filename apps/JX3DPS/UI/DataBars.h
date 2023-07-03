/**
 * Project: JX3DPS
 * File: DataBars.cpp
 * Description:
 * Created Date: 2022-01-26 21:34:09
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-03 01:52:42
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef DATABARS_H
#define DATABARS_H

#include <QLineEdit>
//#include <QLinearGradient>

class DataBars : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(double rate READ GetRate WRITE SetRate)
    Q_PROPERTY(double opacity READ GetOpacity WRITE SetOpacity)

public:
    explicit DataBars(QWidget *parent = nullptr);
    ~DataBars();

    double GetRate() const;
    void   SetRate(double rate);

    double GetOpacity() const;
    void   SetOpacity(double opacity);

    QColor GetColor(const double rate);
    int    GetWidth(const double rate, const int width);
    double GetValue() const;
    void   SetValue(double value);
    void   UpdateValue();

signals:
    void Signal_EnableChanged(bool enable);

protected:
    void paintEvent(QPaintEvent *e) override;
    bool event(QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    static double                s_valueMax;
    static double                s_valueMin;
    static std::list<DataBars *> s_lineEdits;

    double m_value     = 0.0;
    double m_rate      = 0.0;
    double m_opacity   = 0.0;
    bool   m_isPressed = false;

    QColor m_color0 = QColor(192, 0, 0);
    QColor m_color1 = QColor(240, 170, 0);
    QColor m_color2 = QColor(54, 190, 62);

    // QLinearGradient m_gradient;
    

};

#endif // DATABARS_H