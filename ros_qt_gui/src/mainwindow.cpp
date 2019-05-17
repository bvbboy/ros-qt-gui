#include "../include/mainwindow.h"
//#include "ui_mainwindow.h"
#include "stdio.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ros_qt_template");

    viz=new MyViz();
    ui->VizArea->addSubWindow(viz, Qt::FramelessWindowHint); // FramelessWindowHint removes close, minimize and maximize title bar
    viz->showMaximized();

    RobotModelFlag=false;

    ur_terminal=new QProcess(this);
    moveit_terminal=new QProcess(this);

    this->setMouseTracking(true);
    ui->dockWidget_left->setMouseTracking(true);

    node_terminal=new QProcess(this);
    ui->showButton->setVisible(false);
    UpdateTimer=new QTimer();
    connect(UpdateTimer,SIGNAL(timeout()),this,SLOT(update()));
    UpdateTimer->start(50);

    connect(ui->StartButton,SIGNAL(clicked()),this,SLOT(StartPtn_clicked()));
    connect(ui->StopButton,SIGNAL(clicked()),this,SLOT(StopPtn_clicked()));
    connect(ui->QuitButton,SIGNAL(clicked()),this,SLOT(QuitPtn_clicked()));
    connect(ui->MoveitButton,SIGNAL(clicked()),this,SLOT(MoveitBtn_clicked()));
    connect(node_terminal,SIGNAL(readyRead()),this,SLOT(ReadOutput()));
    connect(ui->textBrowser,SIGNAL(textChanged()),this,SLOT(textBrower_textChanged()));
    connect(ui->ModelButton,SIGNAL(clicked()),this,SLOT(ModelBtn_clicked()));
    connect(ui->NodeButton,SIGNAL(clicked()),this,SLOT(NodeBtn_clicked()));
   // connect(ros_terminal,SIGNAL(started()),viz,SLOT(addRobotModel()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete viz;

    if(ur_terminal->state()!=0)   ur_terminal->terminate();
    delete ur_terminal;

    if(moveit_terminal->state()!=0)   moveit_terminal->terminate();
    delete moveit_terminal;

    if(node_terminal->state()!=0)  node_terminal->terminate();
    delete node_terminal;

    delete UpdateTimer;
}



void MainWindow::StartPtn_clicked()
{
    if (ur_terminal->state()!=0)  ui->VizArea->update();
    else {
        QString run_cmd = QString("roslaunch ur_modern_driver dual_ur10_bringup.launch");
        int n = run_cmd.length()+1;
        char cmd[n];
        std::string cmd_string = run_cmd.toStdString();
        strncpy(cmd,cmd_string.c_str(),n-1);
        cmd[n-1] =0;
        qDebug()<<cmd;
        ur_terminal->start(cmd);
        if(ur_terminal->waitForStarted(-1))
        {
            qDebug()<<"started";
            qDebug()<<ur_terminal->state();
            RobotModelFlag=true;
            initTime=QTime::currentTime();
            qDebug()<<ur_terminal->children();
        }
        qDebug()<<ur_terminal->children();
        ui->textBrowser->setText("connecting to dual ur10...");
    }
}

void MainWindow::StopPtn_clicked()
{
    qDebug()<<node_terminal->state();
    if(node_terminal->state()!=0){
        node_terminal->terminate();
    }
    qDebug()<<node_terminal->state();
   viz->removeRobotModel();
//   ui->textBrowser->setText("stop the node!");
   ui->textBrowser->append("stop the node!");
}

void MainWindow::QuitPtn_clicked()
{
    if(node_terminal->state()!=0)
    {
      node_terminal->terminate();
      node_terminal->kill();
    }
    if(moveit_terminal->state()!=0)   moveit_terminal->terminate();
    if(ur_terminal->state()!=0)   ur_terminal->terminate();
    QProcess node_kill;
    QString run_cmd = QString("rosnode ")
            + QString("kill") + QString(" ")
            + QString("-a");
    int n = run_cmd.length()+1;
    char cmd[n];
    std::string cmd_string = run_cmd.toStdString();
    strncpy(cmd,cmd_string.c_str(),n-1);
    cmd[n-1] =0;
    node_kill.start(run_cmd);
    exit(0);
}

void MainWindow::MoveitBtn_clicked()
{
    QString run_cmd = QString("roslaunch dual_ur10_moveit_config dual_ur10_robot_moveit_planning_execution.launch");
    int n = run_cmd.length() + 1;
    char cmd[n];
    std::string cmd_string = run_cmd.toStdString();
    strncpy(cmd, cmd_string.c_str(), n-1);
    cmd[n-1] = 0;
    qDebug()<<cmd;
    moveit_terminal->start(cmd);
    if (moveit_terminal->waitForStarted(-1)) {
        qDebug()<<"start moveit";
        qDebug()<<moveit_terminal->state();
    }
//    ui->textBrowser->setText("using moveit");
    ui->textBrowser->append("using moveit...");
    QString str = moveit_terminal->readAll();
    qDebug()<<str;
    ui->textBrowser->append(str);
}

void MainWindow::ModelBtn_clicked()
{
    viz->addRobotModel();
}

void MainWindow::NodeBtn_clicked()
{
    node_terminal->setWorkingDirectory("/home/a/catkin_ws");
    QString run_cmd = QString("rosrun test_pkg test_83");
    int n = run_cmd.length()+1;
    char cmd[n];
    std::string cmd_string = run_cmd.toStdString();
    strncpy(cmd,cmd_string.c_str(),n-1);
    cmd[n-1] =0;
    qDebug()<<cmd;
    node_terminal->start(cmd);
    qDebug()<<node_terminal->state();
    node_terminal->waitForStarted(-1);
    qDebug()<<node_terminal->state();
    ui->textBrowser->append("running node...");
}

void MainWindow::update()
{
//    ui->textBrowser->moveCursor(QTextCursor::End);

    int t=initTime.msecsTo(QTime::currentTime());
    if(t>=1500 && RobotModelFlag)
    {
      viz->addRobotModel();
      RobotModelFlag=false;
    }
}

void MainWindow::ReadOutput()
{
    QString str = QString(node_terminal->readAll());
    ui->textBrowser->setText(str);
    qDebug()<<node_terminal->readAll();
}


void MainWindow::textBrower_textChanged()
{
//    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
 // if(event->x()>400)  ui->showButton->setVisible(true);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
 // qDebug()<<ui->dockWidget_left->geometry();
  if(ui->dockWidget_right->isVisible())
  {
    event->ignore();
    ui->showButton->setVisible(false);
  }
  else {
    QRect rect=ui->dockWidget_left->geometry();
    if(event->x()<rect.right() &&event->x()>(rect.right()-25))  ui->showButton->setVisible(true);
    else ui->showButton->setVisible(false);
  }
}

