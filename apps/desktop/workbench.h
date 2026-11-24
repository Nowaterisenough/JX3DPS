#ifndef DESKTOP_WORKBENCH_H
#define DESKTOP_WORKBENCH_H

#include <memory>

#include "controls/frameless/frameless.h"

class Workbench : public Frameless
{
public:
    explicit Workbench(QWidget *parent = nullptr);
    ~Workbench() override;
    void StartUpdateChecks();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif
