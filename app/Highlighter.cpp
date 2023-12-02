#include "Highlighter.h"

#include <QRegularExpression>
#include <QString>

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) { }

void Highlighter::highlightBlock(const QString &text)
{
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(197, 134, 192));
    QStringList keywordPatterns;
    keywordPatterns << "\\bcast\\b"
                    << "\\bfcast\\b"
                    << "\\bscast\\b"
                    << "\\bsfcast\\b";

    for (const QString &pattern : keywordPatterns) {
        QRegularExpression      expression(pattern);
        QRegularExpressionMatch match = expression.match(text);
        if (match.hasMatch()) {
            setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
        }
    }

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(78, 201, 176));
    QStringList keywordPatterns2;
    keywordPatterns2 << "\\bskill_cd\\b"
                     << "\\bskill_energy\\b"
                     << "\\blast_skill\\b"
                     << "\\btbufftime\\b"
                     << "\\bbufftime\\b"
                     << "\\bqidian\\b"
                     << "\\bnobuff\\b"
                     << "\\bbuff\\b"
                     << "\\bskill_notin_cd\\b";

    for (const QString &pattern : keywordPatterns2) {
        QRegularExpression      expression(pattern);
        QRegularExpressionMatch match = expression.match(text);
        if (match.hasMatch()) {
            setFormat(match.capturedStart(), match.capturedLength(), singleLineCommentFormat);
        }
    }

    QTextCharFormat singleLineCommentFormat2;
    singleLineCommentFormat2.setForeground(QColor(140, 220, 254));
    QStringList keywordPatterns3;
    keywordPatterns3 << "三环套月"
                     << "万剑归宗"
                     << "无我无剑"
                     << "八荒归元"
                     << "三柴剑法"
                     << "人剑合一"
                     << "碎星辰"
                     << "吞日月"
                     << "紫气东来"
                     << "玄门"
                     << "叠刃";

    for (const QString &pattern : keywordPatterns3) {
        QRegularExpression      expression(pattern);
        QRegularExpressionMatch match = expression.match(text);
        if (match.hasMatch()) {
            setFormat(match.capturedStart(), match.capturedLength(), singleLineCommentFormat2);
        }
    }

    QTextCharFormat singleLineCommentFormat3;
    singleLineCommentFormat3.setForeground(QColor(220, 220, 170));
    QStringList keywordPatterns4;
    keywordPatterns4 << "宏·紫气宏";

    for (const QString &pattern : keywordPatterns4) {
        QRegularExpression      expression(pattern);
        QRegularExpressionMatch match = expression.match(text);
        if (match.hasMatch()) {
            setFormat(match.capturedStart(), match.capturedLength(), singleLineCommentFormat3);
        }
    }

    QTextCharFormat singleLineCommentFormat4;
    singleLineCommentFormat4.setForeground(QColor(106, 153, 85)); // 单行注释

    QRegularExpression      expression("//[^\n]*");
    QRegularExpressionMatch match = expression.match(text);
    if (match.hasMatch()) {
        setFormat(match.capturedStart(), match.capturedLength(), singleLineCommentFormat4);
    }

    // 多行注释
    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setForeground(QColor(106, 153, 85));

    QRegularExpression      startExpression("/\\*");
    QRegularExpressionMatch startMatch = startExpression.match(text);
    QRegularExpression      endExpression("\\*/");
    QRegularExpressionMatch endMatch = endExpression.match(text);

    // if (!startMatch.hasMatch() && !endMatch.hasMatch()) {
    //     setCurrentBlockState(0);
    //     return;
    // }

    // int startIndex = 0;
    // if (previousBlockState() != 1) {
    //     startIndex = startMatch.capturedStart();
    // }

    // while (startIndex >= 0) {
    //     QRegularExpressionMatch endMatch = endExpression.match(text, startIndex);
    //     int                     endIndex = endMatch.capturedStart();
    //     int                     commentLength;
    //     if (endIndex == -1) {
    //         setCurrentBlockState(1);
    //         commentLength = text.length() - startIndex;
    //     } else {
    //         commentLength = endIndex - startIndex + endMatch.capturedLength();
    //     }
    //     setFormat(startIndex, commentLength, multiLineCommentFormat);
    //     startIndex = text.indexOf(startExpression, startIndex + commentLength);
    //     if (startIndex >= 0) {
    //         endMatch = endExpression.match(text, startIndex);
    //     }
    // }

    setCurrentBlockState(0);
    
    /******************************************************************************************************
     * 查找起始表达式的位置
     *
     * 如果先前的块状态为“处于注释中”（previousBlockState()== 1），则在文本块的开头开始搜索结束表达式。
     * 如果previousBlockState()返回0，则查找首次出现起始表达式的位置*/
    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(startExpression);
    }

    // 接着查找结束表达式的位置，因为跨行，所以需要循环查找。
    while (startIndex >= 0) {
        QRegularExpressionMatch match         = endExpression.match(text, startIndex);
        int                     endIndex      = match.capturedStart();
        int                     commentLength = 0;
        // 如果在当前文本块中找不到结束表达式，则将当前块状态设置为1，并计算当前注释文本的长度
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        // 如果在当前文本块中找到结束表达式，则计算注释文本的实际长度
        else
        {

            commentLength = endIndex - startIndex + match.capturedLength();
        }
        // 应用多行注释格式。
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        // 然后，我们搜索下一个出现的开始表达式并重复该过程。
        startIndex = text.indexOf(startExpression, startIndex + commentLength);
        /* 这样就完成了Highlighter类的实现。*/
    }
}