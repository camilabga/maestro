#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Vision.h"
//#include "Weareable.h"
#include "Trajectory.h"
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QPalette>
#include <QBrush>
#include <QTextStream>

using namespace std;
using namespace cv;
//using namespace aruco;




MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){

    ui->setupUi(this);
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(DisplayImage()));
    ui->stopBt->hide();
    ui->startBt->hide();
    newGesture = false;
    correction = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::DisplayImage(){

    ui->image->show();

    vision.calculateTagCenter();

    if(newGesture){
        if (vision.isTargetOn()) {
            trajectory.savePoint(vision.getCenter());
            vision.saveVideo();
        }
        vision.saveVideo();

    }else if(correction){
        //std::cerr<<"Era um garoto que como eu\n";

        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());
            vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());

            weareable.send(trajectory.getError(vision.getCenter()));

            trajectory.savePoint(vision.getCenter());
        }
    }

    Mat img = vision.getInputImage();
    cv::cvtColor(img,img,CV_BGR2RGB);
    QImage imdisplay((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    ui->image->setPixmap(QPixmap::fromImage(imdisplay));
}




void MainWindow::on_actionNovo_Gesto_triggered(){

    QString fileName = QFileDialog::getSaveFileName(this,"Save as", "filename.csv", "CSV files (.csv);;Zip files (.zip, *.7z)", 0, 0);
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly |QFile::Truncate)){
        QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
    }else{
        trajectory.saveMovement(fileName.toStdString());
        vision.record("Videos/random_test.avi");
        Timer->start(); //Will start the timer
        ui->stopBt->show();
        ui->startBt->show();
    }

}



void MainWindow::on_stopBt_clicked()
{
    Timer->stop(); //Will stop the timer
    trajectory.endSaving();
    ui->image->hide();
    QBrush tb(Qt::transparent);
    ui->stopBt->hide();
    ui->startBt->hide();
}

void MainWindow::on_startBt_clicked()
{
    ui->startBt->hide();
    newGesture = true;
}

void MainWindow::on_actionTreinar_Gestos_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Save as", "gestos/", tr("CSV files (*.csv);;Zip files (*.zip, *.7z)"));
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::warning(this,"Aviso","Não foi possível abrir o arquivo...");
    }else{

        trajectory.getPointsFromCSV2(fileName.toStdString());
        trajectory.unnormalize2();
        //trajectory.getPointsFromnCSV(fileName.toStdString());
        //trajectory.unnormalize();
        trajectory.saveMovement("/data/random_test.csv");
        weareable.setIP((char*)"192.168.43.236");
        weareable.start();
        correction = true;
        std::cerr<<"Trajetoria "<<trajectory.getSize();
        Timer->start();
    }


}


void MainWindow::on_actionNovo_Gesto_PSMove_triggered()
{
    vision.release();

    QString fileName = QFileDialog::getSaveFileName(this,"Save as", "filename.csv", "CSV files (.csv);;Zip files (.zip, *.7z)", 0, 0);
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly |QFile::Truncate)){
        QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
    }else{
        std::string command = "python3 examples/luiz/camera.py ";
        command += fileName.toStdString();
        system(command.c_str());

    }


}
