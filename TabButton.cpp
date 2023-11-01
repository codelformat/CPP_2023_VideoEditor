#include<TabButton.h>
#include<QEvent>
int TabButton::count=0;
int TabButton::curIndex=-1;
bool TabButton::isStackFold=false;
const int TabButton::fixSize=45;
TabButton::TabButton(QWidget* parent)
    :QPushButton{parent},m_index{count}{
    this->setMaximumWidth(fixSize);
    this->setMinimumWidth(fixSize);
    this->setMaximumHeight(fixSize);
    this->setMinimumHeight(fixSize);
    //this->setFlat(true);
    this->setStyleSheet("background-color:rgba(255,255,255,0)");
    ++count;
    if(m_index==0){
        img[0]=QIcon(":/Resources/file.svg");
        img[1]=QIcon(":/Resources/file_hover.svg");
        img[2]=QIcon(":/Resources/file_select.svg");
    }
    else if(m_index==1){
        img[0]=QIcon(":/Resources/cut.svg");
        img[1]=QIcon(":/Resources/cut_hover.svg");
        img[2]=QIcon(":/Resources/cut_select.svg");
    }
    else if(m_index==2){
        img[0]=QIcon(":/Resources/color.svg");
        img[1]=QIcon(":/Resources/color_hover.svg");
        img[2]=QIcon(":/Resources/color_select.svg");
    }
    else if(m_index==3){
        img[0]=QIcon(":/Resources/net.svg");
        img[1]=QIcon(":/Resources/net_hover.svg");
        img[2]=QIcon(":/Resources/net_select.svg");
    }
    else if(m_index==4){
        img[0]=QIcon(":/Resources/math.svg");
        img[1]=QIcon(":/Resources/math_hover.svg");
        img[2]=QIcon(":/Resources/math_select.svg");
    }

    setIcon(img[0]);

    setIconSize(iconSize);
}
TabButton::~TabButton(){
    //--count;如果减了，可能存在m_index相同的情况
}
int TabButton::index()const{
    return m_index;
}


bool TabButton::event(QEvent *e){
    switch (e->type()) {
    case QEvent::Enter:
        setCursor(Qt::PointingHandCursor);
        if(TabButton::curIndex==m_index){
            break;
        }
        else{
            setIcon(img[1]);
            break;
        }
        //setIcon(Img_hover);

        //emit sendIndex(m_index);
        //setIcon(img[1]);
        //break;
    case QEvent::Leave:
        if(TabButton::curIndex==m_index){
            break;
        }
        else{
            setIcon(img[0]);
            break;
        }
        //setIcon(Img);
        //setIcon(img[0]);
        //emit sendIndex(m_index);
        //break;
    //case QEvent::MouseButtonPress:
        //setIcon(Img_pressed);
        //break;
    case QEvent::MouseButtonRelease:
        //setIcon(img[2]);
        //setIcon(Img_selected);
        //mouseReleaseEvent((QMouseEvent*)(e));
        /*if(TabButton::curIndex==m_index){
            setIcon(img[0]);
        }
        else{
            setIcon(img[2]);
        }*/
        //设置Icon交给choosePage做，否则容易出现多个渲染选中。
        emit signal_click(m_index);

        break;
    default:
        break;
    }
    return QPushButton::event(e);
    //return true;//不再向上传递
}
/*
inline void TabButton::resetIcon(){
    setIcon(img[0]);
}
*/
