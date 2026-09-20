#ifndef DESKTOP_WORKBENCH_H
#define DESKTOP_WORKBENCH_H

#include "controls/frameless/frameless.h"
#include <memory>

class Workbench : public Frameless {
public:
    explicit Workbench(QWidget *parent = nullptr);
    ~Workbench() override;
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif
