#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QPixmap>

class MyThread: public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QTcpSocket *,QObject *parent = nullptr);
    ~MyThread();
signals:
    void signrecvdata(QString,QPixmap);
protected slots:
    void serversend();
    void sendicon(QPixmap,QString);
private:
    QTcpSocket *s;
};

#endif // MYTHREAD_H
