#include"BasicDialog.h"
BasicDialog::BasicDialog(QSpinBox* receiver1,QSpinBox* receiver2,QWidget* parent):QDialog(parent){
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFixedHeight(200);
    this->setFixedWidth(300);
    this->receiver1=receiver1;
    this->receiver2=receiver2;
    layout=new QVBoxLayout(this);
    this->spinBoxWidth = new QSpinBox(this);
    this->spinBoxHeight = new QSpinBox(this);
    label1=new QLabel("宽度：");
    label2=new QLabel("高度：");
    form=new QFormLayout();
    form->addRow(label1,this->spinBoxWidth);
    form->addRow(label2,this->spinBoxHeight);
    layout->addLayout(form);
    this->spinBoxWidth->setMaximum(500);
    this->spinBoxWidth->setMinimum(0);
    this->spinBoxHeight->setMaximum(500);
    this->spinBoxHeight->setMinimum(0);
    this->btn_size_ok= new QPushButton("OK",this);
    this->spinBoxWidth->setValue(receiver1->value());
    this->spinBoxHeight->setValue(receiver2->value());

    this->btn_size_cancel= new QPushButton("Cancel", this);
    buttons=new QHBoxLayout();
    buttons->addWidget(this->btn_size_ok);
    buttons->addWidget(this->btn_size_cancel);
    layout->addLayout(buttons);
    //connect(btn_size_ok,SIGNAL(clicked()),this,SIGNAL(message(double,double)));
    connect(btn_size_ok,SIGNAL(clicked()),this,SLOT(sendMessageAndClose()));
    connect(btn_size_cancel,SIGNAL(clicked()),this,SLOT(close()));

}
void BasicDialog::sendMessageAndClose(){
    receiver1->setValue(spinBoxWidth->value());
    receiver2->setValue(spinBoxHeight->value());
    this->close();
}
BasicDialog::~BasicDialog(){
    delete spinBoxWidth;
    delete spinBoxHeight;
    delete btn_size_cancel;
    delete btn_size_ok;
    delete buttons;
    delete label1;
    delete label2;
    delete form;
    delete layout;
}
