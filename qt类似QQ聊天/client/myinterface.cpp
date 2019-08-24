#include "myinterface.h"
#include "ui_myinterface.h"
#include <QIcon>
#include <QDebug>
#include <QtEndian>
#include <QStandardItem>
#include <QString>
#include <QMessageBox>

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
    flag=0;

    s->waitForReadyRead();
    QByteArray arr1 = s->read(1024);
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

    thr=new MyThread(s,this);
    connect(thr,SIGNAL(signrecvdata(QString,QPixmap)),this,SLOT(serversend(QString,QPixmap)));
    connect(this,SIGNAL(signThread(QPixmap,QString)),thr,SLOT(sendicon(QPixmap,QString)));
    connect(&changeinfo,SIGNAL(signalChange(QString)),this,SLOT(change(QString)));
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

void MyInterface::serversend(QString data,QPixmap friendicon)
{

    QString instruct=data.section(' ',0,0);
    qDebug()<<"recvdata"<<data<<endl;

    if ("0"==instruct) {//接收到线程好友信息
        QString str=data.section(' ',1,1);
        if("nofriend(#)"==str){
            QMessageBox::warning(this, "提示",
                                   "你还没有添加好友\n可点击窗口右上方\n或者点击列表的自己信息\n可以进行添加好友",
                         QMessageBox::Ok);
        }
        else {
            if(0==flag){
                myfriends=new QStandardItem(tr("好友"));
                model->appendRow(myfriends);
            }
            QString friendid=data.section(' ',3,3);
            QString friendname=data.section(' ',2,2);
            int friendisonline=data.section(' ',1,1).toInt();

            friendinfo node;
            node.id=friendid;
            node.name=friendname;
            node.isopen=false;
            node.icon=friendicon;
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
            flag++;
        }
    }
    else if ("4"==instruct) {//接收到线程更改自己用户名成功与否
        QString str=data.section(' ',1,1);
        if("no"==str){
            QMessageBox::warning(this, "警告",
                                   "更改失败",
                         QMessageBox::Ok);
        }
        else if ("yes"==str) {
            QMessageBox::warning(this, "警告",
                                   "更改成功",
                         QMessageBox::Ok);
            QString str1="用户名：";
            str1+=data.section(' ',2,2);
            ui->MyNameLabel->setText(str1);
            item->child(0,0)->setText(" "+data.section(' ',2,2)+" ("+id+")");
            ui->treeView->update();
            changeinfo.hide();
        }
    }
    else if ("5"==instruct) {//接收到线程更改头像成功与否
        QString str=data.section(' ',1,1);
        if("no"==str){
            QMessageBox::warning(this, "警告",
                                   "更改失败",
                         QMessageBox::Ok);
        }
        else if ("yes"==str) {
            QMessageBox::warning(this, "提示",
                                   "更改成功",
                         QMessageBox::Ok);
            myicon=changeIcon;
            item->child(0,0)->setIcon(myicon);
            ui->treeView->update();
            ui->MyIconLabel->setPixmap(myicon);
            changeinfo.hide();
        }
    }
    else if ("6"==instruct)//接收到线程有发送消息
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
    else if ("7"==instruct) {//接收到线程有好友添加请求
        QByteArray arr;
        tempname=data.section(' ',1,1);
        tempid=data.section(' ',2,2);
        tempicon=QPixmap();
        tempicon=friendicon;
        qDebug()<<"tempname"<<tempname<<endl;
        qDebug()<<"tempid"<<tempid<<endl;
        friendinfo newfriend;
        QStandardItem * myfriend;
        QMessageBox::StandardButton btn=
                QMessageBox::warning(this, "提示",
                                       tempname+" ("+tempid+")"+"请求添加好友",
                             QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        switch (btn) {
        case QMessageBox::Yes:
            arr.append("b yes");
            s->write(arr,1024);
            newfriend.id=tempid;
            newfriend.name=tempname;
            newfriend.isonline=true;
            newfriend.icon=tempicon;
            newfriend.isopen=false;
            feiendinfos<<newfriend;
            myfriend =new QStandardItem(tempicon," "+tempname+" ("+tempid+")");
            if(0==flag){
                myfriends=new QStandardItem(tr("好友"));
                model->appendRow(myfriends);
            }
            myfriends->appendRow(myfriend);
            ui->treeView->update();
            flag++;
            break;
        default:
            arr.append("b no");
            s->write(arr,1024);
            break;
        }
    }
    else if ("8"==instruct) {//接收到线程删除好友成功与否
        QString str=data.section(' ',1,1);
        if("no"==str){
            return ;
        }
        else if ("haveno"==str) {
            QMessageBox::warning(this, "警告",
                                   "本就无该好友",
                         QMessageBox::Ok);
            return ;
        }
        else {

            QString str2;
            if("yes"==str){
                str2=data.section(' ',2,2);
            }
            else {
                str2=data.section(' ',1,1);
            }
            int n=0;
            QList<friendinfo>::iterator ci;
            for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
                if(ci->id==str2){
                    feiendinfos.removeAt(n);
                    break;
                }
                n++;
            }
            for (int i=0;i<=myfriends->row();i++) {
                QStandardItem* childItem =myfriends->child(i,0);
                QString data=childItem->text();
                qDebug()<<"data"<<data<<endl;
                if(data.contains(str2,Qt::CaseSensitive))
                {
                    myfriends->removeRow(i);
                    flag--;
                    qDebug()<<"flag="<<flag<<endl;
                    if(0==flag)
                    {
                        model->removeRow(1);
                    }
                    ui->treeView->update();
                    break;
                }
            }
            if("yes"==str){
                QMessageBox::warning(this, "提示",
                                       "删除成功",
                             QMessageBox::Ok);
            }
            changeinfo.hide();
            return;
        }
    }
    else if ("9"==instruct){//接收到线程在线
        QString upfriend=data.section(' ',1,1);
        qDebug()<<"upfriend="<<upfriend<<endl;
        changeStandardItem(upfriend,true);
    }
    else if ("a"==instruct){//接收到线程下线
        QString upfriend=data.section(' ',1,1);
        qDebug()<<"upfriend="<<upfriend<<endl;
        changeStandardItem(upfriend,false);
    }
    else if ("b"==instruct) {//接收到线程添加好友回复
        QString str=data.section(' ',1,1);
        if("haveno"==str)
        {
            QMessageBox::warning(this, "警告",
                                   "无该好友",
                         QMessageBox::Ok);
            return ;
        }
        else if ("noline"==str) {
            QMessageBox::warning(this, "警告",
                                   "对方不在线，不能添加",
                         QMessageBox::Ok);
            return ;
        }
        else if ("no"==str) {
            QMessageBox::warning(this, "警告",
                                   "对方不同意添加好友",
                         QMessageBox::Ok);
            return ;
        }
        else if ("yes"==str) {
            QString friendid=data.section(' ',3,3);
            QString friendname=data.section(' ',2,2);
            friendinfo node;
            node.id=friendid;
            node.name=friendname;
            node.isopen=false;
            node.isonline=true;
            node.icon=friendicon;
            QStandardItem * myfriend =new QStandardItem(friendicon," "+friendname+" ("+friendid+")");
            if(0==flag){
                myfriends=new QStandardItem(tr("好友"));
                model->appendRow(myfriends);
            }

            myfriends->appendRow(myfriend);
            feiendinfos << node;
            flag++;
            QMessageBox::warning(this, "提示",
                                   "对方同意添加好友",
                         QMessageBox::Ok);
        }
        changeinfo.hide();
        return ;
    }
    else if ("c"==instruct) {//接收到线程好友列表中有人更改自己的信息
        QString str=data.section(' ',1,1);
        if("name"==str){//更改用户名
            QString str2=data.section(' ',2,2);
            QList<friendinfo>::iterator ci;
            for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
                if(ci->id==str2){
                    ci->name=data.section(' ',3,3);
                    break;
                }
            }
            for (int i=0;i<=myfriends->row();i++) {
                QStandardItem* childItem =myfriends->child(i,0);
                QString data1=childItem->text();
                if(data1.contains(str2,Qt::CaseSensitive))
                {
                    myfriends->child(i,0)->setText(" "+data.section(' ',3,3)+" ("+str2+")");
                    ui->treeView->update();
                    break;
                }
            }
        }
        else if ("icon"==str) {//更改头像
            QString str2=data.section(' ',2,2);
            QList<friendinfo>::iterator ci;
            for(ci=feiendinfos.begin(); ci!=feiendinfos.end(); ++ci){
                if(ci->id==str2){
                    ci->icon=friendicon;
                    break;
                }
            }
            for (int i=0;i<=myfriends->row();i++) {
                QStandardItem* childItem =myfriends->child(i,0);
                QString data1=childItem->text();
                if(data1.contains(str2,Qt::CaseSensitive))
                {
                    myfriends->child(i,0)->setIcon(friendicon);
                    ui->treeView->update();
                    break;
                }
            }
        }
    }

}

