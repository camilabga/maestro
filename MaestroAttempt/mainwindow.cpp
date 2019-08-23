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


// Main Window
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    correctionEffect(this),
    aproxEffect(this),
    metronomoTick(this)
{

    ui->setupUi(this);
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(DisplayImage()));
    ui->stopBt->hide();
    ui->startBt->hide();
    newGesture = false;

    // Correção
    correctionEffect.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/SimpleAudio/audios/300hz_short.wav"));
    correctionEffect.setVolume(0.25f);
    correction = false;
    correctionValue.x = 0;
    correctionValue.y = 0;

    // Aproximação
    longe = perto = false;
    aproxEffect.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/SimpleAudio/audios/metronome_click.wav"));
    aproxEffect.setVolume(0.25f);

    // Initialize Audio
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    if (!QAudioDeviceInfo::defaultOutputDevice().isFormatSupported(format)) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = QAudioDeviceInfo::defaultOutputDevice().nearestFormat(format);
    }

    const int durationSeconds = 1;
    toneSampleRateHz = 0;
    m_generator.reset(new Generator(format, durationSeconds * 1000000, toneSampleRateHz));
    m_audioOutput.reset(new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format));
    m_generator->start();
    m_audioOutput->setVolume(0.15f);

    // Metronomo
    metronomoValue = 60000/40;
    metronomoTick.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/SimpleAudio/audios/metronome_click.wav"));
    metronomoTick.setVolume(0.25f);
    metronomoTimer = new QTimer(this);
    connect(metronomoTimer, SIGNAL(timeout()), this, SLOT(MetronomoSlot()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DisplayImage(){

    Point newValue;

    ui->image->show();

    vision.calculateTagCenter();
    vision.circleTracker();

    if(newGesture){
        if (vision.isTargetOn()) {
            trajectory.savePoint(vision.getCenter());
            vision.saveVideo();
        }
        vision.saveVideo();

    }else if(correction){
        unsigned int beforePonit, afterPoint;

        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());

        beforePonit = trajectory.getCurrentPointId();

        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());

            // Feedback de Tap (Click quando o ponto é trocado)
            afterPoint = trajectory.getCurrentPointId();
            if(afterPoint != beforePonit && afterPoint == 0) aproxEffect.play();

            newValue = vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());
            //weareable.send(trajectory.getError(vision.getCenter()));
            trajectory.savePoint(vision.getCenter());
        }
    }

    // Plays audible feedback
    correctionValue = audioFeedbackHandler(correctionValue, newValue);

    Mat img = vision.getInputImage();
    cv::cvtColor(img,img,CV_BGR2RGB);
    QImage imdisplay((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    ui->image->setPixmap(QPixmap::fromImage(imdisplay));
}

void MainWindow::MetronomoSlot()
{
    metronomoTick.play();
}

void MainWindow::on_spinBox_valueChanged(int value)
{
    metronomoValue = 60000/value;
    qDebug() << metronomoValue;
}

void MainWindow::on_startMetronomeButton_clicked()
{
    metronomoTick.stop();
    metronomoTimer->start(metronomoValue);
}

void MainWindow::on_stopMetronomeButton_clicked()
{
    metronomoTick.stop();
    metronomoTimer->stop();
    m_audioOutput->stop();
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
    // Inicia o streaming de audio
    m_audioOutput->start(m_generator.data());
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
        std::string command = "python3 ../structure/camera.py ";
        command += fileName.toStdString();
        system(command.c_str());

    }

}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100),
                                               QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);

    m_audioOutput->setVolume(linearVolume);
    metronomoTick.setVolume(linearVolume);
}

Point MainWindow::audioFeedbackHandler(Point correctionValue, Point newValue)
{

    double aproxValue = (newValue.x)^2 + (newValue.y)^2;
    double compValue = (correctionValue.x)^2 + (correctionValue.y)^2;

    // Audible Feedback for correction
    if(correctionValue != newValue){
        if(newValue.y < 0){
            if ( toneSampleRateHz != 440){
                const int durationSeconds = 1;
                toneSampleRateHz = 440;
                m_generator->stop();
                m_generator->generateData(format, durationSeconds * 1000000, toneSampleRateHz);
                m_generator->start();
            }
        }
        else if(newValue.y > 0){
            if ( toneSampleRateHz != 880){
                const int durationSeconds = 1;
                toneSampleRateHz = 880;
                m_generator->stop();
                m_generator->generateData(format, durationSeconds * 1000000, toneSampleRateHz);
                m_generator->start();
            }
        }
        correctionValue = newValue;
    }

    return correctionValue;
}
