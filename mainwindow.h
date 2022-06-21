#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Controller.h"
#include <QMainWindow>
#include <QThread>
#include <QThreadPool>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Controller      m_ctrl;
    QThread        *t_ctrl;

public slots:
    void update_id(QString text);
    void update_fn(QString text);
    void update_username(QString text);

private slots:
    void h_btn_start_stop();

};
#endif // MAINWINDOW_H
