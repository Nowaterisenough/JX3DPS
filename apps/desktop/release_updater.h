#pragma once

#include <QNetworkAccessManager>
#include <QUrl>
#include <QVersionNumber>

class QWidget;
class QJsonObject;

namespace desktop {
struct DesktopRelease {
    QString tag, notes;
    QUrl package;
    QByteArray sha256;
    qint64 bytes = 0;
};

class ReleaseUpdater : public QObject {
public:
    explicit ReleaseUpdater(QWidget *window);
    static QString Version();
    static QString BuildId();
    static QVersionNumber NumericVersion(QString tag);
    static DesktopRelease ReadRelease(const QJsonObject &json, const QString &build);
    void StartAutomaticChecks();
    void Check(bool manual);

private:
    void Offer(const DesktopRelease &release);
    QWidget *window_;
    QNetworkAccessManager network_;
    bool busy_ = false, manual_ = false;
    QString offered_;
};
} // namespace desktop
