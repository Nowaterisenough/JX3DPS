#pragma once

#include <functional>
#include <memory>
#include <QWidget>
#include "flow_macro.h"

class CodeEditor;

namespace desktop {
class FlowTimelineCanvas final : public QWidget {
public:
    explicit FlowTimelineCanvas(bool compact, QWidget *parent = nullptr);
    void SetModel(const CompiledFlow &model, int duration);
    void SetReadOnly(bool readOnly);
    void SelectLine(int line);
    void FitAll();
    void Zoom(double factor, int anchor = -1);
    QPoint NodePoint(int line, int field = -1) const;
    QRect PlotRect() const;
    int FrameAt(int x) const;
    int CursorFrame() const { return cursorFrame_; }
    std::function<void(int)> selected, edited, removed;
    std::function<void(int, int)> moved, added;
    std::function<void()> expand, undo, redo;
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
private:
    struct Marker { int line, field, frame; QPoint point; };
    QVector<Marker> Markers() const;
    int Track(int field) const;
    int FieldAt(int y) const;
    const FlowNode *Node(int line) const;
    int Hit(QPoint point) const;
    int XAt(int frame) const;
    void Pan(int delta);
    void CancelDrag();
    bool compact_, readOnly_ = false, dragging_ = false, panning_ = false, changed_ = false;
    CompiledFlow model_;
    int duration_ = 4800, end_ = 4800, begin_ = 0, span_ = 4800, selection_ = -1, cursorFrame_ = 0;
    int dragLine_ = -1, previewFrame_ = 0, panBegin_ = 0;
    QPoint pressed_;
};

class FlowTimelineEditor final : public QWidget {
public:
    explicit FlowTimelineEditor(CodeEditor *script, QWidget *parent = nullptr);
    ~FlowTimelineEditor() override;
    void SetDuration(int frames);
    void SetReadOnly(bool readOnly);
    void ShowScript();
    void OpenEditor();
    void AddNode(int frame = 0, int field = 2);
    void EditNode(int line);
    bool MoveNode(int line, int frame);
    bool RemoveNode(int line);
    const CompiledFlow &Model() const;
    std::function<void(const QString &, bool)> statusChanged;
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
} // namespace desktop
