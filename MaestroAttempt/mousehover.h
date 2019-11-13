#ifndef MOUSEHOVER_H
#define MOUSEHOVER_H

#include <QObject>
#include <QMediaPlayer>
#include <QEvent>
#include <QDebug>
#include <QFocusEvent>
#include "ui_mainwindow.h"
#include <QtCore>


class MouseHover : public QObject{
    Q_OBJECT
public:
    MouseHover(QMediaPlayer *mediaPlayer, Ui::MainWindow *mainwindow);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QMediaPlayer *player;
    Ui::MainWindow *ui;
};

#endif // MOUSEHOVER_H
