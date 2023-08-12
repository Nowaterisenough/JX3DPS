/**
 * Project: JX3DPS
 * File: DataBars.cpp
 * Description:
 * Created Date: 2022-01-26 21:34:09
 * Author: 难为水
 * -----
 * Last Modified: 2023-08-07 05:50:43
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __DATA_BAR_H__
#define __DATA_BAR_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_DATA_BAR // CMake add_definitions
#        ifdef __GNUC__
#            define DATA_BAR_API __attribute__((dllexport))
#        else
#            define DATA_BAR_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define DATA_BAR_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_DATA_BAR_LIB
#                define DATA_BAR_API __declspec(dllimport)
#            else
#                define DATA_BAR_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_DATA_BAR
#    define DATA_BAR_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define DATA_BAR_API     __attribute__((visibility("default")))
#        define DATA_BAR_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define DATA_BAR_API
#        define DATA_BAR_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define DATA_BAR_API     __attribute__((visibility("default")))
#    define DATA_BAR_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QLineEdit>

class DATA_BAR_API DataBar : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(double rate READ GetRate WRITE SetRate)
    Q_PROPERTY(double opacity READ GetOpacity WRITE SetOpacity)

public:
    explicit DataBar(QWidget *parent = nullptr);
    ~DataBar();

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

    static double               s_valueMax;
    static double               s_valueMin;
    static std::list<DataBar *> s_lineEdits;

    double m_value     = 0.0;
    double m_rate      = 0.0;
    double m_opacity   = 0.0;
    bool   m_isPressed = false;

    QColor m_color0 = QColor(192, 0, 0);
    QColor m_color1 = QColor(240, 170, 0);
    QColor m_color2 = QColor(54, 190, 62);
};

#endif // __DATA_BAR_H__