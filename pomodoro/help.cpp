#include "help.h"
#include "ui_help.h"

Help::Help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Popup);

    connect(ui->pushButton, SIGNAL(released()), this, SLOT(close()));
}

Help::~Help()
{
    delete ui;
}
