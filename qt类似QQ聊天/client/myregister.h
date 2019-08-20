#ifndef MYREGISTER_H
#define MYREGISTER_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class MyRegister;
}

class MyRegister : public QWidget
{
    Q_OBJECT

public:
    explicit MyRegister(QWidget *parent = nullptr);
    ~MyRegister();

private slots:
    void on_registerBtn_clicked();

private:
    Ui::MyRegister *ui;
    QTcpSocket *s;

signals:
    void mySignal(bool);
};

#endif // MYREGISTER_H
