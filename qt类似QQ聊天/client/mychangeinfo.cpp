#include "mychangeinfo.h"
#include "ui_mychangeinfo.h"
#include <QMessageBox>
#include <QRegExpValidator>
#include <QFileDialog>

MyChangeInfo::MyChangeInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyChangeInfo)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/oneimage/HeadImage.png"));
    setWindowTitle("更改信息");
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    ui->addEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

MyChangeInfo::~MyChangeInfo()
{
    delete ui;
}

void MyChangeInfo::on_addBtn_clicked()
{
    if(ui->addEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "警告","要添加的朋友不能为空",
                             QMessageBox::Ok);
        return;
    }
    else if (ui->addEdit->text().size()<8) {
        QMessageBox::warning(this, "警告","要添加的朋友id小于8位",
                             QMessageBox::Ok);
        ui->addEdit->setText("");
        return;
    }
    else{
        QString str="7 ";
        str+=ui->addEdit->text();
        emit signalChange(str);
        ui->addEdit->setText("");
    }
}

void MyChangeInfo::on_deleteBtn_clicked()
{
    if(ui->deleteEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "警告","要添加的朋友不能为空",
                             QMessageBox::Ok);
        return;
    }
    else if (ui->deleteEdit->text().size()<8) {
        QMessageBox::warning(this, "警告","要添加的朋友id小于8位",
                             QMessageBox::Ok);
        ui->deleteEdit->setText("");
        return;
    }
    else{
        QString str="8 ";
        str+=ui->deleteEdit->text();
        emit signalChange(str);
        ui->deleteEdit->setText("");
    }
}

void MyChangeInfo::on_nameBtn_clicked()
{
    if(ui->nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "警告","要更改的用户名不能为空",
                             QMessageBox::Ok);
        return;
    }
    else {
        QString str="4 ";
        str+=ui->nameEdit->text();
        emit signalChange(str);
        ui->nameEdit->setText("");
    }
}

void MyChangeInfo::on_imageBtn_clicked()
{
    QString file=QFileDialog::getOpenFileName(this,"添加头像", nullptr,"图片(*.jpg)");
    ui->iconEdit->setText(file);
}

void MyChangeInfo::on_iconBtn_clicked()
{
    if(ui->iconEdit->text().isEmpty()){
        QMessageBox::warning(this, "警告","请先选择文件路径",
                             QMessageBox::Ok);
        return;
    }
    else {
        QString str="5 ";
        str+=ui->iconEdit->text();
        emit signalChange(str);
        ui->iconEdit->setText("");
    }
}
