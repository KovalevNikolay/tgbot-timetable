#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QThreadPool::globalInstance()->setMaxThreadCount(210);

    t_ctrl = new QThread();
    m_ctrl.moveToThread(t_ctrl);
    QObject::connect(this, &MainWindow::destroyed, t_ctrl, &QThread::quit);
    t_ctrl->start(QThread::NormalPriority);
}

MainWindow::~MainWindow()
{
    delete ui;
}

