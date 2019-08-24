#include "mythread.h"
#include <QtEndian>
#include <QBuffer>

MyThread::MyThread(QTcpSocket *s1,QObject *parent)
    : QThread (parent)
{
    s=s1;
    connect(s,SIGNAL(readyRead()),this,SLOT(serversend()));//关联套接字，有消息时触发
}

MyThread::~MyThread()
{

}

void MyThread::serversend()
{
    QByteArray arr = s->read(1024);
    QString data;
    data.append(arr);
    QString instruct=data.section(' ',0,0);
    qDebug()<<"recvdata"<<data<<endl;
    QPixmap friendicon;

    if("0"==instruct){//登录时传来的好友消息（头像、id、用户名）
        QString str=data.section(' ',1,1);
        if("nofriend(#)"!=str){
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
                friendicon.loadFromData(arr,"jpg");
                break;
            }
        }
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("4"==instruct) {//服务器传回更改用户名成功与否
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("5"==instruct) {//服务器传回更改头像成功与否
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("6"==instruct) {//服务器传回有发送消息
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("7"==instruct) {//服务器传回有好友添加请求
        int len=data.section(' ',3,3).toInt();
        qDebug()<<"len"<<len <<endl;
        while (1) {
            s->waitForReadyRead();
            qDebug("-----recv context------%s------%lld-------\n", __func__,s->bytesAvailable());

            //套接字缓存是否至少有len字节数据
            if(s->bytesAvailable() < len){
                continue;
            }
            QByteArray arr = s->read(len);
            friendicon.loadFromData(arr,"jpg");
            break;
        }
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("8"==instruct) {//服务器传回删除好友成功与否
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("9"==instruct) {//服务器传回有好友在线
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("a"==instruct) {//服务器传回有好友下线
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("b"==instruct) {//服务器传回添加好友回复
        QString str=data.section(' ',1,1);
        if ("yes"==str){
            int len=data.section(' ',4,4).toInt();
            qDebug()<<"len"<<len <<endl;
            while (1) {
                s->waitForReadyRead();
                qDebug("-----recv context------%s------%lld-------\n", __func__,s->bytesAvailable());

                //套接字缓存是否至少有len字节数据
                if(s->bytesAvailable() < len){
                    continue;
                }
                QByteArray arr = s->read(len);
                friendicon.loadFromData(arr,"jpg");
                break;
            }
        }
        emit signrecvdata(data,friendicon);
        return;
    }
    else if ("c"==instruct) {//服务器传回好友列表中有人更改自己的信息
        QString str=data.section(' ',1,1);
        if("name"==str){//更改用户名
            emit signrecvdata(data,friendicon);
            return;
        }
        else if ("icon"==str) {//更改头像
            int len=data.section(' ',3,3).toInt();
            while (1) {
                s->waitForReadyRead();
                qDebug("-----recv context------%s------%lld-------\n", __func__,s->bytesAvailable());

                //套接字缓存是否至少有len字节数据
                if(s->bytesAvailable() < len){
                    continue;
                }
                QByteArray arr = s->read(len);
                friendicon.loadFromData(arr,"jpg");
                break;
            }
            emit signrecvdata(data,friendicon);
            return;
        }
    }
}

void MyThread::sendicon(QPixmap changeIcon,QString id)
{
    QByteArray arr1;
    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    changeIcon.save(&buffer,"jpg");
    int pix_len = buffer.data().size();
    QString str1="5 ";
    str1+=id;
    str1+=" ";
    str1+=QString::number(pix_len);
    qDebug()<<"updata icon"<<str1<<endl;
    QByteArray arr;
    arr.append(str1);
    s->write(arr,1024);
    arr1.append(buffer.data());
    s->write(arr1);
    qDebug("-----%s-----",__func__);
}



