#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Vision.h"
#include "Weareable.h"
#include "Trajectory.h"
#include <QFile>

#include <QMainWindow>
#include <QMediaPlayer>


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
    QMediaPlayer* player;

private slots:

    void on_actionNovo_Gesto_triggered();

    void on_stopBt_clicked();

    void on_startBt_clicked();

    void on_actionTreinar_Gestos_triggered();

    void on_actionNovo_Gesto_PSMove_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    bool newGesture,correction;
};

#endif // MAINWINDOW_H
