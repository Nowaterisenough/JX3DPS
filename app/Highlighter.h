#ifndef __HIGHLIGHTER_H__
#define __HIGHLIGHTER_H__

#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:

    Highlighter(QTextDocument *parent = 0);

protected:

    void highlightBlock(const QString &text) override;
};

#endif // __HIGHLIGHTER_H__