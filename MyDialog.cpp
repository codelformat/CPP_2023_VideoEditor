#include<MyDialog.h>
#include<QLayout>
#include<QFormLayout>
MyDialog::MyDialog(QWidget *parent)
    :QDialog(parent)
{

}
void MyDialog::do_btn_size_ok_clicked(){
    this->accept();
}
void MyDialog::do_btn_size_cancel_clicked(){
    this->close();
}

