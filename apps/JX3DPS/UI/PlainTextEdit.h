/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-07-02 19:06:01
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QClipboard>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPlainTextEdit>

class PlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    PlainTextEdit(QWidget *parent = nullptr);

    // 重写事件过滤器方法
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Paste)) {
                QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>(watched);
                if (plainTextEdit) {
                    QString         clipboardText = QGuiApplication::clipboard()->text();
                    QJsonParseError error;
                    QJsonDocument   doc = QJsonDocument::fromJson(clipboardText.toUtf8(), &error);
                    if (error.error == QJsonParseError::NoError) {
                        QByteArray formattedJson = doc.toJson(QJsonDocument::Indented);
                        QString    indentedJsonString(formattedJson);
                        plainTextEdit->insertPlainText(indentedJsonString);
                        return true;
                    }
                }
            }
        }

        return QObject::eventFilter(watched, event);
    }
};

#endif // PLAINTEXTEDIT_H
