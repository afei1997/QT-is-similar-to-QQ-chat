#include "myinterface.h"
#include "ui_myinterface.h"

MyInterface::MyInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyInterface)
{
    ui->setupUi(this);
}

MyInterface::~MyInterface()
{
    delete ui;
}
