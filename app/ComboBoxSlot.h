#ifndef COMBOBOXSLOT_H
#define COMBOBOXSLOT_H


#include <QLabel>
#include <QComboBox>
#include <QListWidgetItem>
#include <QBoxLayout>
#include <QListWidgetItem>
#include <QBasicTimer>

class ScrollLabel : public QLabel
{
    Q_OBJECT
public:
    enum ScrollDirection
    {
        RIGHT_TO_LEFT = 1,
        LEFT_TO_RIGHT = 2
    };

    explicit ScrollLabel(QWidget *parent = nullptr);

    // 滚动方向
    ScrollLabel::ScrollDirection GetDirection() const;
    void SetDirection(ScrollLabel::ScrollDirection direction);

    // 刷新间隔
    int GetInterval() const;
    void SetInterval(int interval);

protected:
    // basictimer定时器触发
    virtual void timerEvent(QTimerEvent *event) override;

    // 绘制
    virtual void paintEvent(QPaintEvent *event) override;

    // 大小变化时重新计算
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    // 滚动前后停滞时间
    int m_startTimes = 0;
    int m_endTimes = 0;

    // 刷新间隔
    int m_interval = 20;

    // 偏移量
    int m_offset = 0;
    int m_textWidth = 0;
    int m_labelWidth = 0;

    // 默认右往左
    ScrollDirection m_direction = RIGHT_TO_LEFT;

    // 滚动定时器 也可以使用QTimer QTimeLine QAnimation等实现
    QBasicTimer m_scrollTimer;

public slots:
    void Scroll();
    void Reset();

};

class IconLabelSlot : public QLabel
{
public:
    IconLabelSlot(QWidget *parent = nullptr);
    IconLabelSlot(const QString &imgPath, bool isColor, QWidget *parent = nullptr);

    bool IsNull();
    void SetImgPath(const QString &imgPath);
    void SetColor(bool isColor);
    void SetOpacity(const double opacity);
    void SetSize(const int size);
    void SetDefault(bool b);

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QString m_imgPath = nullptr;
    bool m_isDefault = false;
    bool m_isColor = false;
    double m_opacity = 0.3;
    int m_size = 30;

};

class ComboBoxSlotItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ComboBoxSlotItemWidget(const int index, QListWidgetItem *item = nullptr, QWidget *parent = nullptr);

    //void SetAttrType(const JX3DPS::AttrType type);

    QListWidgetItem &GetItem();

    QString GetIconPath();
    QString GetSubText();

    // 预选择
    void ItemPrimarySelected();

    // 预选择取消
    void ItemPrimarySelectionCancelled();

    // 选择
    void ItemSelected();

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    int m_index = 0;
    QString m_imgPath = nullptr;
    QString m_text = nullptr;

    IconLabelSlot *m_icon = nullptr;
    ScrollLabel *m_textMain = nullptr;
    ScrollLabel *m_textSub = nullptr;
    QListWidgetItem *m_item = nullptr;

    QVBoxLayout *m_vLayout = nullptr;
    QHBoxLayout *m_hLayout = nullptr;

signals:
    void CurrentIndexChanged(int index);
};

class ComboBoxSlotView : public QListWidget
{
    Q_OBJECT
public:
    explicit ComboBoxSlotView(QWidget *parent = nullptr);

    virtual QRect visualRect(const QModelIndex &index) const override;

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
    void PrimarySelectionChanged(QListWidgetItem *current, QListWidgetItem *previous);
};

class ComboBoxSlot : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboBoxSlot(QWidget *parent = nullptr);

    void AddItems();
    void SetCurrentIndex(int index);

protected:
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void showPopup() override;
    virtual void hidePopup() override;

private:
    int m_lastIndex = -1;
    int m_slotIndex = 0;

    IconLabelSlot *m_icon = nullptr;
    ScrollLabel *m_textMain = nullptr;
    ScrollLabel *m_textSub = nullptr;

    ComboBoxSlotView *m_itemList = nullptr;

    QVBoxLayout *m_vLayout = nullptr;
    QHBoxLayout *m_hLayout = nullptr;

    QVector<ComboBoxSlotItemWidget *> m_itemWidgets;
    QVector<QListWidgetItem *> m_widgetItems;
    bool m_isActivated = false;

public slots:
    void CurrentIndexChanged(int index);
};

#endif // COMBOBOXSLOT_H
