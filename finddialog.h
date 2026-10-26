#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QPushButton;
class QCheckBox;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);

signals:
    void findNextRequested(const QString &text,
                           bool caseSensitive);

private:
    QLineEdit *findLineEdit;
    QPushButton *findButton;
    QPushButton *closeButton;
    QCheckBox *caseCheckBox;
};

#endif // FINDDIALOG_H