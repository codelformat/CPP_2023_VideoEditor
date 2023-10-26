#ifndef BASICDIALOG_H
#define BASICDIALOG_H
#include <QtWidgets/QDialog>
#include<QtWidgets/QPushButton>
#include<QtWidgets/QSpinBox>
#include<QtWidgets/QLayout>
#include<QtWidgets/QHBoxLayout>
#include<QtWidgets/QVBoxLayout>
#include<QtWidgets/QFormLayout>
#include<QtWidgets/QLabel>
class BasicDialog :public QDialog{
    Q_OBJECT

public:
    BasicDialog(QSpinBox* receiver1,QSpinBox* receiver2,QWidget* parent=nullptr);
    QSpinBox* spinBoxWidth;
    QSpinBox* spinBoxHeight;
    QSpinBox* receiver1;
    QSpinBox* receiver2;
    QPushButton* btn_size_ok;
    QPushButton* btn_size_cancel;
    //QDoubleSpinBox* spinBoxVertical;
    QFormLayout* form;
    QLabel* label1;
    QLabel* label2;
    QVBoxLayout* layout;
    QHBoxLayout* buttons;
    ~BasicDialog();
signals:
    void message(double ,double);
public slots:
    void sendMessageAndClose();

};

#endif // BASICDIALOG_H
