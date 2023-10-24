
#ifndef MYDIALOG_H
#define MYDIALOG_H

#include<QtWidgets/QDialog>
#include<QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include<QPushButton>
class MyDialog: public QDialog{
      Q_OBJECT
public:
    MyDialog(QWidget* parent=nullptr);
    QSpinBox*spinBoxWidth;
    QSpinBox*spinBoxHeight;
    QPushButton*btn_size_ok;
    QPushButton*btn_size_cancel;
    QDoubleSpinBox*spinBoxVertical;
private slots:
    void do_btn_size_ok_clicked();
    void do_btn_size_cancel_clicked();
 };

#endif // MYDIALOG_H
