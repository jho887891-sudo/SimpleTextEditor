#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"
#include "finddialog.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbout, &QAction::triggered,
            this, [this]() {
                AboutDialog dialog(this);
                dialog.exec();
            });
    connect(ui->actionFind, &QAction::triggered,
            this, [this]() {
                FindDialog dialog(this);
                dialog.exec();
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}
