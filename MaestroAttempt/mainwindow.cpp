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

    // States Seting
    isMetrnonomeOn = false;
    isAudibleFeedbackOn = false;
    isTapOn = true;
    isTreinarOn = false;
    isLivreOn = false;
    gravarGesto = false;
    weareableIsOn = true;
    espIP = (char*)"10.6.4.143";

    /* Language Settings, starting as portuguese. */
    isPt = true;
    isEn = false;
    this->setPortuguese();

    // Sets inicial interface
    this->gravarMenuShow(false);
    this->salvarMenuShow(false);
    this->feedbackMenuShow(false);
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
    m_audioOutput->setVolume(0.15f);

    // Metronomo
    metronomoValue = 60000/40;
    /*-------------------------------------> Mudar caminho para "/YOUR_PATH/maestro/MaestroAttempt/audioFiles/metronome_click.wav" <------*/
    metronomoTick.setSource(QUrl::fromLocalFile("/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/metronome_click.wav"));
    metronomoTick.setVolume(0.25f);
    metronomoTimer = new QTimer(this);
    connect(metronomoTimer, SIGNAL(timeout()), this, SLOT(MetronomoSlot()));

    startEffect.play();

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

    ui->image->show();

 //   vision.calculateTagCenter();
    vision.circleTracker();

    if(newGesture){
        if (vision.isTargetOn()) {
            trajectory.savePoint(vision.getCenter());
            //vision.saveVideo();
        }
        vision.saveVideo();

    }else if(correction){
        unsigned int currentPoint, beforePoint;

        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        beforePoint = trajectory.getCurrentPointId();

        if (vision.isTargetOn()) {
            // Descomentar para mudar os pontos
            //trajectory.setNextPoint0(vision.getCenter());

            // Feedback de Tap (Click quando o ponto é trocado)
            currentPoint = trajectory.getCurrentPointId();
            int midPoint = 9;
            if((currentPoint != beforePoint) && isTapOn) {
                if(int(currentPoint) == midPoint || int(currentPoint) == 19)  proxEffect.play();
            }

            newValue = vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());

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

void MainWindow::salvarCSV()
{
    QMessageBox::information(this,"Aviso","informe o nome do arquivo csv a ser salvo");
    fileName = QFileDialog::getSaveFileName(this,"Save as", "filename.csv", "CSV files (.csv);;Zip files (.zip, *.7z)", 0, 0);
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly |QFile::Truncate)){
        QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
    }else{
        trajectory.saveMovement(fileName.toStdString());
    }
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
    }
    weareableIsOn = !weareableIsOn;
}

/************************************
        Menu Action Functions
*************************************/
void MainWindow::on_praticaLivreButton_clicked()
{   
    salvarCSV();
    QMessageBox::information(this,"Aviso","informe o nome do arquivo de video a ser salvo");
    QString fileName1 = QFileDialog::getSaveFileName(this,"Save as", "Videos/filename.avi", "AVI files (.avi);", 0, 0);
    QFile file1(fileName1);
    if(!file1.open(QFile::WriteOnly |QFile::Truncate)){
        QMessageBox::warning(this,"Aviso","Não foi possível salvar o arquivo...");
    }else{
        vision.record("Videos/"+fileName1.toStdString());
        Timer->start(); //Will start the timer

       this->praticarInterface();
    }
    isLivreOn = true;
    isTreinarOn = false;
    this->IMBShow(false);
    selectEffect.play();
}

void MainWindow::on_treinarButton_clicked()
{
    salvarCSV();
    QMessageBox::information(this,"Aviso","informe o nome do arquivo csv a ser lido");
    QString fileName = QFileDialog::getOpenFileName(this,"Save as", "gestos/", tr("CSV files (*.csv);;Zip files (*.zip, *.7z)"));
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        QMessageBox::warning(this,"Aviso","Não foi possível abrir o arquivo...");
    }else{
        trajectory.getPointsFromCSV2(fileName.toStdString());
        trajectory.unnormalize2();
        //trajectory.getPointsFromnCSV(fileName.toStdString());
        //trajectory.unnormalize();
        weareable.setIP(espIP);
        weareable.start();
        correction = true;
        std::cerr<<"Trajetoria "<<trajectory.getSize();
        Timer->start();

        this->treinarInterface();
        isTreinarOn = true;
        isLivreOn = false;
        selectEffect.play();
        this->IMBShow(false);
    }
}

