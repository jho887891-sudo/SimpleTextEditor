#include "codeeditor.h"

#include <QEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QTextBlock>
#include <QWidget>
#include <QColor>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor)
        : QWidget(editor), editor_(editor) {}

    QSize sizeHint() const override
    {
        return QSize(editor_->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        editor_->paintLineNumberArea(event);
    }

private:
    CodeEditor *editor_;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , lineNumberArea_(new LineNumberArea(this))
{
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &CodeEditor::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int CodeEditor::lineNumberAreaWidth() const
{
    if (!showLineNumbers_)
        return 0;

    int digits = 1;
    int maxLine = qMax(1, blockCount());
    while (maxLine >= 10) {
        maxLine /= 10;
        ++digits;
    }
    return 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::setLineNumbersVisible(bool visible)
{
    if (showLineNumbers_ == visible)
        return;

    showLineNumbers_ = visible;
    lineNumberArea_->setVisible(visible);
    updateLineNumberAreaWidth(0);
    lineNumberArea_->update();
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (!showLineNumbers_)
        return;

    if (dy)
        lineNumberArea_->scroll(0, dy);
    else
        lineNumberArea_->update(0, rect.y(),
                                lineNumberArea_->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    const QRect rect = contentsRect();
    lineNumberArea_->setGeometry(rect.left(), rect.top(),
                                 lineNumberAreaWidth(), rect.height());
}

void CodeEditor::changeEvent(QEvent *event)
{
    QPlainTextEdit::changeEvent(event);
    if (event->type() == QEvent::FontChange && lineNumberArea_) {
        lineNumberArea_->setFont(font());
        updateLineNumberAreaWidth(0);
        lineNumberArea_->update();
    }
}

void CodeEditor::paintLineNumberArea(QPaintEvent *event)
{
    if (!showLineNumbers_)
        return;

    QPainter painter(lineNumberArea_);
    painter.fillRect(event->rect(), QColor(240, 240, 240));
    painter.setFont(font());
    painter.setPen(QColor(100, 100, 100));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.drawText(0, top, lineNumberArea_->width() - 5,
                             fontMetrics().height(), Qt::AlignRight,
                             QString::number(blockNumber + 1));
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
