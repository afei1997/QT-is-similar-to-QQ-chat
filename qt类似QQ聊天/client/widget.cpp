#include "widget.h"
#include "ui_widget.h"
#include "myinterface.h"
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

    QFile file("ip_port.txt");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QByteArray line = file.readLine();
    QString ip_port;
    ip_port.append(line);
    if(ip_port==nullptr)
    {
        ip="127.0.0.1";
        port=quint16(8888);
        ip_port="127.0.0.1 ";
        ip_port+="8888";
        QByteArray new_ip_port;
        new_ip_port.append(ip_port);
        file.write(new_ip_port);
    }
    else {
        ip=ip_port.section(' ',0,0);
        port=quint16(ip_port.section(' ',1,1).toUInt());
    }
    s = new QTcpSocket(this);
    s->connectToHost(QHostAddress(ip), port);

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

    connect(&change,SIGNAL(signalChange()),this,SLOT(changeIP()));

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
    if((ui->nameEdit->text().isEmpty()) || (ui->passwordEdit->text().isEmpty()))
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
        qDebug() << "chmond" <<chmond <<endl;
        QByteArray arr;
        arr.append(chmond);
        s->write(arr);

        s->waitForReadyRead();
        QByteArray arr1 = s->readAll();
        QString text;
        text.append(arr1);
        qDebug()<<"text"<<text<<endl;
        if(text.isEmpty())
        {
            QMessageBox::warning(this, "警告","服务器退出或者ip地址或端口号不对，可选择右上方进行改变",
                                 QMessageBox::Ok);
            return ;
        }
        else if(text=="no")
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
        else if(text=="yes"){
            MyInterface *in=new MyInterface(s,ui->nameEdit->text());
            in->show();
            this->hide();
        }
        ui->loginButton->setEnabled(true);
    }
}

void Widget::on_pButtonRegistAccount_clicked()
{

    MyRegister *re=new MyRegister(s);
    connect(re,SIGNAL(mySignal(bool)),this,SLOT(tomainSlot(bool)));
    re->show();
    this->hide();
}

void Widget::tomainSlot(bool n)
{
    if(n)
    {
        qDebug("%s\n",__func__);
        this->show();
    }
    else {
        this->close();
    }
}

void Widget::changeIP()
{
    s->close();
    QFile file("ip_port.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QByteArray line = file.readLine();
    QString ip_port;
    ip_port.append(line);
    ip=ip_port.section(' ',0,0);
    port=quint16(ip_port.section(' ',1,1).toUInt());

    s = new QTcpSocket(this);
    s->connectToHost(QHostAddress(ip), port);
}

void Widget::on_pButtonArrow_clicked()
{
    change.show();
}
