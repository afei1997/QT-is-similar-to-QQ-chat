#include "myinterface.h"
#include "ui_myinterface.h"
#include <QIcon>
#include <QDebug>
#include <QtEndian>
#include <QStandardItem>
#include <QString>

MyInterface::MyInterface(QTcpSocket *fd,QString str,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyInterface)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/oneimage/HeadImage.png"));
    s=fd;
    id=str;
    ui->idLabel->setText("id号："+id);

    s->waitForReadyRead();
    QByteArray arr1 = s->readAll();
    QString text="用户名：";
    text.append(arr1);
    ui->MyNameLabel->setText(text);

    s->waitForReadyRead();
    quint32 len;
    s->read((char *)&len, 4);
    len = qFromBigEndian(len);
    qDebug()<<"len"<<len <<endl;
    while (1) {
        s->waitForReadyRead();
        qDebug("-----recv context------%s------%lld-------\n", __func__,s->bytesAvailable());

        //套接字缓存是否至少有len字节数据
        if(s->bytesAvailable() < len){
            continue;
        }
        QByteArray arr = s->read(len);
        myicon.loadFromData(arr,"jpg");
        ui->MyIconLabel->setPixmap(myicon);
        break;
    }
    ui->treeView->header()->hide();
    model=new QStandardItemModel(ui->treeView);
    ui->treeView->setModel(model);
    item = new QStandardItem(tr("自己"));
    model->appendRow(item);
    QString myname;
    myname.append(arr1);
    QStandardItem * item1 = new QStandardItem(myicon," "+myname+" ("+str+")");
    item->appendRow(item1);

    s->waitForReadyRead();
    QString str1;
    QByteArray arr2 = s->readAll();
    str1.append(arr2);
    if(str1=="nofriend");
    else {
        myfriends=new QStandardItem(tr("好友"));
        model->appendRow(myfriends);
        int n=str1.toInt();
        qDebug() << "n=" << n << endl;
        for (int i=0;i<n;i++) {
            s->waitForReadyRead();
            QByteArray arr3 = s->readAll();
            QString fri;
            fri.append(arr3);
            qDebug()<< "friend=" <<fri << endl;
            QString friendid=fri.section(' ',2,2);
            QString friendname=fri.section(' ',1,1);
            int friendisonline=fri.section(' ',0,0).toInt();
            qDebug()<<"friendisonline="<<friendisonline<<endl;

            qDebug("75\n");
            s->waitForReadyRead();
            quint32 len;
            s->read((char *)&len, 4);
            len = qFromBigEndian(len);
            qDebug()<<"len"<<len <<endl;
            QPixmap friendicon;

            friendinfo node;
            node.id=friendid;
            node.name=friendname;
            node.isopen=false;
            while (1) {
                s->waitForReadyRead();
                qDebug("-----recv context------%s------%lld-------\n", __func__,s->bytesAvailable());

                //套接字缓存是否至少有len字节数据
                if(s->bytesAvailable() < len){
                    continue;
                }
                QByteArray arr = s->read(len);
                friendicon.loadFromData(arr,"jpg");
                node.icon=friendicon;
                break;
            }
            QStandardItem * myfriend =new QStandardItem(friendicon," "+friendname+" ("+friendid+")");
            if(0==friendisonline)
            {
                myfriend->setEnabled(false);
                node.isonline=false;
            }
            else {
                node.isonline=true;
            }
            myfriends->appendRow(myfriend);
            feiendinfos << node;
        }
    }

    connect(s,SIGNAL(readyRead()),this,SLOT(serversend()));
}

MyInterface::~MyInterface()
{
    delete ui;
}

void MyInterface::mousePressEvent(QMouseEvent *e)
{
    last = e->globalPos();
}
void MyInterface::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    last = e->globalPos();
    move(x()+dx, y()+dy);
}
void MyInterface::mouseReleaseEvent(QMouseEvent *e)
{
    int dx = e->globalX() - last.x();
    int dy = e->globalY() - last.y();
    move(x()+dx, y()+dy);
}

