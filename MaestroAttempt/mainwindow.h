#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Vision.h"
#include "Weareable.h"
#include "Trajectory.h"

#include <QMainWindow>
#include <QMediaPlayer>
#include <QDebug>
#include <QFocusEvent>
#include <QPushButton>
#include <QtCore>
#include <QSoundEffect>
#include <QAudioOutput>
#include <QByteArray>
#include <QIODevice>

namespace Ui {
class MainWindow;
}

// Gerador de Som
class Generator : public QIODevice
{
    Q_OBJECT

public:
    Generator(const QAudioFormat &format, qint64 durationUs, int sampleRate);

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;

public:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);

private:
    qint64 m_pos = 0;
    QByteArray m_buffer;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:    // A slot or function is defined which will be intiated by timer
    void DisplayImage();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage imdisplay;  //This will create QImage which is shown in Qt label
    QTimer* Timer;   // A timer is needed in GUI application
    Vision vision;
    Weareable weareable;
    Trajectory trajectory;


private slots:

    void on_actionNovo_Gesto_triggered();

    void on_stopBt_clicked();

    void on_startBt_clicked();

    void on_actionTreinar_Gestos_triggered();

    void on_actionNovo_Gesto_PSMove_triggered();

    void MetronomoSlot();

    void on_spinBox_valueChanged(int arg1);

    void on_startMetronomeButton_clicked();

    void on_stopMetronomeButton_clicked();

    void on_volumeSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    bool newGesture,correction;

    int toneSampleRateHz;
    QAudioFormat format;

    QScopedPointer<Generator> m_generator;
    QScopedPointer<QAudioOutput> m_audioOutput;

    Point correctionValue;
    QSoundEffect correctionEffect;

    QSoundEffect metronomoTick;
    QTimer *metronomoTimer;
    int metronomoValue;
};

#endif // MAINWINDOW_H
