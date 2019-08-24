#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QTcpSocket>
#include "myregister.h"
#include "mychangeip.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QPoint last;
    QTcpSocket *s;
    QString ip;
    quint16 port;
    MyChangeIp change;
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private slots:
    void on_loginButton_clicked();
    void on_pButtonRegistAccount_clicked();

    void tomainSlot(bool);
    void changeIP();
    void on_pButtonArrow_clicked();
};

#endif // WIDGET_H
