#ifndef MYCHANGEINFO_H
#define MYCHANGEINFO_H

#include <QWidget>

namespace Ui {
class MyChangeInfo;
}

class MyChangeInfo : public QWidget
{
    Q_OBJECT

public:
    explicit MyChangeInfo(QWidget *parent = nullptr);
    ~MyChangeInfo();

private slots:
    void on_addBtn_clicked();

    void on_deleteBtn_clicked();

    void on_nameBtn_clicked();

    void on_imageBtn_clicked();

    void on_iconBtn_clicked();

private:
    Ui::MyChangeInfo *ui;
signals:
    void signalChange(QString);
};

#endif // MYCHANGEINFO_H
