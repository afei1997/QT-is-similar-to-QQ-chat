#include "mychat.h"
#include "ui_mychat.h"
#include <QIcon>
#include <QDebug>

MyChat::MyChat(QString str,QString str1,QPixmap icon,QTcpSocket *s1,QString one,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyChat)
{
    ui->setupUi(this);
    s=s1;
    setWindowTitle(str);
    sendHead=str1;
    windowIcon=icon;
    setWindowIcon(QIcon(windowIcon));
    if(nullptr!=one)
    {
        QString recv="对方说：";
        recv+=one;
        recv+="\n\n";
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEdit->insertPlainText(recv);
    }
    ui->textEdit->setReadOnly(true);
}

MyChat::~MyChat()
{
    delete ui;
}

void MyChat::recvData(QString data)
{
    QString sendData=data;
    qDebug()<<"data"<<data<<endl;
    QString recv="对方说：";
    recv+=sendData;
    recv+="\n\n";
    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertPlainText(recv);
}

void MyChat::on_sendBtn_clicked()
{
    if(ui->sendEdit->text().isEmpty())
        return;
    else {
        QString send="自己说：";
        send+=ui->sendEdit->text();
        send+="\n\n";
        ui->textEdit->moveCursor(QTextCursor::End);
        ui->textEdit->insertPlainText(send);
        QString sendData=sendHead;
        sendData+=ui->sendEdit->text();
        QByteArray arr;
        arr.append(sendData);
        s->write(arr);
        ui->sendEdit->setText("");
    }
}

void MyChat::closeEvent (QCloseEvent * e )
{
    e->ignore();
    this->hide();
}