void MyInterface::serversend()
{
    QByteArray arr = s->readAll();
    QString data;
    data.append(arr);
    QString instruct=data.section(' ',0,0);
    if("9"==instruct)
    {
        QString upfriend=data.section(' ',1,1);
        qDebug()<<"upfriend="<<upfriend<<endl;
        changeStandardItem(upfriend,true);
    }
    else if("a"==instruct)
    {
        QString upfriend=data.section(' ',1,1);
        qDebug()<<"upfriend="<<upfriend<<endl;
        changeStandardItem(upfriend,false);
    }
    else if("6"==instruct)
    {
        QString sendfriend=data.section(' ',2,2);
        QList<friendinfo>::iterator ci;
        for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
            if(sendfriend==ci->id && ci->isopen)
            {
                QString sendData=data.section("（#）",1,1);
                qDebug()<<"sendData"<<sendData<<endl;
                connect(this,SIGNAL(signalRecvData(QString)),ci->confriend,SLOT(recvData(QString)),Qt::UniqueConnection);
                emit signalRecvData(sendData);
                ci->confriend->setWindowFlags(Qt::WindowStaysOnTopHint);
                ci->confriend->show();
                break;
            }
            else if(sendfriend==ci->id && !ci->isopen){
                ci->isopen=true;
                QString sendHead="6 ";
                sendHead+=ci->id;
                sendHead+=" ";
                sendHead+=id;
                sendHead+=" （#）";
                qDebug()<<"sendHead="<<sendHead<<endl;
                QString sendData=data.section("（#）",1,1);
                qDebug()<<"sendData"<<sendData<<endl;
                ci->confriend=new MyChat(" "+ci->name+" ("+ci->id+")",sendHead,ci->icon,s,sendData);
                ci->confriend->setWindowFlags(Qt::WindowStaysOnTopHint);
                ci->confriend->show();
                break;
            }
        }
    }
}

void MyInterface::on_treeView_doubleClicked(const QModelIndex &index)
{

    QString mydata=index.sibling(index.row(),0).data().toString();
    qDebug()<<"mydata="<<mydata<<endl;
    QString friendid=mydata.section('(',1,1).section(')',0,0);
    QList<friendinfo>::iterator ci;
    for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
        if(friendid==ci->id && ci->isonline && !ci->isopen)
        {
            ci->isopen=true;
            QString sendHead="6 ";
            sendHead+=ci->id;
            sendHead+=" ";
            sendHead+=id;
            sendHead+=" （#）";
            qDebug()<<"sendHead="<<sendHead<<endl;
            ci->confriend=new MyChat(mydata,sendHead,ci->icon,s);
            ci->confriend->show();
        }
        else if (friendid==ci->id && ci->isonline && ci->isopen) {
            ci->confriend->show();
        }
    }
}

void MyInterface::changeStandardItem(QString upfriend,bool n)
{
    qDebug()<<"row="<<myfriends->rowCount()<<endl;
    for (int i=0;i<=myfriends->row();i++) {
        QStandardItem* childItem =myfriends->child(i,0);
        QString data=childItem->text();
        qDebug()<<"data"<<data<<endl;
        qDebug()<<"upfriend"<<upfriend<<endl;
        if(data.contains(upfriend,Qt::CaseSensitive))
        {
            if(n){
                qDebug("true\n");
                myfriends->child(i,0)->setEnabled(true);
                ui->treeView->update();
                QList<friendinfo>::iterator ci;
                for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
                    if(ci->id==upfriend){
                        ci->isonline=true;
                    }
                }
                break;
            }
            else {
                qDebug("false\n");
                myfriends->child(i,0)->setEnabled(false);
                ui->treeView->update();
                QList<friendinfo>::iterator ci;
                for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
                    if(ci->id==upfriend){
                        ci->isonline=false;
                        ci->confriend->hide();
                    }
                }
                break;
            }
        }
    }
}

void MyInterface::closeEvent (QCloseEvent * e )
{
    QList<friendinfo>::iterator ci;
    for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
        if(ci->isopen)
            ci->confriend->close();
    }
    e->accept();
}
