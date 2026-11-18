#pragma once

#include <memory>
#include <QJsonArray>
#include <QWidget>
#include "runtime_catalog.h"

class QCheckBox;

namespace desktop {
class TalentPanel final : public QWidget {
public:
    explicit TalentPanel(Specialization specialization, QWidget *parent = nullptr);
    ~TalentPanel() override;
    QCheckBox *AddTalent(const QString &name, const QString &key);
    static QString DataVersion();
    QJsonArray Selection() const;
    bool ValidateSelection(const QJsonArray &selection, QString &error) const;
    void RestoreSelection(const QJsonArray &selection);
    bool HasSelection() const;
    void ClearSelection();
    void SetColumns(int columns);
    QWidget *HeaderWidget() const;
    QWidget *TakeHeader();

protected:
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
} // namespace desktop
