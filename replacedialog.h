#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class QCheckBox;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

signals:
    void replaceRequested(const QString &findText,
                          const QString &replaceText,
                          bool caseSensitive);

    void replaceAllRequested(const QString &findText,
                             const QString &replaceText,
                             bool caseSensitive);

private:
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;

    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;

    QCheckBox *caseCheckBox;
};

#endif // REPLACEDIALOG_H