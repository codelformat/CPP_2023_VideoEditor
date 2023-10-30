#include "SwitchButton.h"
//#include <QDebug>

SwitchButton::SwitchButton(QWidget *parent)
    : QWidget{parent}
{
    mEnable=true;
}

bool SwitchButton::getSwitch(){
    return mOnOff;
}
bool SwitchButton::getEnabled(){
    return mEnable;
}
void SwitchButton::setSwitch(bool onoff){
    if(mWaitSigModel) return ;
    /// 状态切换
    mOnOff = onoff;
    /// 发送信号
    sigSwitchChanged(mOnOff);
    /// 动画-背景颜色
    QPropertyAnimation * colorAnimation = new QPropertyAnimation(this,"pBackColor");
    colorAnimation->setDuration(mAnimationPeriod);
    colorAnimation->setStartValue(mBackColor);
    colorAnimation->setEndValue(mOnOff ?  mBackOnColor: mBackOffColor);
    colorAnimation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);   //停止后删除
    /// 动画-开关按钮位置
    QVariantAnimation* posAnimation = new QVariantAnimation(this);
    posAnimation->setDuration(mAnimationPeriod);
    posAnimation->setStartValue(mButtonRect.topLeft());
    posAnimation->setEndValue(mOnOff ?   mRightPos :mLeftPos);
    connect(posAnimation,&QPropertyAnimation::valueChanged,[=](const QVariant &value){
        mButtonRect.moveTo(value.toPointF());
        update();
    });
    posAnimation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);   //停止后删除
}

void SwitchButton::setSwitchForWaitModel(bool onoff)
{
    if(!mWaitSigModel) return ;
    if(mOnOff == onoff){
        /// 表示值未改变先运行按钮位置动画
        QVariantAnimation* posAnimation = new QVariantAnimation(this);
        posAnimation->setDuration(mAnimationPeriod);
        posAnimation->setStartValue(mOnOff ? mLeftPos : mRightPos);
        posAnimation->setEndValue(mOnOff ?  mRightPos : mLeftPos);
        connect(posAnimation,&QVariantAnimation::valueChanged,[=](const QVariant &value){
            mButtonRect.moveTo(value.toPointF());
            update();
        });
        posAnimation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);   //停止后删除
        return ;
    }
    /// 状态切换
    mOnOff = onoff;
    /// 发送信号
    sigSwitchChanged(mOnOff);
    /// 后运行背景颜色动画
    QPropertyAnimation * colorAnimation = new QPropertyAnimation(this,"pBackColor");
    colorAnimation->setDuration(mAnimationPeriod);
    colorAnimation->setStartValue(mBackColor);
    colorAnimation->setEndValue(mOnOff ?  mBackOnColor: mBackOffColor);
    colorAnimation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);   //停止后删除
    connect(colorAnimation,&QPropertyAnimation::valueChanged,[=](const QVariant &value){
        update();
    });
}

void SwitchButton::setEnabled(bool enable){
    QWidget::setEnabled(enable);
    mEnable = enable;
    emit sigEnableChanged(mEnable);
    update();
}

void SwitchButton::setAnimationPeriod(int period){
    mAnimationPeriod = period;
}

void SwitchButton::setPrecisionClick(bool flag){
    mPrecisionClickFlagh = flag;
}

void SwitchButton::setWaitModel(bool flag)
{
    mWaitSigModel = flag;
}

void SwitchButton::setButtonColor(QColor color){
    mButtonColor = color;
    update();
}

void SwitchButton::setBackOnColor(QColor color){
    mBackOnColor = color;
    update();
}

void SwitchButton::setBackOffColor(QColor color){
    mBackOffColor = color;
    update();
}

void SwitchButton::setEdgeColor(QColor color){
    mEdgeColor = color;
    update();
}

void SwitchButton::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);

    /// 绘制边缘颜色
    QPainterPath path;
    path.addRect(this->rect());
    path.addRoundedRect(this->rect(),mRadius,mRadius);
    path.setFillRule(Qt::OddEvenFill);
    painter.setBrush(mEdgeColor);
    painter.drawPath(path);

    /// 绘制背景颜色
    painter.setBrush(mBackColor);
    painter.drawRoundedRect(this->rect(),mRadius,mRadius);

    /// 绘制圆形按钮
    painter.setBrush(mButtonColor);
    painter.drawEllipse(mButtonRect);

    /// 绘制按钮阴影
    painter.setBrush(Qt::NoBrush);
    QColor color(Qt::black);
    int count = (this->height() - mButtonRect.height())/2;
    float stepColor = (0.15-0.0)/count;
    for (int i = mButtonRect.height()/2 + 1; i < this->height()/2; i++){
        color.setAlphaF(0.15 - stepColor*(i - mButtonRect.height()/2));
        painter.setPen(color);
        painter.drawEllipse(mButtonRect.center(),i,i);
    }

    /// 失能显示，添加一层蒙层
    if(!mEnable){
        QColor disable(Qt::black);
        disable.setAlphaF(0.5);
        painter.setBrush(disable);
        painter.drawRoundedRect(this->rect(),mRadius,mRadius);
    }
}

void SwitchButton::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event)
    /// 更新按钮大小、圆角大小、动画两个位置
    int size = qMin(this->width(),this->height());
    mRadius = size/2;
    float width = size * 3 / 4;
    float border = (size - width) / 2;
    mLeftPos = QPoint(border,border);
    mRightPos = QPoint(this->width() - border - width,border);
    mButtonRect.setWidth(width);
    mButtonRect.setHeight(width);
    mButtonRect.moveTo(mOnOff ? mRightPos : mLeftPos);
    mBackColor = mOnOff ? mBackOnColor : mBackOffColor ;
    update();
}

void SwitchButton::mouseReleaseEvent(QMouseEvent *event){
    if(mWaitSigModel){
        /// 先运行按钮位置动画
        QVariantAnimation* posAnimation = new QVariantAnimation(this);
        posAnimation->setDuration(mAnimationPeriod);
        posAnimation->setStartValue(mOnOff ? mRightPos : mLeftPos);
        posAnimation->setEndValue(mOnOff ?  mLeftPos : mRightPos);
        connect(posAnimation,&QVariantAnimation::valueChanged,[=](const QVariant &value){
            mButtonRect.moveTo(value.toPointF());
            update();
        });
        posAnimation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);   //停止后删除
        return ;
    }
    if(!mEnable)    return ;
    if(mButtonRect.contains(event->pos()) || !mPrecisionClickFlagh){
        setSwitch(!mOnOff);
    }
    //setSwitch(!mOnOff);
}

void SwitchButton::enterEvent(QEvent *event){
    Q_UNUSED(event)
    mHover=true;
}

void SwitchButton::leaveEvent(QEvent *event){
    Q_UNUSED(event)
    mHover=false;
}
