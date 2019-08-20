#include "myregister.h"
#include "ui_myregister.h"
#include "QRegExpValidator"
#include "widget.h"
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>

MyRegister::MyRegister(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyRegister)
{
    ui->setupUi(this);
    s = new QTcpSocket(this);
    s->connectToHost(QHostAddress("127.0.0.1"), 8888);

    ui->idEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    setWindowTitle("注册页面");
    setWindowIcon(QIcon(":/oneimage/HeadImage.png"));
    ui->idEdit->setPlaceholderText(QStringLiteral("至少8位数"));
    ui->nameEdit->setPlaceholderText(QStringLiteral("个性名字"));
    ui->passEdit->setPlaceholderText(QStringLiteral("用于以后登录的密码"));
    ui->oncePassEdit->setPlaceholderText(QStringLiteral("再次确认密码"));

    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);

}

MyRegister::~MyRegister()
{
    delete ui;
}

void MyRegister::on_registerBtn_clicked()
{
    if((ui->idEdit->text().isEmpty()) || (ui->nameEdit->text()
           .isEmpty()) || (ui->passEdit->text().isEmpty()) ||
            (ui->oncePassEdit->text().isEmpty()))
    {
        QMessageBox::warning(this, "警告","请将信息填写完毕",
                             QMessageBox::Ok);
        return;
    }
    else if(ui->passEdit->text()!=ui->oncePassEdit->text())
    {
        QMessageBox::warning(this, "警告","两次密码不一致",
                             QMessageBox::Ok);
        ui->passEdit->setText("");
        ui->oncePassEdit->setText("");///
        return;
    }
    else if (ui->idEdit->text().size()<8) {
        QMessageBox::warning(this, "警告","id长度小于8",
                             QMessageBox::Ok);
        ui->passEdit->setText("");
        ui->oncePassEdit->setText("");
        ui->idEdit->setText("");
        ui->nameEdit->setText("");
    }
    else {
        ui->registerBtn->setEnabled(false);

        QString str="1 ";
        str+=ui->idEdit->text();
        str+=" ";
        str+=ui->nameEdit->text();
        str+=" ";
        str+=ui->passEdit->text();
        qDebug() << str << endl;
        QByteArray arr;
        arr.append(str);
        s->write(arr);

        s->waitForReadyRead();
        QByteArray arr1 = s->readAll();
        QString text;
        text.append(arr1);
        qDebug()<<"text"<<text<<endl;
        if(text=="yes")
        {
            QMessageBox::warning(this, "提示","注册成功",
                                 QMessageBox::Ok);
            s->close();
            this->hide();
            emit mySignal(true);
            this->close();
        }
        else if (text=="existing") {
            QMessageBox::warning(this, "提示","用户已经存在",
                                 QMessageBox::Ok);
            ui->registerBtn->setEnabled(true);
            ui->passEdit->setText("");
            ui->oncePassEdit->setText("");
            ui->idEdit->setText("");
            ui->nameEdit->setText("");
        }
        else {
            QMessageBox::warning(this, "提示","注册失败",
                                 QMessageBox::Ok);
            ui->registerBtn->setEnabled(true);
            ui->passEdit->setText("");
            ui->oncePassEdit->setText("");
            ui->idEdit->setText("");
            ui->nameEdit->setText("");
        }
    }
}
