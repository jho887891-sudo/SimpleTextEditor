#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QCloseEvent;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    // 文件操作
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    bool writeToFile(const QString &fileName);
    bool maybeSave();
    void updateWindowTitle();

    // 格式设置
    void chooseFont();
    void chooseFontColor();
    void chooseBackgroundColor();

    // 查找和替换
    bool findNext(const QString &text, bool caseSensitive);
    void replaceOne(const QString &findText,
                    const QString &replaceText, bool caseSensitive);
    void replaceAll(const QString &findText,
                    const QString &replaceText, bool caseSensitive);

    // 第8项：光标位置、行数和文本长度
    void updateEditorStatus();

    // 第9项：剪切、复制、粘贴操作使能
    void updateEditActions();

    Ui::MainWindow *ui;
    QString currentFilePath;
};

#endif // MAINWINDOW_H
