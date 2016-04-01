#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include<QMessageBox>

#include <QSpinBox>
#include<QRadioButton>
#include<QGroupBox>

extern int grainValue;
extern float tensionValue;
extern int durationValue;
extern QTimer *timer;//定义定时器

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    PaintedWidget view;
    //view.setMouseTracking(false);


    QPushButton clear_btn(&view);
    clear_btn.setText("Clear");
    //clear_btn.show();
    QObject::connect(&clear_btn,&QPushButton::clicked,&view,&PaintedWidget::clear);
    QObject::connect(&clear_btn,&QPushButton::clicked,&view,&PaintedWidget::clickupdate);

    QPushButton car_btn(&view);
    car_btn.setText("Start");//开始按钮
    //QObject::connect(&car_btn,&QPushButton::clicked,&view,&PaintedWidget::animation);
    QObject::connect(&car_btn,&QPushButton::clicked,&view,&PaintedWidget::animationStart);

    //计算总长的按钮
    QPushButton finish_btn(&view);
    finish_btn.setText("FinishPainting");
    QObject::connect(&finish_btn,&QPushButton::clicked,&view,&PaintedWidget::Widget_Length);



    Set o_set;


    // speed 和duration成反比。。。
    QLabel *labelSpeed = new QLabel("Speed");
    QSlider *sliderSpeed = new QSlider(Qt::Horizontal,&view);
    sliderSpeed->setRange(0,20);
    sliderSpeed->setValue(9);
    void (QSpinBox::*spinBoxSignal)(int) = &QSpinBox::valueChanged;
    //QObject::connect(sliderSpeed,&QSlider::valueChanged,&view,&PaintedWidget::setDuration);
    QObject::connect(sliderSpeed,&QSlider::valueChanged,&o_set,&Set::setDurationValue);


    //QObject::connect(boxGrain,spinBoxSignal,&view,&PaintedWidget::Widget_Length);
    QSpinBox *boxSpeed = new QSpinBox(&view);
    boxSpeed->setRange(0,20);
    boxSpeed->setValue(9);
    QObject::connect(boxSpeed,spinBoxSignal,sliderSpeed,&QSlider::valueChanged);
     QObject::connect(sliderSpeed,&QSlider::valueChanged,boxSpeed,&QSpinBox::setValue);
    //输入弧长的box
    QSlider*sliderLength= new QSlider(Qt::Horizontal,&view);
    sliderLength->setRange(0,300);
    sliderLength->setValue(50);
    QSpinBox *boxLength = new QSpinBox(&view);
    boxLength->setRange(0,300);
    boxLength->setValue(50);
    QObject::connect(boxLength,spinBoxSignal,sliderLength,&QSlider::valueChanged);
     QObject::connect(sliderLength,&QSlider::valueChanged,boxLength,&QSpinBox::setValue);
    QObject::connect(boxLength,spinBoxSignal,&view,&PaintedWidget::DividSpline);
    //QObject::connect(boxLength,spinBoxSignal,&view,&PaintedWidget::sliderupdate);
    QLabel *labelLength = new QLabel("lengthunit");

    //匀速 变速单选按钮
    QRadioButton *uniform = new QRadioButton(&view);
    uniform->setText("constantSpeed");
    uniform->setChecked(true);
    QRadioButton *variable = new QRadioButton(&view);
    variable->setText("freeSpeed");

    QObject::connect(uniform,&QRadioButton::toggled,&view,&PaintedWidget::motion_choose1);
    QObject::connect(variable,&QRadioButton::toggled,&view,&PaintedWidget::motion_choose0);



    QGridLayout* rightlayout = new QGridLayout;
    rightlayout->addWidget(uniform,1,0);
    rightlayout->addWidget(variable,1,1);

    // rightlayout->addWidget(car,1,2);


    rightlayout->addWidget(labelSpeed,1,3);
    rightlayout->addWidget(sliderSpeed,1,4);
    rightlayout->addWidget(boxSpeed,1,5);
    rightlayout->addWidget(&car_btn,3,4);
    rightlayout->addWidget(&finish_btn,3,0);
    rightlayout->addWidget(&clear_btn,3,5);
    rightlayout->addWidget(labelLength,3,1);
    rightlayout->addWidget(boxLength,3,3);
    rightlayout->addWidget(sliderLength,3,2);//单位弧长输入
    //rightlayout->addWidget(buttonGroup,5,0,2,5);

    //w.setLayout(rightlayout);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addWidget(&view);
    mainlayout->addLayout(rightlayout);
    mainlayout->setStretchFactor(&view,1);
    mainlayout->setStretchFactor(rightlayout,0);
    w.setLayout(mainlayout);
//    QHBoxLayout* mainlayout = new QHBoxLayout(&view);
//    mainlayout->addWidget();

    //test

    timer = new QTimer(&view);
    QObject::connect(timer,&QTimer::timeout,&view,&PaintedWidget::animationControl);
   timer->start(100);



   // view.show();
    w.show();
    return a.exec();
}
