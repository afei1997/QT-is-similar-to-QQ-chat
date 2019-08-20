#include "register.h"
#include "ui_register.h"

register::register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::register)
{
    ui->setupUi(this);
    ui->idEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

register::~register()
{
delete ui;
}
