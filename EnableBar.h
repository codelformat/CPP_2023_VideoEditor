#ifndef ENABLEBAR_H
#define ENABLEBAR_H
#include<QtCore/QObject>
class EnalbeBar:public QObject{
    Q_OBJECT


private:
    bool enable=true;
public:
    void setEnable(bool enable);
    bool getEnable();

};

#endif // ENABLEBAR_H
