#include "mousehover.h"



MouseHover::MouseHover(QMediaPlayer *mediaplayer, Ui::MainWindow *mainwindow)
{
    player = mediaplayer;
    ui = mainwindow;
}


bool MouseHover::eventFilter(QObject *obj, QEvent *event) {
    QString path = "/home/angelo/_Angelo/_Projetos/maestro/MaestroAttempt/audioFiles/";
    if(event->type() == QEvent::FocusIn || event->type() == QEvent::HoverEnter) {

        /*                         *
         * Menu de Sleção superior *
         *                         */
        if (obj->objectName() == "praticaLivreButton"){
            player->setMedia(QUrl::fromLocalFile(path +"treinolivre.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "salvarButton"){
            player->setMedia(QUrl::fromLocalFile(path +"salvarvideo.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "vizualizarButton"){
            player->setMedia(QUrl::fromLocalFile(path +"gerargrafico.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "imbButton"){
            player->setMedia(QUrl::fromLocalFile(path +"treinarimb.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "imbBox") {
            if (ui->imbBox->currentIndex() == 0){
                player->setMedia(QUrl::fromLocalFile(path + "quadrado.wav"));
                player->play();
                qDebug() << player->errorString();
            }
            else if (ui->imbBox->currentIndex() == 1){
                player->setMedia(QUrl::fromLocalFile(path + "circulo.wav"));
                player->play();
                qDebug() << player->errorString();
            }
        }
        else if (obj->objectName() == "treinarButton"){
            player->setMedia(QUrl::fromLocalFile(path +"treinarcompasso.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "treinarBox") {
            if (ui->treinarBox->currentIndex() == 0){
                player->setMedia(QUrl::fromLocalFile(path + "binario.wav"));
                player->play();
                qDebug() << player->errorString();
            }
            else if (ui->treinarBox->currentIndex() == 1){
                player->setMedia(QUrl::fromLocalFile(path + "ternario.wav"));
                player->play();
                qDebug() << player->errorString();
            }
            else if (ui->treinarBox->currentIndex() == 2){
                player->setMedia(QUrl::fromLocalFile(path + "quaternario.wav"));
                player->play();
                qDebug() << player->errorString();
            }
        }

        /*                          *
         * Retorno Tatil e Auditivo *
         *                          */
        else if (obj->objectName() == "pulsieraButton"){
            player->setMedia(QUrl::fromLocalFile(path +"comecarPararTatil.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "playFeedbackButton"){
            player->setMedia(QUrl::fromLocalFile(path +"comecarPararAuditivo.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "volumeFeedbackSlider"){
            player->setMedia(QUrl::fromLocalFile(path +"volumeAuditivo.wav"));
            player->play();
            qDebug() << player->errorString();
        }

        /*                        *
         * Metônomo, Tap e Idioma *
         *                        */
        else if (obj->objectName() == "startMetronomeButton"){
            player->setMedia(QUrl::fromLocalFile(path +"comecarPararMetronomo.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "metronomeVolumeSlider"){
            player->setMedia(QUrl::fromLocalFile(path +"volumeMetronomo.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "spinBox"){
            player->setMedia(QUrl::fromLocalFile(path +"bpm.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "tapButton"){
            player->setMedia(QUrl::fromLocalFile(path +"tap.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "languageButton"){
            player->setMedia(QUrl::fromLocalFile(path +"pten.wav"));
            player->play();
            qDebug() << player->errorString();
        }

        /*                        *
         * Começar e Parar Treino *
         *                        */
        else if (obj->objectName() == "startBt"){
            player->setMedia(QUrl::fromLocalFile(path +"comecar.wav"));
            player->play();
            qDebug() << player->errorString();
        }
        else if (obj->objectName() == "stopBt"){
            player->setMedia(QUrl::fromLocalFile(path +"parar.wav"));
            player->play();
            qDebug() << player->errorString();
        }

        qDebug() << obj->objectName();
    }
    /*else if(event->type() == QEvent::HoverLeave || event->type() == QEvent::FocusOut){
        player->setMedia(QUrl::fromLocalFile(""));
        player->pause();
    }*/

    return QObject::eventFilter(obj, event);
}

