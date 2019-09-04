# Running Qt application

Firstly you must Download and install Qt Creator and its dependencies with [Qt Online Installer](https://www.qt.io/download).

- Then, open the Qt Creator and import the [`MaestroAttempt.pro`](MaestroAttempt.pro) file with **Open Project** option in the Welcome menu.

- With the project opened, go to the Edit menu and edit the [`MaestroAttempt.pro`](MaestroAttempt.pro) file on these lines:

        ...

        QT       += core gui multimedia

        ...

        INCLUDEPATH += (YOUR_PATH_TO_OPENCV)
        INCLUDEPATH += (YOUR_PATH_TO_ARUCO_SRC)
        LIBS += -L(YOUR_PATH_TO_ARUCO_BUILD) -L(YOUR_PATH_TO_C++_LIBS) -L(YOUR_PATH_TO_C++_LIBS) -laruco -lopencv_(OPENCV_LIB_1) -lopencv_(OPENCV_LIB_2) -lopencv_(OPENCV_LIB_3) ... -lopencv_(OPENCV_LIB_N)

At this point you should compile to make sure it all works

- Finally, edit the static paths to the audio files in the [`mainwindow.cpp`](mainwindow.cpp) to look like this:

        ...

        /*******************
            Main Window
        ********************/
        MainWindow::MainWindow(QWidget *parent) :
            QMainWindow(parent),
            ui(new Ui::MainWindow),
            correctionEffect(this),
            proxEffect(this),
            metronomoTick(this)
        {
            ...

            // Aproximação
            proxEffect.setSource(QUrl::fromLocalFile("/(YOUR_PATH_TO_THE_maestro_FOLDER)/maestro/MaestroAttempt/audioFiles/metronome_click.wav"));
            proxEffect.setVolume(0.25f);

            ...

            // Metronomo
            metronomoValue = 60000/40;
            metronomoTick.setSource(QUrl::fromLocalFile("/(YOUR_PATH_TO_THE_maestro_FOLDER)/maestro/MaestroAttempt/audioFiles/metronome_click.wav"));
            metronomoTick.setVolume(0.25f);
            metronomoTimer = new QTimer(this);
            connect(metronomoTimer, SIGNAL(timeout()), this, SLOT(MetronomoSlot()));

        }

        ...

Now you should be able to compile everything and run!
