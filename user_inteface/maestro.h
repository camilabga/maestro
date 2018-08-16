#ifndef MAESTRO_H
#define MAESTRO_H

#include <QMainWindow>

namespace Ui {
class Maestro;
}

class Maestro : public QMainWindow
{
    Q_OBJECT

public:
    explicit Maestro(QWidget *parent = 0);
    ~Maestro();

private:
    Ui::Maestro *ui;
};

#endif // MAESTRO_H
