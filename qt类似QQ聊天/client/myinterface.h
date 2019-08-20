#ifndef MYINTERFACE_H
#define MYINTERFACE_H

#include <QWidget>

namespace Ui {
class MyInterface;
}

class MyInterface : public QWidget
{
    Q_OBJECT

public:
    explicit MyInterface(QWidget *parent = nullptr);
    ~MyInterface();

private:
    Ui::MyInterface *ui;
};

#endif // MYINTERFACE_H
