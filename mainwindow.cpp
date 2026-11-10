#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QAction>
#include <QCloseEvent>
#include <QColor>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QMessageBox>
#include <QPalette>
#include <QPlainTextEdit>
#include <QSaveFile>
#include <QStatusBar>
#include <QTextCursor>
#include <QTextDocument>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 三个独立对话框。对话框类和对应 .h/.cpp 文件保持原样。
    connect(ui->actionAbout, &QAction::triggered, this, [this]() {
        AboutDialog dialog(this);
        dialog.exec();
    });
    connect(ui->actionFind, &QAction::triggered, this, [this]() {
        FindDialog dialog(this);
        connect(&dialog, &FindDialog::findNextRequested,
                this, &MainWindow::findNext);
        dialog.exec();
    });
    connect(ui->actionReplace, &QAction::triggered, this, [this]() {
        ReplaceDialog dialog(this);
        connect(&dialog, &ReplaceDialog::replaceRequested,
                this, &MainWindow::replaceOne);
        connect(&dialog, &ReplaceDialog::replaceAllRequested,
                this, &MainWindow::replaceAll);
        dialog.exec();
    });

    // 文件操作
    connect(ui->actionNew, &QAction::triggered,
            this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered,
            this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, [this]() {
        saveFile();
    });
    connect(ui->actionSaveAs, &QAction::triggered, this, [this]() {
        saveFileAs();
    });
    connect(ui->actionExit, &QAction::triggered,
            this, &QWidget::close);
    connect(ui->textEdit->document(), &QTextDocument::modificationChanged,
            this, [this](bool) { updateWindowTitle(); });
    updateWindowTitle();

    // 基础编辑操作
    connect(ui->actionUndo, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered,
            ui->textEdit, &QPlainTextEdit::selectAll);

    // 字体和外观
    connect(ui->actionFont, &QAction::triggered,
            this, &MainWindow::chooseFont);
    connect(ui->actionFontColor, &QAction::triggered,
            this, &MainWindow::chooseFontColor);
    connect(ui->actionBackgroundColor, &QAction::triggered,
            this, &MainWindow::chooseBackgroundColor);
    ui->actionWordWrap->setCheckable(true);
    connect(ui->actionWordWrap, &QAction::toggled, this, [this](bool enabled) {
        ui->textEdit->setLineWrapMode(
            enabled ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    });
    // 保证初始状态和 UI 中的 checked 属性一致。
    ui->textEdit->setLineWrapMode(ui->actionWordWrap->isChecked()
        ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);

    // 查看菜单的显示开关
    connect(ui->actionToolBar, &QAction::toggled,
            ui->mainToolBar, &QToolBar::setVisible);
    connect(ui->mainToolBar, &QToolBar::visibilityChanged,
            ui->actionToolBar, &QAction::setChecked);
    connect(ui->actionStatusBar, &QAction::toggled,
            ui->statusBar, &QStatusBar::setVisible);

    // 第 9 项可以继续完善 Action 使能规则；这里先让撤销/重做/剪切/复制随状态更新。
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    connect(ui->textEdit, &QPlainTextEdit::undoAvailable,
            ui->actionUndo, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::redoAvailable,
            ui->actionRedo, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable,
            ui->actionCopy, &QAction::setEnabled);
    connect(ui->textEdit, &QPlainTextEdit::copyAvailable,
            ui->actionCut, &QAction::setEnabled);

    ui->statusBar->showMessage(QStringLiteral("就绪"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile()
{
    if (!maybeSave()) return;
    ui->textEdit->clear();
    currentFilePath.clear();
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
}

void MainWindow::openFile()
{
    if (!maybeSave()) return;

    const QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("打开文件"), QString(),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, QStringLiteral("打开失败"), file.errorString());
        return;
    }
    const QByteArray data = file.readAll();
    if (file.error() != QFile::NoError) {
        QMessageBox::warning(this, QStringLiteral("读取失败"), file.errorString());
        return;
    }
    file.close();

    ui->textEdit->setPlainText(QString::fromUtf8(data));
    currentFilePath = fileName;
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
}

bool MainWindow::saveFile()
{
    return currentFilePath.isEmpty() ? saveFileAs() : writeToFile(currentFilePath);
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("另存为"),
        currentFilePath.isEmpty() ? QStringLiteral("未命名.txt") : currentFilePath,
        QStringLiteral("文本文件 (*.txt);;所有文件 (*)"));
    if (fileName.isEmpty()) return false;
    if (QFileInfo(fileName).suffix().isEmpty()) fileName += QStringLiteral(".txt");
    return writeToFile(fileName);
}

