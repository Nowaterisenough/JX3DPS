/**
 * Project: JX3DPS
 * File: Network.cpp
 * Description:
 * Created Date: 2023-06-29 17:44:48
 * Author: 难为水
 * -----
 * Last Modified: 2023-06-29 17:45:08
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#include "Network.h"

QPixmap RequestIcon(const QUrl &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest       request(url);

    QNetworkReply *reply = manager.get(request);
    QEventLoop     eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    QByteArray responseData = reply->readAll();
    reply->deleteLater();

    QPixmap pixmap;
    pixmap.loadFromData(responseData);

    return pixmap;
}