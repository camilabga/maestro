#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Vision.h"
#include "Weareable.h"
#include "Trajectory.h"
#include "generator.h"

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

    Point audioFeedbackHandler(Point correctionValue, Point newValue);

    int distanceToHz(int distance, int steps);

    void on_stopBt_clicked();

    void on_startBt_clicked();

    void MetronomoSlot();

    void on_spinBox_valueChanged(int arg1);

    void on_startMetronomeButton_clicked();

    void on_stopMetronomeButton_clicked();

    void on_volumeFeedbackSlider_valueChanged(int value);

    void on_playFeedbackButton_clicked();

    void on_stopFeedbackButton_clicked();

    void on_metronomeVolumeSlider_valueChanged(int value);

    void on_novoGestoButton_clicked();

    void on_treinarButton_clicked();

    void setPortuguese();

    void setEnglish();

    void on_languageButton_clicked();

private:
    Ui::MainWindow *ui;

    bool newGesture, correction;

    bool isPt, isEn;

    int toneSampleRateHz;
    const int durationSeconds = 1;
    QAudioFormat format;

    QScopedPointer<Generator> m_generator;
    QScopedPointer<QAudioOutput> m_audioOutput;

    Point correctionValue;
    QSoundEffect proxEffect;

    QSoundEffect metronomoTick;
    QTimer *metronomoTimer;
    int metronomoValue;
};

#endif // MAINWINDOW_H
