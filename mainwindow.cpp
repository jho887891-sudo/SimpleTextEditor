#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "aboutdialog.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}
