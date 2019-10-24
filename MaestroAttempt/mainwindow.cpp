#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Vision.h"
#include "Weareable.h"
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

/*******************
    Main Window
********************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    proxEffect(this),
    startEffect(this),
    selectEffect(this),
    metronomoTick(this)
{
    ui->setupUi(this);
    Timer = new QTimer(this);
    connect(Timer, SIGNAL(timeout()), this, SLOT(DisplayImage()));

    // Seting States
    isMetrnonomeOn = false;
    isAudibleFeedbackOn = false;
    isTapOn = true;
    isTreinarOn = false;
    isLivreOn = false;
    gravarGesto = false;
    weareableIsOn = false;
    espIP = (char*)"10.6.4.143";
    myFileName = "/home/angelo/_Angelo/_Projetos/maestro/build-MaestroAttempt-Desktop_Qt_5_13_0_GCC_64bit-Debug/csv/binarioOrdenado.csv";

    // Language Settings, starting as portuguese.
    isPt = true;
    isEn = false;
    this->setPortuguese();

    // Sets inicial interface
    this->gravarMenuShow(false);
    this->salvarMenuShow(false);
    this->feedbackMenuShow(false);
    this->IMBShow(false);
    newGesture = false;

    // Correção
    correction = false;
    correctionValue.x = 0;
    correctionValue.y = 0;

    // Aproximação
    /*-------------------------------------> Mudar caminho para "/YOUR_PATH/maestro/MaestroAttempt/audioFiles/YOUR_EFFECT.wav" <------*/
    proxEffect.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/metronome_click.wav"));
    proxEffect.setVolume(0.25f);
    selectEffect.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/beat.wav"));
    selectEffect.setVolume(0.25f);
    startEffect.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/startup.wav"));
    startEffect.setVolume(0.25f);


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

    toneSampleRateHz = 0;
    m_generator.reset(new Generator(format, durationSeconds * 1000000, toneSampleRateHz));
    m_audioOutput.reset(new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format));
    m_generator->start();
    m_audioOutput->setVolume(0.0f);

    // Metronomo
    metronomoValue = 60000/40;
    /*-------------------------------------> Mudar caminho para "/YOUR_PATH/maestro/MaestroAttempt/audioFiles/metronome_click.wav" <------*/
    metronomoTick.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/metronome_click.wav"));
    metronomoTick.setVolume(0.0f);
    metronomoTimer = new QTimer(this);
    connect(metronomoTimer, SIGNAL(timeout()), this, SLOT(MetronomoSlot()));

    startEffect.play();

    // Sets inicial frame
    Mat img = imread("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/pics/Webp.net-resizeimage.png", CV_LOAD_IMAGE_COLOR);
    cv::cvtColor(img,img,CV_BGR2RGB);
    QImage imdisplay((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    ui->image->setPixmap(QPixmap::fromImage(imdisplay));

}


MainWindow::~MainWindow()
{
    delete ui;
}

/*********************************
       Especific Functions
**********************************/

void MainWindow::DisplayImage(){

    Point newValue;

    ui->image->setEnabled(true);

    vision.circleTracker();

    if(newGesture){
        if (vision.isTargetOn()) {
            trajectory.savePoint(vision.getCenter());
        }
        vision.saveVideo();

    } else if(correction) {
        unsigned int currentPoint, beforePoint;
        vision.saveVideo();
        int pointId = (trajectory.getSize() == trajectory.getCurrentPointId() + 1
                       ? 0 : trajectory.getCurrentPointId() + 1);
        vision.drawTrajectory(trajectory, pointId);
        beforePoint = trajectory.getCurrentPointId();
        if (vision.isTargetOn()) {
            // Descomentar para mudar os pontos
            trajectory.setNextPoint0(vision.getCenter(), 1);

            // Feedback de Tap (Click quando o ponto é trocado)
            currentPoint = trajectory.getCurrentPointId();
            int midPoint = 9;
            if((currentPoint != beforePoint) && isTapOn) {
                if(int(currentPoint) == midPoint || int(currentPoint) == 19)  proxEffect.play();
            }
            newValue = vision.drawError(vision.getCenter(), trajectory.getPoint(pointId));

            if (weareableIsOn) weareable.send(trajectory.getError(vision.getCenter()));
            if(gravarGesto) trajectory.savePoint(vision.getCenter());
        }
    }

    // Plays audible feedback
    correctionValue = audioFeedbackHandler(correctionValue, newValue);

    // Frame Image
    Mat img = vision.getInputImage();
    cv::cvtColor(img,img,CV_BGR2RGB);
    QImage imdisplay((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    ui->image->setPixmap(QPixmap::fromImage(imdisplay));
}


Point MainWindow::audioFeedbackHandler(Point correctionValue, Point newValue)
{

    double aproxValue = sqrt(pow(newValue.x, 2) + pow(newValue.y,2));

    // Audible Feedback for correction
    if(correctionValue != newValue){
        int newHz = distanceToHz(int(aproxValue), 10);
        if (newHz == 0){
            m_audioOutput->setVolume(0);
        }
        else if (newHz != toneSampleRateHz){
            int volume = ui->volumeFeedbackSlider->value();
            qreal linearVolume = QAudio::convertVolume(volume / qreal(100), QAudio::LogarithmicVolumeScale,
                                                       QAudio::LinearVolumeScale);
            m_audioOutput->setVolume(qreal(linearVolume));
            toneSampleRateHz = newHz;
            m_generator->stop();
            m_generator->generateData(format, durationSeconds * 1000000, toneSampleRateHz);
            m_generator->start();
        }
    }

    return newValue;
}


int MainWindow::distanceToHz(int distance, int steps)
{
    int y, a, b, maximo, minimo, stepValues;
    float stepDist;
    // Definições
    y = 200;
    maximo = 1000;
    minimo = 200;
    a = 50;
    b = 300;
    stepValues = (maximo - minimo)/steps;
    stepDist = (b-a)/steps;

    if (distance <= a){
        y = 0;
    }
    else if ((a <= distance) && (distance < b)){
       // Steps Logic
        for (int i = 0; i < steps; i++){
            float aAux = stepDist*i;
            float bAux = stepDist*(steps-(i+1));
            if ((a + aAux <= distance) && (distance < (b - bAux))){
                y = minimo + stepValues*i;
            }
        }
    }
    else if (b <= distance){
        y = maximo;
    }
    return y;
}


void MainWindow::MetronomoSlot()
{
    metronomoTick.play();
}


/*******************************************
        Metronome Action Functions
********************************************/

void MainWindow::on_spinBox_valueChanged(int value)
{
    metronomoValue = 60000/value;
    qDebug() << metronomoValue;
    metronomoTick.stop();
    metronomoTimer->start(metronomoValue);
}


void MainWindow::on_startMetronomeButton_clicked()
{
    if(!isMetrnonomeOn){
        metronomoTick.stop();
        metronomoTimer->start(metronomoValue);
        isMetrnonomeOn = true;
    }
    else {
        metronomoTick.stop();
        metronomoTimer->stop();
        isMetrnonomeOn = false;
    }
}


void MainWindow::on_metronomeVolumeSlider_valueChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
    metronomoTick.setVolume(linearVolume);
    ui->labelMetronomeVolume->setText("Volume : " + QString::number(value));
}


/************************************************
        Audio Feedback Action Functions
*************************************************/

void MainWindow::on_volumeFeedbackSlider_valueChanged(int value)
{
    qreal linearVolume = QAudio::convertVolume(value / qreal(100), QAudio::LogarithmicVolumeScale,
                                               QAudio::LinearVolumeScale);
    m_audioOutput->setVolume(linearVolume);
    ui->labelFeedbackVolume->setText("Volume : " + QString::number(value));
}


void MainWindow::on_playFeedbackButton_clicked()
{
    if(!isAudibleFeedbackOn){
        // Inicia o streaming de audio
        m_audioOutput->start(m_generator.data());
    }
    else {
        m_audioOutput->stop();
    }
    isAudibleFeedbackOn = !isAudibleFeedbackOn;
}


void MainWindow::on_tapButton_clicked()
{
    if(!isTapOn) isTapOn = true;
    else isTapOn = false;
}


void MainWindow::on_pulsieraButton_clicked()
{
    if(weareableIsOn){
        weareable.setIP((char*)"0.0.0.000");
        QMessageBox::warning(this,"Aviso","Pulseira Desativada");
    } else {
        weareable.setIP(espIP);
        QMessageBox::warning(this,"Aviso","Pulseira Ativada");
        weareable.start();
    }
    weareableIsOn = !weareableIsOn;
}


/************************************
        Menu Action Functions
*************************************/

bool MainWindow::salvarCSV()
{
    QMessageBox::StandardButton ret = QMessageBox::information(this,
                                                               "Aviso",
                                                               "Informe o nome do arquivo CSV a ser salvo",
                                                               QMessageBox::Cancel | QMessageBox::Ok);
    if (ret == QMessageBox::Ok){
        fileName = QFileDialog::getSaveFileName(this,"Save as", "filename.csv", "CSV files (.csv);;Zip files (.zip, *.7z)", 0, 0);
        QFile file(fileName);
        if(!file.open(QFile::WriteOnly |QFile::Truncate)){
            QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
            return false;
        }else{
            trajectory.saveMovement(fileName.toStdString());
            return true;
        }
    }
    else {
        return false;
    }
}


bool MainWindow::salvarVideo()
{
    QMessageBox::StandardButton ret = QMessageBox::information(this,
                                                               "Aviso",
                                                               "Informe o nome do arquivo '.avi' a ser salvo",
                                                               QMessageBox::Cancel | QMessageBox::Ok);
    if(ret == QMessageBox::Ok){
        QString fileName1 = QFileDialog::getSaveFileName(this,"Save as", "Videos/filename.avi", "AVI files (.avi);", 0, 0);
        QFile file1(fileName1);
        if(!file1.open(QFile::WriteOnly |QFile::Truncate)){
            QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
            return false;
        }
        else {
            vision.record(fileName1.toStdString());
            return true;
        }
    }
    else return false;
}


void MainWindow::on_praticaLivreButton_clicked()
{   
    if(salvarCSV()){
        if(salvarVideo()){
            isLivreOn = true;
            isTreinarOn = false;
            Timer->start(); //Will start the timer
            this->praticarInterface();
            selectEffect.play();
        }
        else return;
    }
    else return;
}


void MainWindow::on_treinarBox_currentIndexChanged(int index)
{
    trajectory.flush();
    switch (index) {
        case 0:
            myFileName = "/home/angelo/_Angelo/_Projetos/maestro/build-MaestroAttempt-Desktop_Qt_5_13_0_GCC_64bit-Debug/csv/binarioOrdenado.csv";
            break;
        case 1:
            myFileName = "/home/angelo/_Angelo/_Projetos/maestro/build-MaestroAttempt-Desktop_Qt_5_13_0_GCC_64bit-Debug/csv/ternarioOrdenado.csv";
            break;
        case 2:
            myFileName = "/home/angelo/_Angelo/_Projetos/maestro/build-MaestroAttempt-Desktop_Qt_5_13_0_GCC_64bit-Debug/csv/quaternario.csv";
            break;
        default:
            myFileName = "/home/angelo/_Angelo/_Projetos/maestro/build-MaestroAttempt-Desktop_Qt_5_13_0_GCC_64bit-Debug/csv/binarioOrdenado.csv";
            break;
    }

}


void MainWindow::on_treinarButton_clicked()
{
    bool saveCSV = salvarCSV();
    if (saveCSV){
        bool saveVideo = salvarVideo();
        if(saveVideo){
            // Carrega trajetória
            QString fileName = myFileName;
            QFile file(fileName);
            if(!file.open(QFile::ReadOnly)){
                QMessageBox::warning(this,"Aviso","Não foi possível abrir o arquivo...");
                return;
            }
            trajectory.getPointsFromCSV2(fileName.toStdString());
            trajectory.unnormalize2();
            //trajectory.getPointsFromnCSV(fileName.toStdString());
            //trajectory.unnormalize();

            // Pulseira iniciada ao clicar o botão referente a pulseira

            correction = true;

            //std::cerr<<"Trajetoria "<<trajectory.getSize();

            Timer->start();

            this->IMBShow(false);
            this->treinarInterface();
            isTreinarOn = true;
            isLivreOn = false;
            selectEffect.play();
        }
        else return;
    }
    else return;
}


void MainWindow::on_stopBt_clicked()
{
    Timer->stop(); //Will stop the timer
    trajectory.endSaving();

    ui->image->clear();
    ui->image->setEnabled(false);

    QBrush tb(Qt::transparent);
    this->salvarMenuShow(true);
    this->gravarMenuShow(false);
    this->treinarShow(true);
    if(isTreinarOn) isTreinarOn = false;
    if(isLivreOn) isLivreOn = false;
    gravarGesto=false;
    newGesture=false;
}


void MainWindow::on_startBt_clicked()
{
    ui->startBt->setEnabled(false);
    if(!isTreinarOn) newGesture = true;
    gravarGesto=true;
}


void MainWindow::on_vizualizarButton_clicked()
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
    }else{
        std::string command = "python3 plot.py ";
        command += fileName.toStdString();
        system(command.c_str());
    }
}


