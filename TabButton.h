#ifndef TABBUTTON_H
#define TABBUTTON_H
#include<QPushButton>
#include<QIcon>
#include<QEvent>
class TabButton:public QPushButton{
    Q_OBJECT

    Q_PROPERTY(int index READ index CONSTANT FINAL)

private:
    static int count;
    const int m_index;
public:
    QIcon img[3];
    QSize iconSize=QSize(30,30);

protected:

    bool event(QEvent *e)override;
public:
    TabButton(QWidget* parent=nullptr);
    ~TabButton();
    static bool isStackFold;
    static int curIndex;
    static const int fixSize;
    int index()const;
    /*inline */void resetIcon(){setIcon(img[0]);};//?无法解析？

signals:
    void signal_click(int index);
};

#endif // TABBUTTON_H
