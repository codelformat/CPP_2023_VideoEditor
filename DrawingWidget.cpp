#include<DrawingWidget.h>

void DrawingWidget::mousePressEvent(QMouseEvent *event){
    isDrawing=true;//由按钮触发变为true
    if (isDrawing)
    {
        startPos = event->pos();
        endPos = startPos;
        //update(); // 强制重绘
    }
}
void DrawingWidget::mouseMoveEvent(QMouseEvent* event){
    if (isDrawing)
    {
        endPos = event->pos();
        update(); // 强制重绘
    }
}
void DrawingWidget::mouseReleaseEvent(QMouseEvent *event){
    if (isDrawing)
    {
        if(endPos==startPos){
            update(); // 清除矩形
            return;//不发送信号
        }
        endPos = startPos;
        update(); // 清除矩形

        //发送信号，执行裁剪函数。
        double xRatio=(double)clipRect.x()/this->width();
        double yRatio=(double)clipRect.y()/this->height();
        double widthRatio=(double)clipRect.width()/this->width();
        double heightRatio=(double)clipRect.height()/this->height();

        if((xRatio+widthRatio)<1&&(yRatio+heightRatio)<1&&(xRatio+widthRatio)>0&&(yRatio+heightRatio)>0){
            emit clipSignal(xRatio,yRatio,widthRatio,heightRatio);
        }
    }
}

void DrawingWidget::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //this->setAttribute(Qt::WA_TranslucentBackground,true);
    if (isDrawing)
    {

        if(startPos==endPos){

            return;
        }
        clipRect=QRect(startPos, endPos);
        //painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
        painter.setPen(QPen(Qt::white,1,Qt::SolidLine));
        painter.setBrush(QColor(250,250,250,100));
        painter.drawRect(clipRect);
    }
}
