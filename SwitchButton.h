#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QPainter>
#include <QRadialGradient>
#include <QMouseEvent>

///
/// \brief 基础控件-Switch开关按钮
///
class SwitchButton : public QWidget
{
    Q_OBJECT
public:
    Q_PROPERTY(QColor pBackColor MEMBER mBackColor)  //新增背景颜色属性,用于动画

    explicit SwitchButton(QWidget *parent = nullptr);
    bool getSwitch();   /// 获取开关状态

public slots:
    void setSwitch(bool onoff);    /// 设置开关状态,default:0
    void setEnabled(bool enable);    /// 设置使能状态,default:1
    bool getEnabled();    /// 获取使能状态
    void setAnimationPeriod(int period);    /// 设置切换状态周期
    void setPrecisionClick(bool flag);    /// 设置精确点击，即只有点中按钮的时候才开关
    void setWaitModel(bool flag);    /// 设置等待模式，点击后不会主动切换开关，需要setSwitch
    void setSwitchForWaitModel(bool onoff);    /// 设置开关状态,default:0

    void setButtonColor(QColor color);    /// 设置开关（圆形按钮）颜色
    void setBackOnColor(QColor color);    /// 设置背景颜色-开
    void setBackOffColor(QColor color);    /// 设置背景颜色-关
    void setEdgeColor(QColor color);    /// 设置边缘颜色，默认透明

signals:
    void sigEnableChanged(bool enable);    /// 使能状态变化信号
    void sigSwitchChanged(bool onoff);    /// 开关状态变化信号

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    bool mOnOff{0};     //开关状态
    bool mEnable{1};    //使能状态
    bool mPrecisionClickFlagh{0};   //精确点击标志位
    bool mWaitSigModel{0};  //等待模式，点击后按钮位置会进行切换，但是颜色需要等待外部信号变动
    bool mAnimationOnOff{1};    //动画开关，default：1
    bool mHover{0};

    QColor mButtonColor{Qt::white};    //开关（圆形按钮）颜色
    QColor mBackColor{Qt::red};
    QColor mEdgeColor{Qt::transparent};  //边缘颜色
    QRectF mButtonRect; //开关按钮rect
    int mRadius{8}; // 开关外观边缘圆角

    int mAnimationPeriod{200};  //动画周期
    QPointF mRightPos;       // 动画位置-开
    QPointF mLeftPos;        // 动画位置-关
    QColor mBackOnColor{165,222,228};  //背景颜色-开
    QColor mBackOffColor{Qt::darkGray};  //背景颜色-关
};


#endif // SWITCHBUTTON_H
