#include "aboutdialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("关于简单文本编辑器"));
    setFixedSize(380, 240);

    QLabel *titleLabel = new QLabel(
        QStringLiteral("简单文本编辑器"), this);

    QLabel *infoLabel = new QLabel(
        QStringLiteral(
            "版本：1.0\n"
            "课程：Qt 应用程序开发\n"
            "功能：文本编辑、文件操作与文本处理\n"
            "开发环境：Qt Widgets"
            ), this);

    QPushButton *closeButton = new QPushButton(
        QStringLiteral("关闭"), this);

    titleLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked,
            this, &QDialog::accept);
}