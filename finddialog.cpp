#include "finddialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("查找"));
    setFixedSize(380, 150);

    QLabel *findLabel = new QLabel(
        QStringLiteral("查找内容："), this);

    findLineEdit = new QLineEdit(this);
    findLineEdit->setObjectName("findLineEdit");
    findLineEdit->setPlaceholderText(
        QStringLiteral("请输入要查找的内容"));

    findButton = new QPushButton(
        QStringLiteral("查找下一个"), this);

    closeButton = new QPushButton(
        QStringLiteral("关闭"), this);

    caseCheckBox = new QCheckBox(
        QStringLiteral("区分大小写"), this);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(findLabel);
    inputLayout->addWidget(findLineEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(caseCheckBox);
    buttonLayout->addStretch();
    buttonLayout->addWidget(findButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked,
            this, &QDialog::reject);

    connect(findButton, &QPushButton::clicked,
            this, [this]() {
                emit findNextRequested(
                    findLineEdit->text(),
                    caseCheckBox->isChecked()
                    );
            });
}