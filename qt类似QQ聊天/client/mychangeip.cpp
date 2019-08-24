#include "mychangeip.h"
#include "ui_mychangeip.h"
#include <QRegExpValidator>
#include <QFile>
#include <QMessageBox>
#include <QDebug>

MyChangeIp::MyChangeIp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyChangeIp)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint);
    setWindowIcon(QIcon(":/oneimage/HeadImage.png"));
    setWindowTitle("更改ip和端口号");

    ui->ipEdit1->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->ipEdit2->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->ipEdit3->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->ipEdit4->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    ui->portEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));

    ui->ipEdit1->setPlaceholderText(QStringLiteral("127"));
    ui->ipEdit2->setPlaceholderText(QStringLiteral("0"));
    ui->ipEdit3->setPlaceholderText(QStringLiteral("0"));
    ui->ipEdit4->setPlaceholderText(QStringLiteral("1"));
    ui->portEdit->setPlaceholderText(QStringLiteral("8888"));
}

MyChangeIp::~MyChangeIp()
{
    delete ui;
}

void MyChangeIp::on_changeIpBt_clicked()
{
    if(ui->ipEdit1->text().isEmpty() && ui->ipEdit2->text().isEmpty()
           && ui->ipEdit3->text().isEmpty() && ui->ipEdit4->text().isEmpty()
            && ui->portEdit->text().isEmpty());
    else if (ui->ipEdit1->text().isEmpty() && ui->ipEdit2->text().isEmpty()
             && ui->ipEdit3->text().isEmpty() && ui->ipEdit4->text().isEmpty()
              && !ui->portEdit->text().isEmpty()) {
        int port=ui->portEdit->text().toInt();
        if((port<1024) || (port>65535))
        {
            QMessageBox::warning(this, "提示","端口号为1024-65535之间",
                                 QMessageBox::Ok);
            ui->portEdit->setText("");
            return;
        }
        QString ip_port="127.0.0.1 ";
        ip_port+=ui->portEdit->text();
        QFile file("ip_port.txt");
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed." << endl;
            return ;
        }
        QByteArray arr;
        arr.append(ip_port);
        file.write(arr);
        file.close();
        emit signalChange();
        QMessageBox::warning(this, "提示","更改成功",
                             QMessageBox::Ok);
        this->close();
    }

    else if (!ui->ipEdit1->text().isEmpty() && !ui->ipEdit2->text().isEmpty()
             && !ui->ipEdit3->text().isEmpty() && !ui->ipEdit4->text().isEmpty()
              && ui->portEdit->text().isEmpty()) {
        int ip1=ui->ipEdit1->text().toInt();
        int ip2=ui->ipEdit2->text().toInt();
        int ip3=ui->ipEdit3->text().toInt();
        int ip4=ui->ipEdit4->text().toInt();
        if(ip1>255 || ip2>255 || ip3 >255 || ip4>255){
            QMessageBox::warning(this, "提示","ip每段应在0-255",
                                 QMessageBox::Ok);
            ui->ipEdit1->setText("");
            ui->ipEdit2->setText("");
            ui->ipEdit3->setText("");
            ui->ipEdit4->setText("");
            return;
        }
        QString ip_port=ui->ipEdit1->text();
        ip_port+=".";
        ip_port+=ui->ipEdit2->text();
        ip_port+=".";
        ip_port+=ui->ipEdit3->text();
        ip_port+=".";
        ip_port+=ui->ipEdit4->text();
        ip_port+=" ";
        ip_port+="8888";
        QFile file("ip_port.txt");
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed." << endl;
            return ;
        }
        QByteArray arr;
        arr.append(ip_port);
        file.write(arr);
        file.close();
        emit signalChange();
        QMessageBox::warning(this, "提示","更改成功",
                             QMessageBox::Ok);
        this->close();
    }

    else if (!ui->ipEdit1->text().isEmpty() && !ui->ipEdit2->text().isEmpty()
             && !ui->ipEdit3->text().isEmpty() && !ui->ipEdit4->text().isEmpty()
              && !ui->portEdit->text().isEmpty()) {
        int ip1=ui->ipEdit1->text().toInt();
        int ip2=ui->ipEdit2->text().toInt();
        int ip3=ui->ipEdit3->text().toInt();
        int ip4=ui->ipEdit4->text().toInt();
        if(ip1>255 || ip2>255 || ip3 >255 || ip4>255){
            QMessageBox::warning(this, "提示","ip每段应在0-255",
                                 QMessageBox::Ok);
            ui->ipEdit1->setText("");
            ui->ipEdit2->setText("");
            ui->ipEdit3->setText("");
            ui->ipEdit4->setText("");
            return;
        }
        int port=ui->portEdit->text().toInt();
        if((port<1024) || (port>65535))
        {
            QMessageBox::warning(this, "提示","端口号为1024-65535之间",
                                 QMessageBox::Ok);
            ui->portEdit->setText("");
            return;
        }
        QString ip_port=ui->ipEdit1->text();
        ip_port+=".";
        ip_port+=ui->ipEdit2->text();
        ip_port+=".";
        ip_port+=ui->ipEdit3->text();
        ip_port+=".";
        ip_port+=ui->ipEdit4->text();
        ip_port+=" ";
        ip_port+=ui->portEdit->text();
        QFile file("ip_port.txt");
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Open failed." << endl;
            return ;
        }
        QByteArray arr;
        arr.append(ip_port);
        file.write(arr);
        file.close();
        emit signalChange();
        QMessageBox::warning(this, "提示","更改成功",
                             QMessageBox::Ok);
        this->close();
    }
}
