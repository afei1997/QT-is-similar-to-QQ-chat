#include "widget.h"
#include "ui_widget.h"
#include <QMovie>
#include <QRegExpValidator>
#include <QDebug>
#include "myregister.h"
#include <QHostAddress>
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    s = new QTcpSocket(this);
    s->connectToHost(QHostAddress("127.0.0.1"), 8888);

    setWindowFlags(Qt::FramelessWindowHint);
    QMovie *movie = new QMovie();
    movie->setFileName(":/oneimage/back.gif");
    ui->label->setMovie(movie);
    movie->start();
    ui->label->move(0, 0);

    //暗注释;
    ui->nameEdit->setPlaceholderText(QStringLiteral("用户id（至少8位数）"));
    ui->passwordEdit->setPlaceholderText(QStringLiteral("用户密码"));

    setWindowIcon(QIcon(":/oneimage/HeadImage.png"));
    ui->nameEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}
void Widget::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx, y()+dy);
}
void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}

void Widget::on_loginButton_clicked()
{
    if((ui->nameEdit->text().isEmpty()) || (ui->passwordEdit->text().isEmpty())
            || (ui->nameEdit->text().size()<8))
    {
        QMessageBox::warning(this, "警告","请将信息填写完毕",
                             QMessageBox::Ok);
        return;
    }
    else if (ui->nameEdit->text().size()<8) {
        QMessageBox::warning(this, "警告","用户id应为8位数",
                             QMessageBox::Ok);
        return;
    }
    else {
        ui->loginButton->setEnabled(false);
        QString chmond="0 ";
        chmond+=ui->nameEdit->text();
        chmond+=" ";
        chmond+=ui->passwordEdit->text();
        QByteArray arr;
        arr.append(chmond);
        s->write(arr);

        s->waitForReadyRead();
        QByteArray arr1 = s->readAll();
        QString text;
        text.append(arr1);
        qDebug()<<"text"<<text<<endl;
        if(text=="no")
        {
            QMessageBox::warning(this, "提示","用户名或密码不正确",
                                 QMessageBox::Ok);
            ui->nameEdit->setText("");
            ui->passwordEdit->setText("");

        }
        else if (text=="online") {
            QMessageBox::warning(this, "提示","用户已经在线",
                                 QMessageBox::Ok);
        }
        else {

        }
        ui->loginButton->setEnabled(true);
    }
}

void Widget::on_pButtonRegistAccount_clicked()
{
    MyRegister *re=new MyRegister;
    this->close();
    re->show();
}
