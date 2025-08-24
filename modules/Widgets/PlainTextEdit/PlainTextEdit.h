/**
 * Project: JX3DPS
 * File: TabWidget copy.h
 * Description:
 * Created Date: 2023-06-10 08:38:29
 * Author: 难为水
 * -----
 * Last Modified: 2023-09-05 03:39:56
 * Modified By: 难为水
 * -----
 * HISTORY:
 * Date      	By   	Comments
 * ----------	-----	----------------------------------------------------------
 */

#ifndef __PLAIN_TEXT_EDIT_H__
#define __PLAIN_TEXT_EDIT_H__

#if defined _WIN32 || defined __CYGWIN__
#    if defined EXPORT_PLAIN_TEXT_EDIT // CMake add_definitions
#        ifdef __GNUC__
#            define PLAIN_TEXT_EDIT_API __attribute__((dllexport))
#        else
#            define PLAIN_TEXT_EDIT_API __declspec(dllexport)
#        endif // __GNUC__
#    else
#        ifdef __GNUC__
#            define PLAIN_TEXT_EDIT_API __attribute__((dllimport))
#        else
#            ifndef IMPORT_STATIC_PLAIN_TEXT_EDIT_LIB
#                define PLAIN_TEXT_EDIT_API __declspec(dllimport)
#            else
#                define PLAIN_TEXT_EDIT_API
#            endif
#        endif // __GNUC__
#    endif     // EXPORT_PLAIN_TEXT_EDIT
#    define PLAIN_TEXT_EDIT_PRIVATE

#elif defined __GNUC__
#    if __GNUC__ >= 4
#        define PLAIN_TEXT_EDIT_API     __attribute__((visibility("default")))
#        define PLAIN_TEXT_EDIT_PRIVATE __attribute__((visibility("hidden")))
#    else
#        define PLAIN_TEXT_EDIT_API
#        define PLAIN_TEXT_EDIT_PRIVATE
#    endif // __GNUC__ >= 4

#elif defined __clang__
#    define PLAIN_TEXT_EDIT_API     __attribute__((visibility("default")))
#    define PLAIN_TEXT_EDIT_PRIVATE __attribute__((visibility("hidden")))

#else
#    error "Do not know how to export classes for this platform"
#endif // defined(_WIN32) || defined(__CYGWIN__)

#include <QClipboard>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPlainTextEdit>

#include <nlohmann/json.hpp>

class PLAIN_TEXT_EDIT_API PlainTextEdit : public QPlainTextEdit
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
                    // QString         clipboardText = QGuiApplication::clipboard()->text();
                    // QJsonParseError error;
                    // QJsonDocument doc = QJsonDocument::fromJson(clipboardText.toUtf8(), &error);
                    // if (error.error == QJsonParseError::NoError) {
                    //     QByteArray formattedJson = doc.toJson(QJsonDocument::Indented);
                    //     QString    indentedJsonString(formattedJson);
                    //     plainTextEdit->insertPlainText(indentedJsonString);
                    //     return true;
                    // }

                    nlohmann::ordered_json  json;
                    try {
                        json = nlohmann::ordered_json ::parse(QGuiApplication::clipboard()->text().toStdString());
                        plainTextEdit->insertPlainText(QString::fromStdString(json.dump(4)));
                        return true;
                    } catch (const std::exception &e) {
                        return false;
                    }
                }
            }
        }

        return QObject::eventFilter(watched, event);
    }
};

#endif // __PLAIN_TEXT_EDIT_H__
