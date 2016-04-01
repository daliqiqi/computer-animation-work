#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include<QMouseEvent>
#include<QPainter>
#include <QPoint>
#include <QWidget>
#include<QPixmap>
#include<QDebug>
#include<QPropertyAnimation>
#include<Qimage>
#include<QDialog>
#include <QPushButton>
#include<QInputDialog>
#include<QDir>
#include<Qtimer>
#include<QMessageBox>
#include <QRect>
#include <QImage>


namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



private:
    Ui::MainWindow *ui;
};


class  CSpline {
    // Construction
public:
    void CSplineInit(int x[], int y[], int n, int grain, float tension);


public:


    int dividCount;
    QPoint dividPoint[1024];
  //  QPoint Spline[1024];// budong
    QPoint s[1024];
    int allCount;   //allCount 在何处赋值？

    void CubicSpline(int n, QPoint* knots, int grain, float tension);  // attention:CPt*knots  change into QPoint *knots)
    void clearSpline();
    void Divid(float length);
    int SearchLength(float length);
    float Length();
   //virtual ~CSpline();
    // Generated message map functions

    float lengthList[500];
private:

    QPoint *knots0;

    int n0;

    double m[16];
    float Matrix(float a, float b, float c, float d, float u);
    void GetCardinalMatrix(float a1); //why should it be private
};

class PaintedWidget:public QWidget{
       //QPainter?
private:
    int count;
    QPoint pp[50];      //?
    QPixmap carimg;
    QLabel* car;   //?????????????
    double duration;
    int x[100];
    int y[100];
    int animationPos;
    bool animatStart ;
    bool uniform;


public:
    CSpline spline;//在此处申明对象
    explicit PaintedWidget(QWidget *parent = 0);//QWidget *parentchushihua  there must be some initiallize problems
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event) ;
    void clear();// 函数未编写
    void animation();
    void animation2();
    void animationControl();
    void sliderupdate(){update();}
    void motion_choose1();
    void motion_choose0();
    void clickupdate(){update();}//未定义 要内联
   // void sliderupdate(int t){this->setValue(t);}
    void  setDuration ( int h ){ duration=h;}//数字瞎编
    //~PaintedWidget();
//friend class CSpline;
    void animationStart();
    float Widget_Length();
    void DividSpline(float length);
    int  SearchLengthPosition(float length);
};

class Set:public QObject {

public:


    void update(){update();}
    void setDurationValue(int duration);

};
//void update(){Pupdate();}

//end of head file
#endif