void MyInterface::on_treeView_doubleClicked(const QModelIndex &index)
{

    QString mydata=index.sibling(index.row(),0).data().toString();
    qDebug()<<"mydata="<<mydata<<endl;
    if(mydata.contains(id,Qt::CaseSensitive)){
        changeinfo.show();
        return;
    }

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

//好友在线信息更改是否更新好友表
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
                    if(ci->id==upfriend && ci->isopen){
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

void MyInterface::on_pushButton_clicked()
{
    changeinfo.show();
}

//接收mychangeinfo的信号
void MyInterface::change(QString str)
{
    qDebug()<<"change"<<str<<endl;
    QString instruct=str.section(' ',0,0);
    if("7"==instruct)//添加好友
    {
        QString str1=str.section(' ',1,1);
        if(str1==id)
        {
            QMessageBox::warning(this, "警告","不能添加自己为好友",
                                 QMessageBox::Ok);
            return;
        }
        str+=" ";
        str+=id;
        qDebug()<<"addfriend:"<<str<<endl;
        QByteArray arr;
        arr.append(str);
        s->write(arr,1024);
    }
    else if ("8"==instruct) {
        str+=" ";
        str+=id;
        qDebug()<<"deletefriend:"<<str<<endl;
        QByteArray arr;
        arr.append(str);
        s->write(arr,1024);
    }
    else if ("4"==instruct) {
        QString str1="4 ";
        str1+=id;
        str1+=" ";
        str1+=str.section(' ',1,1);
        qDebug()<<"update name:"<<str1<<endl;
        QByteArray arr;
        arr.append(str1);
        s->write(arr,1024);
    }
    else if ("5"==instruct) {
        QString file=str.section(' ',1,1);
        QFile f(file);
        if(!f.open(QIODevice::ReadWrite|QIODevice::Text))
        {
            QMessageBox::warning(this, "警告","文件路径错误",
                                 QMessageBox::Ok);
            changeinfo.show();
            return;
        }
        f.close();

        changeIcon.load(file);
        emit signThread(changeIcon,id);
    }
}