bool MainWindow::writeToFile(const QString &fileName)
{
    QSaveFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), file.errorString());
        return false;
    }
    const QByteArray data = ui->textEdit->toPlainText().toUtf8();
    if (file.write(data) != data.size()) {
        const QString reason = file.errorString();
        file.cancelWriting();
        QMessageBox::warning(this, QStringLiteral("保存失败"), reason);
        return false;
    }
    if (!file.commit()) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), file.errorString());
        return false;
    }
    currentFilePath = fileName;
    ui->textEdit->document()->setModified(false);
    updateWindowTitle();
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->textEdit->document()->isModified()) return true;
    const QMessageBox::StandardButton result = QMessageBox::warning(
        this, QStringLiteral("未保存的修改"),
        QStringLiteral("当前文件已修改，是否保存？"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);
    if (result == QMessageBox::Save) return saveFile();
    return result == QMessageBox::Discard;
}

void MainWindow::updateWindowTitle()
{
    const QString name = currentFilePath.isEmpty()
        ? QStringLiteral("未命名") : QFileInfo(currentFilePath).fileName();
    setWindowTitle(QStringLiteral("%1[*] - 简单文本编辑器").arg(name));
    setWindowModified(ui->textEdit->document()->isModified());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) event->accept();
    else event->ignore();
}

void MainWindow::chooseFont()
{
    bool accepted = false;
    const QFont font = QFontDialog::getFont(
        &accepted, ui->textEdit->font(), this, QStringLiteral("选择字体"));
    if (accepted) ui->textEdit->setFont(font);
}

// 原上传文件声明并连接了此函数，但缺少实现，会导致链接失败。
void MainWindow::chooseFontColor()
{
    QPalette palette = ui->textEdit->palette();
    const QColor color = QColorDialog::getColor(
        palette.color(QPalette::Text), this, QStringLiteral("选择字体颜色"));
    if (!color.isValid()) return;
    palette.setColor(QPalette::Text, color);
    ui->textEdit->setPalette(palette);
}

void MainWindow::chooseBackgroundColor()
{
    QPalette palette = ui->textEdit->palette();
    const QColor color = QColorDialog::getColor(
        palette.color(QPalette::Base), this, QStringLiteral("选择背景颜色"));
    if (!color.isValid()) return;
    palette.setColor(QPalette::Base, color);
    ui->textEdit->setPalette(palette);
}

bool MainWindow::findNext(const QString &text, bool caseSensitive)
{
    if (text.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("查找"),
                                 QStringLiteral("请输入要查找的内容。"));
        return false;
    }

    QTextDocument::FindFlags flags;
    if (caseSensitive) flags |= QTextDocument::FindCaseSensitively;
    if (ui->textEdit->find(text, flags)) return true;

    // 到达文本末尾后，从开头再找一次。
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    ui->textEdit->setTextCursor(cursor);
    if (ui->textEdit->find(text, flags)) return true;

    QMessageBox::information(this, QStringLiteral("查找"),
                             QStringLiteral("未找到：%1").arg(text));
    return false;
}

void MainWindow::replaceOne(const QString &findText,
                            const QString &replaceText, bool caseSensitive)
{
    if (findText.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("替换"),
                                 QStringLiteral("请输入要查找的内容。"));
        return;
    }

    QTextCursor cursor = ui->textEdit->textCursor();
    const Qt::CaseSensitivity cs = caseSensitive ? Qt::CaseSensitive
                                                : Qt::CaseInsensitive;
    // 若当前选区正好是目标，优先替换选区；否则先定位到下一个匹配项。
    if (!cursor.hasSelection() || cursor.selectedText().compare(findText, cs) != 0) {
        if (!findNext(findText, caseSensitive)) return;
        cursor = ui->textEdit->textCursor();
    }
    cursor.insertText(replaceText);
    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::replaceAll(const QString &findText,
                            const QString &replaceText, bool caseSensitive)
{
    if (findText.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("全部替换"),
                                 QStringLiteral("请输入要查找的内容。"));
        return;
    }

    QTextDocument *doc = ui->textEdit->document();
    QTextDocument::FindFlags flags;
    if (caseSensitive) flags |= QTextDocument::FindCaseSensitively;

    QTextCursor cursor(doc);
    cursor.movePosition(QTextCursor::Start);
    cursor.beginEditBlock();  // 所有替换可一次撤销
    int count = 0;
    for (;;) {
        QTextCursor match = doc->find(findText, cursor, flags);
        if (match.isNull()) break;
        match.insertText(replaceText);
        cursor = match;
        ++count;
    }
    cursor.endEditBlock();
    QMessageBox::information(this, QStringLiteral("全部替换"),
                             QStringLiteral("已替换 %1 处。").arg(count));
}
