#ifndef MYINTERFACE_H
#define MYINTERFACE_H

#include <QWidget>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QStandardItemModel>
#include <QStandardItem>
#include "mychat.h"
#include <QCloseEvent>
#include "mychangeinfo.h"
#include "mythread.h"

namespace Ui {
class MyInterface;
}

class MyInterface : public QWidget
{
    Q_OBJECT

public:
    explicit MyInterface(QTcpSocket *,QString,QWidget *parent = nullptr);
    ~MyInterface();
    void closeEvent (QCloseEvent * e );
    struct friendinfo{
        QString id;
        QString name;
        QPixmap icon;
        bool isopen;
        bool isonline;
        MyChat *confriend;
    };

private:
    Ui::MyInterface *ui;
    QPoint last;
    QTcpSocket *s;
    QString id;
    QPixmap myicon;
    QStandardItemModel* model;
    QStandardItem * item;
    QStandardItem *myfriends;
    int flag;
    QList<friendinfo>feiendinfos;
    MyChangeInfo changeinfo;
    QPixmap changeIcon;
    QString tempid;
    QString tempname;
    QPixmap tempicon;
    MyThread *thr;
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void changeStandardItem(QString,bool);
protected slots:
    void serversend(QString,QPixmap);
    void change(QString);
signals:
    void signalRecvData(QString);
    void signThread(QPixmap,QString);
private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_pushButton_clicked();
};

#endif // MYINTERFACE_H
