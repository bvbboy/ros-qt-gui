 #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QProcess>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QMouseEvent>
#include <QDir>
#include <QMessageBox>

#include "gui_myrviz.h"

#include "ui_mainwindow.h"
#include <QTime>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void StopPtn_clicked();
    void StartPtn_clicked();
    void QuitPtn_clicked();
    void MoveitBtn_clicked();
    void update();
    void ReadOutput();
    void textBrower_textChanged();
    void ModelBtn_clicked();
    void NodeBtn_clicked();

private:
    Ui::MainWindow *ui;
    MyViz *viz;

    QTimer *UpdateTimer;
    bool RobotModelFlag;

    QTime initTime,tempTime;

    QProcess *ur_terminal;
    QProcess *moveit_terminal;
    QProcess *node_terminal;


    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
