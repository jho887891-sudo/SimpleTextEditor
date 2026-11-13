#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class QPaintEvent;
class QResizeEvent;
class QEvent;
class LineNumberArea;

// 第12项：继承 QPlainTextEdit，添加行号区域。
// mainwindow.ui 中已将 textEdit 从 QPlainTextEdit 提升为 CodeEditor。
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    int lineNumberAreaWidth() const;
    void paintLineNumberArea(QPaintEvent *event);
    void setLineNumbersVisible(bool visible);
    bool lineNumbersVisible() const { return showLineNumbers_; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

    LineNumberArea *lineNumberArea_ = nullptr;
    bool showLineNumbers_ = true;
};

#endif // CODEEDITOR_H
