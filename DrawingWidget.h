#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include<QtWidgets>

class DrawingWidget: public QWidget{
    Q_OBJECT

public:
    DrawingWidget(QWidget* parent=nullptr):QWidget(parent),isDrawing{false}{ }
protected:
    QRect clipRect;
    void mousePressEvent(QMouseEvent *event)override;
    void mouseMoveEvent(QMouseEvent* event)override;
    void mouseReleaseEvent(QMouseEvent *event)override;
    void paintEvent(QPaintEvent *event)override;
private:
    bool isDrawing=false;
    QPoint startPos;
    QPoint endPos;
signals:
    void clipSignal(double xRatio,double yRatio,double widthRatio,double heightRatio);
};

#endif // DRAWINGWIDGET_H