void MainWindow::on_salvarButton_clicked()
{
//    vision.release();
//    vision.endRecording();
    QMessageBox::warning(this,"Aviso","Video salvo!");
    ui->salvarButton->setEnabled(false);
}


/************************************
        Language Functions
*************************************/

void MainWindow::setPortuguese()
{
    // Buttons
    ui->startMetronomeButton->setText("Começar/Parar");
    ui->playFeedbackButton->setText("Começar/Parar");
    ui->startBt->setText("Começar");
    ui->stopBt->setText("Parar");
    ui->pulsieraButton->setText("Começar/Parar");
    ui->treinarButton->setText("Treinar Compasso");
    ui->praticaLivreButton->setText("Treino Livre");
    ui->vizualizarButton->setText("Gerar Gráfico");
    ui->imbButton->setText("Treinar IMB");
    // Labels
    ui->labelMetronome->setText("Metrônomo");
    ui->labelAudioFeedback->setText("Retorno Auditivo");
    ui->labelPulsiera->setText("Retorno Tátil");
    ui->labelCompasso->setText("Seleção de Padrões de Marcação de Compasso");
    ui->labelIBM->setText("Independência Motora dos Braços");
    // Menu
    ui->actionSair->setText("Sair");
    // Combo Box
    ui->treinarBox->setItemText(0, "Binário");
    ui->treinarBox->setItemText(1, "Ternário");
    ui->treinarBox->setItemText(2, "Quaternário");
    ui->imbBox->setItemText(0, "Quadrado");
    ui->imbBox->setItemText(1, "Círculo");
}


