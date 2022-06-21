#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Contoller.h"


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

    // GUI
    QObject::connect(&m_ctrl, &Controller::update_id, this, &MainWindow::update_id);
    QObject::connect(&m_ctrl, &Controller::update_fn, this, &MainWindow::update_fn);
    QObject::connect(&m_ctrl, &Controller::update_username, this, &MainWindow::update_username);

    QObject::connect(ui->btn_start_bot, &QPushButton::clicked, this, &MainWindow::h_btn_start_stop);

    // upd GUI
    m_ctrl.update_bot_info();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::update_id(QString text)
{
    ui->label_first_name_value->setText(text);
}
void MainWindow::update_fn(QString text)
{
    ui->label_id_value->setText(text);
}
void MainWindow::update_username(QString text)
{
    ui->label_username_value->setText(text);
}
void MainWindow::h_btn_start_stop()
{
    if(ui->btn_start_bot->text() == "Start bot")
    {
        ui->btn_start_bot->setText("Stop bot");
    }
    else if(ui->btn_start_bot->text() == "Stop bot")
    {
        ui->btn_start_bot->setText("Start bot");
    }
}

