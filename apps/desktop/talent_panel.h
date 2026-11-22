#pragma once

#include <memory>

#include <QJsonArray>
#include <QStringList>
#include <QWidget>

#include "runtime_catalog.h"


namespace desktop {
class TalentPanel final : public QWidget
{
    Q_OBJECT

public:
    explicit TalentPanel(Specialization specialization, QWidget *parent = nullptr);
    ~TalentPanel() override;
    QJsonArray         DefaultSelection() const;
    static QStringList Names(Specialization spec, const QJsonArray &selection);

    static QString DataVersion();
    QJsonArray     Selection() const;
    bool           ValidateSelection(const QJsonArray &selection, QString &error) const;
    void           RestoreSelection(const QJsonArray &selection);
    bool           HasSelection() const;
    void           ClearSelection();
    void           SetColumns(int columns);
    QWidget       *HeaderWidget() const;
    QWidget       *TakeHeader();

signals:
    void SelectionChanged();

protected:
    void hideEvent(QHideEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
} // namespace desktop