void MainWindow::setEnglish()
{
    // Buttons
    ui->startMetronomeButton->setText("Start/Stop");
    ui->playFeedbackButton->setText("Start/Stop");
    ui->startBt->setText("Start");
    ui->stopBt->setText("Stop");
    ui->pulsieraButton->setText("Start/Stop");
    ui->treinarButton->setText("Practice Compass");
    ui->praticaLivreButton->setText("Free Practice");
    ui->vizualizarButton->setText("Plot Graph");
    ui->imbButton->setText("Practice IMB");
    // Labels
    ui->labelMetronome->setText("Metrnome");
    ui->labelAudioFeedback->setText("Audible Feedback");
    ui->labelPulsiera->setText("Tactile Feedback");
    ui->labelCompasso->setText("Select Compass Markers Patterns");
        ui->labelIBM->setText("Arms' Locomotorial Independence");
    // Menu
    ui->actionSair->setText("Quit");
    // Combo Box
    ui->treinarBox->setItemText(0, "Binary");
    ui->treinarBox->setItemText(1, "Ternary");
    ui->treinarBox->setItemText(2, "Quaternary");
    ui->imbBox->setItemText(0, "Square");
    ui->imbBox->setItemText(1, "Circle");
}


void MainWindow::on_languageButton_clicked()
{
    if(isPt){
        this->setEnglish();
        isEn = true;
        isPt = false;
    }
    else {
        isPt = true;
        isEn = false;
        this->setPortuguese();
    }
}