void MainWindow::on_stopBt_clicked()
{
    Timer->stop(); //Will stop the timer
    trajectory.endSaving();
    ui->image->hide();
    QBrush tb(Qt::transparent);
    this->salvarMenuShow(true);
    this->gravarMenuShow(false);
    this->treinarShow(true);
    if(isTreinarOn) isTreinarOn = false;
    if(isLivreOn) isLivreOn = false;
    gravarGesto=false;
}

void MainWindow::on_startBt_clicked()
{
    ui->startBt->hide();
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
    vision.endRecording();
    QMessageBox::question(this,"Aviso","Video salvo!");
    ui->salvarButton->hide();
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
    // Labels
    ui->labelMetronome->setText("Metrônomo");
    ui->labelAudioFeedback->setText("Retorno Auditivo");
    ui->labelPulsiera->setText("Retorno Tátil");
    ui->labelCompasso->setText("Seleção de Padrões de Marcação de Compasso");
    // Menu
    ui->actionSair->setText("Sair");
    // Combo Box
    ui->treinarBox->setItemText(0, "Binário");
    ui->treinarBox->setItemText(1, "Ternário");
    ui->treinarBox->setItemText(2, "Quaternário");
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
    // Labels
    ui->labelMetronome->setText("Metrnome");
    ui->labelAudioFeedback->setText("Audible Feedback");
    ui->labelPulsiera->setText("Tactile Feedback");
    ui->labelCompasso->setText("Select Compass Markers Patterns");
    // Menu
    ui->actionSair->setText("Quit");
    // Combo Box
    ui->treinarBox->setItemText(0, "Binary");
    ui->treinarBox->setItemText(1, "Ternary");
    ui->treinarBox->setItemText(2, "Quaternary");
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
    this->gravarMenuShow(true);
    this->feedbackMenuShow(true);
    this->salvarMenuShow(false);
    ui->praticaLivreButton->hide();
}

void MainWindow::salvarMenuShow(bool enable)
{
    if (enable){
        ui->salvarButton->show();
        ui->vizualizarButton->show();
    }
    else{
        ui->salvarButton->hide();
        ui->vizualizarButton->hide();
    }
}

void MainWindow::feedbackMenuShow(bool enable)
{
    if (enable){
        // Show Audio Feedback interface
        ui->labelPulsiera->show();
        ui->pulsieraButton->show();
        ui->labelAudioFeedback->show();
        ui->playFeedbackButton->show();
        ui->volumeFeedbackSlider->show();
        ui->labelFeedbackVolume->show();
        ui->tapButton->show();
    }
    else{
        // Hide Audio Feedback interface
        ui->labelPulsiera->hide();
        ui->pulsieraButton->hide();
        ui->labelAudioFeedback->hide();
        ui->playFeedbackButton->hide();
        ui->volumeFeedbackSlider->hide();
        ui->labelFeedbackVolume->hide();
        ui->tapButton->hide();
    }
}

void MainWindow::gravarMenuShow(bool enable)
{
    if (enable){
        ui->startBt->show();
        ui->stopBt->show();
    }
    else {
        ui->startBt->hide();
        ui->stopBt->hide();
    }
}

void MainWindow::treinarShow(bool enable)
{
    if(enable){
        ui->labelCompasso->show();
        ui->treinarBox->show();
        ui->treinarButton->show();
        ui->treinarBox->show();
        ui->praticaLivreButton->show();
    }
    else {
        ui->labelCompasso->hide();
        ui->treinarBox->hide();
        ui->treinarButton->hide();
        ui->treinarBox->hide();
        ui->praticaLivreButton->hide();
    }
}

void MainWindow::IMBShow(bool enable)
{
    if(enable){
        ui->imbBox->show();
        ui->imbButton->show();
        ui->labelIBM->show();
    }
    else {
        ui->imbBox->hide();
        ui->imbButton->hide();
        ui->labelIBM->hide();
    }
}
