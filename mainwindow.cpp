#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //MainWindow::setWindowFlags(Qt::WindowStaysOnTopHint); // NOTE Always top on other windows

    QThreadPool::globalInstance()->setMaxThreadCount(210);

    t_ctrl = new QThread(this);
    m_ctrl.moveToThread(t_ctrl);
    QObject::connect(this, &MainWindow::destroyed, t_ctrl, &QThread::quit);
    t_ctrl->start(QThread::NormalPriority);
}

MainWindow::~MainWindow()
{
    delete ui;
}

