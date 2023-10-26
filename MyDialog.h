
#ifndef MYDIALOG_H
#define MYDIALOG_H

#include<QtWidgets/QDialog>
#include<QtWidgets/QSpinBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include<QtWidgets/QPushButton>
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
