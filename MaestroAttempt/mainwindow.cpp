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


// Gerador de Som
#include <qmath.h>
#include <qendian.h>

Generator::Generator(const QAudioFormat &format
                     , qint64 durationUs
                     , int sampleRate)
{
    if (format.isValid())
        generateData(format, durationUs, sampleRate);
}

void Generator::start()
{
    open(QIODevice::ReadOnly);
}

void Generator::stop()
{
    m_pos = 0;
    close();
}

void Generator::generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate)
{
    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channelCount() * channelBytes;
    qint64 length = (format.sampleRate() * format.channelCount() * (format.sampleSize() / 8))
            * durationUs / 1000000;
    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes) // suppress warning in release builds

    m_buffer.resize(length);
    unsigned char *ptr = reinterpret_cast<unsigned char *>(m_buffer.data());
    int sampleIndex = 0;

    while (length) {
        // Produces value (-1..1)
        const qreal x = qSin(2 * M_PI * sampleRate * qreal(sampleIndex++ % format.sampleRate()) / format.sampleRate());
        for (int i=0; i<format.channelCount(); ++i) {
            if (format.sampleSize() == 8) {
                if (format.sampleType() == QAudioFormat::UnSignedInt) {
                    const quint8 value = static_cast<quint8>((1.0 + x) / 2 * 255);
                    *reinterpret_cast<quint8 *>(ptr) = value;
                } else if (format.sampleType() == QAudioFormat::SignedInt) {
                    const qint8 value = static_cast<qint8>(x * 127);
                    *reinterpret_cast<qint8 *>(ptr) = value;
                }
            } else if (format.sampleSize() == 16) {
                if (format.sampleType() == QAudioFormat::UnSignedInt) {
                    quint16 value = static_cast<quint16>((1.0 + x) / 2 * 65535);
                    if (format.byteOrder() == QAudioFormat::LittleEndian)
                        qToLittleEndian<quint16>(value, ptr);
                    else
                        qToBigEndian<quint16>(value, ptr);
                } else if (format.sampleType() == QAudioFormat::SignedInt) {
                    qint16 value = static_cast<qint16>(x * 32767);
                    if (format.byteOrder() == QAudioFormat::LittleEndian)
                        qToLittleEndian<qint16>(value, ptr);
                    else
                        qToBigEndian<qint16>(value, ptr);
                }
            }

            ptr += channelBytes;
            length -= channelBytes;
        }
    }
}

qint64 Generator::readData(char *data, qint64 len)
{
    qint64 total = 0;
    if (!m_buffer.isEmpty()) {
        while (len - total > 0) {
            const qint64 chunk = qMin((m_buffer.size() - m_pos), len - total);
            memcpy(data + total, m_buffer.constData() + m_pos, chunk);
            m_pos = (m_pos + chunk) % m_buffer.size();
            total += chunk;
        }
    }
    return total;
}

qint64 Generator::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

qint64 Generator::bytesAvailable() const
{
    return m_buffer.size() + QIODevice::bytesAvailable();
}

// Main Window
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    correctionEffect(this),
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
        vision.drawTrajectory(trajectory, trajectory.getCurrentPointId());
        if (vision.isTargetOn()) {
            trajectory.setNextPoint0(vision.getCenter());
            newValue = vision.drawError(vision.getCenter(), trajectory.getCurrentPoint());
            //weareable.send(trajectory.getError(vision.getCenter()));
            trajectory.savePoint(vision.getCenter());
        }
    }

    // Plays audible feedback
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
