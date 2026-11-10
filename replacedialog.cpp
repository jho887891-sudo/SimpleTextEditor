#include "replacedialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("替换"));
    setFixedSize(420, 190);

    // 查找内容输入框
    QLabel *findLabel = new QLabel(
        QStringLiteral("查找内容："), this);

    findLineEdit = new QLineEdit(this);
    findLineEdit->setObjectName("findLineEdit");
    findLineEdit->setPlaceholderText(
        QStringLiteral("请输入要查找的内容"));

    // 替换内容输入框
    QLabel *replaceLabel = new QLabel(
        QStringLiteral("替换为："), this);

    replaceLineEdit = new QLineEdit(this);
    replaceLineEdit->setObjectName("replaceLineEdit");
    replaceLineEdit->setPlaceholderText(
        QStringLiteral("请输入替换后的内容"));

    // 区分大小写选项
    caseCheckBox = new QCheckBox(
        QStringLiteral("区分大小写"), this);

    // 操作按钮
    replaceButton = new QPushButton(
        QStringLiteral("替换"), this);

    replaceAllButton = new QPushButton(
        QStringLiteral("全部替换"), this);

    closeButton = new QPushButton(
        QStringLiteral("关闭"), this);

    // 第一行：查找内容
    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->addWidget(findLabel);
    findLayout->addWidget(findLineEdit);

    // 第二行：替换内容
    QHBoxLayout *replaceLayout = new QHBoxLayout;
    replaceLayout->addWidget(replaceLabel);
    replaceLayout->addWidget(replaceLineEdit);

    // 第三行：选项与按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(caseCheckBox);
    buttonLayout->addStretch();
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(replaceAllButton);
    buttonLayout->addWidget(closeButton);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(findLayout);
    mainLayout->addLayout(replaceLayout);
    mainLayout->addLayout(buttonLayout);

    // 关闭对话框
    connect(closeButton, &QPushButton::clicked,
            this, &QDialog::reject);

    // 单次替换请求
    connect(replaceButton, &QPushButton::clicked,
            this, [this]() {
                emit replaceRequested(
                    findLineEdit->text(),
                    replaceLineEdit->text(),
                    caseCheckBox->isChecked()
                    );
            });

    // 全部替换请求
    connect(replaceAllButton, &QPushButton::clicked,
            this, [this]() {
                emit replaceAllRequested(
                    findLineEdit->text(),
                    replaceLineEdit->text(),
                    caseCheckBox->isChecked()
                    );
            });
}