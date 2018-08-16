#include "maestro.h"
#include "ui_maestro.h"

Maestro::Maestro(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Maestro)
{
    ui->setupUi(this);
}

Maestro::~Maestro()
{
    delete ui;
}