/************************************
        Hide-Show Functions
*************************************/

void MainWindow::praticarInterface()
{
    // Sets the interface for the Free Practice situation
    this->feedbackMenuShow(false);
    this->salvarMenuShow(false);
    this->gravarMenuShow(true);
    this->treinarShow(false);
}


void MainWindow::treinarInterface()
{
    this->treinarShow(false);
    this->gravarMenuShow(true);
    this->feedbackMenuShow(true);
    this->salvarMenuShow(false);
}


void MainWindow::salvarMenuShow(bool enable)
{

    ui->salvarButton->setEnabled(enable);
    ui->salvarButton->setEnabled(enable);
    ui->vizualizarButton->setEnabled(enable);

}


void MainWindow::feedbackMenuShow(bool enable)
{
    ui->labelPulsiera->setEnabled(enable);
    ui->labelAudioFeedback->setEnabled(enable);
    ui->pulsieraButton->setEnabled(enable);
    ui->playFeedbackButton->setEnabled(enable);
    ui->volumeFeedbackSlider->setEnabled(enable);
    ui->labelFeedbackVolume->setEnabled(enable);
    ui->tapButton->setEnabled(enable);
}


void MainWindow::gravarMenuShow(bool enable)
{
    ui->startBt->setEnabled(enable);
    ui->stopBt->setEnabled(enable);
}


void MainWindow::treinarShow(bool enable)
{    
    ui->labelCompasso->setEnabled(enable);
    ui->treinarBox->setEnabled(enable);
    ui->treinarButton->setEnabled(enable);
    ui->praticaLivreButton->setEnabled(enable);
}


void MainWindow::IMBShow(bool enable)
{
    ui->imbBox->setEnabled(enable);
    ui->imbButton->setEnabled(enable);
    ui->labelIBM->setEnabled(enable);
}
