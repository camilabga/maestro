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

    // Class Functions
    Point audioFeedbackHandler(Point correctionValue, Point newValue);

    int distanceToHz(int distance, int steps);

    bool salvarCSV();

    bool salvarVideo();

    void praticarInterface();

    void treinarInterface();

    void salvarMenuShow(bool enable);

    void feedbackMenuShow(bool enable);

    void gravarMenuShow(bool enable);

    void treinarShow(bool enable);

    void IMBShow(bool enable);

    void setPortuguese();

    void setEnglish();

    // Slots Functions
    void MetronomoSlot();

    // Events functions
    void on_stopBt_clicked();

    void on_startBt_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_startMetronomeButton_clicked();

    void on_volumeFeedbackSlider_valueChanged(int value);

    void on_playFeedbackButton_clicked();

    void on_metronomeVolumeSlider_valueChanged(int value);

    void on_treinarButton_clicked();

    void on_languageButton_clicked();

    void on_vizualizarButton_clicked();

    void on_praticaLivreButton_clicked();

    void on_tapButton_clicked();

    void on_salvarButton_clicked();

    void on_pulsieraButton_clicked();

    void on_treinarBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    bool newGesture, correction;

    bool isPt, isEn, isMetrnonomeOn, isAudibleFeedbackOn,
         isTapOn, isTreinarOn, isLivreOn, gravarGesto,
         weareableIsOn;

    char* espIP;

    QString myFileName;

    int toneSampleRateHz;
    const int durationSeconds = 1;
    QAudioFormat format;

    QScopedPointer<Generator> m_generator;
    QScopedPointer<QAudioOutput> m_audioOutput;

    Point correctionValue;
    QSoundEffect proxEffect, startEffect, selectEffect;

    QSoundEffect metronomoTick;
    QTimer *metronomoTimer;
    int metronomoValue;

    QString fileName;
};

#endif // MAINWINDOW_H
