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
    Controller m_ctrl;
    QThread *t_ctrl;
};
#endif // MAINWINDOW_H
