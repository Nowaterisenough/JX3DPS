#ifndef __HIGHLIGHTER_H__
#define __HIGHLIGHTER_H__

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

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