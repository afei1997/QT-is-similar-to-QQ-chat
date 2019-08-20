#ifndef MYINTERFACE_H
#define MYINTERFACE_H

#include <QWidget>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QStandardItemModel>
#include <QStandardItem>
#include "mychat.h"
#include <QCloseEvent>

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
    QList<friendinfo>feiendinfos;
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void changeStandardItem(QString,bool);
protected slots:
    void serversend();
signals:
    void signalRecvData(QString);
private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);
};

#endif // MYINTERFACE_H
