#ifndef MYCHANGEIP_H
#define MYCHANGEIP_H

#include <QWidget>

namespace Ui {
class MyChangeIp;
}

class MyChangeIp : public QWidget
{
    Q_OBJECT

public:
    explicit MyChangeIp(QWidget *parent = nullptr);
    ~MyChangeIp();

private slots:
    void on_changeIpBt_clicked();

private:
    Ui::MyChangeIp *ui;
signals:
    void signalChange();

};

#endif // MYCHANGEIP_H
