#ifndef MYCHAT_H
#define MYCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QCloseEvent>
namespace Ui {
class MyChat;
}

class MyChat : public QWidget
{
    Q_OBJECT

public:
    explicit MyChat(QString,QString,QPixmap,QTcpSocket *,QString one=nullptr,QWidget *parent = nullptr);
    ~MyChat();
    void closeEvent (QCloseEvent * e );
private:
    Ui::MyChat *ui;
    QString friendid;
    QString sendHead;
    QPixmap windowIcon;
    QTcpSocket *s;
protected slots:
    void recvData(QString);
private slots:
    void on_sendBtn_clicked();
};

#endif // MYCHAT_H
