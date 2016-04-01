#include "mainwindow.h"


 int grainValue = 20;
 float tensionValue = 0.5;
 int durationValue = 500;
 QTimer *timer;
 bool uniform=true;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)

{
    //paintArea = new PaintedWidget(parent);
    resize(800,600);
}

MainWindow::~MainWindow()
{

}

PaintedWidget::PaintedWidget(QWidget *parent):QWidget(parent)
{
    resize(800,600);
    setWindowTitle(tr("Paint Demo"));
    this->count = 0;
    this->setMouseTracking(true);
    for(int j=0;j<100;j++)
    {
        x[j] = y[j] = 0;
    }


    QString path=QDir::currentPath(); //获取程序当前目录
    //path.replace("/","\\"); //将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    qDebug()<<path+"/car.png";
    QPixmap pixmap;//(path+"/car.png");//path 将图片成功引入
    pixmap.load(":/images/car.png");
    carimg = pixmap;
    car = new QLabel(this);
    car->setMaximumWidth(100);
        /*自适应大小,可有可无*/

    // car->setGeometry(QRect R);
        /*自动换行*/
    duration = 2000;

    spline.dividCount=0;//部分初始化
    animationPos = 0;
    animatStart = false;

}

void PaintedWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);//在屏幕上绘制曲线

    /*for(int j=0;j<count-1;j++)
    {
        if(count>1)
        {
            painter.drawLine(x[j+1],y[j+1],x[j],y[j]);
            //qDebug()<<pp[j+1]<<"in paint event:"<<pp[j];

    }*/
    //spline 对象中用每个插值后的小点来代表曲线
    if(count > 1){
        spline.CSplineInit(x,y,count,grainValue,tensionValue);
        //qDebug()<<tensionValue <<"tensiongValue!!";
        for(int j=0;j<spline.allCount-1;j++)
        {
            /*QPen greenpen;
            greenpen.setColor(Qt::black);
            painter.setPen(greenpen);*/
            painter.drawLine(spline.s[j],spline.s[j+1]);
            //qDebug()<< spline.s[j+1]<<"insert"<<spline.s[j];
        }
    }
    painter.setBrush(Qt::blue);//标示出每一个绘制点
    for(int j=0;j<count;j++)
        painter.drawEllipse(pp[j],3,3);

    if(spline.dividCount>2){
        for(int j=0;j<spline.dividCount;j++)
        {
            painter.setBrush(Qt::red);
            painter.drawEllipse(spline.dividPoint[j],3,3);//标示出每一个分割点
            //qDebug()<<"divid point painting"<<j<<spline.dividPoint[j];
        }
    }
    //qDebug()<<"allcount:"<<spline.allCount;
}

void PaintedWidget::mousePressEvent(QMouseEvent *event)
{
    setCursor(Qt::PointingHandCursor);

    QPoint current = event->pos();

    pp[count] = event->pos();//将pp点赋值为当前点的坐标，并用数组x,y 记录坐标位置
    x[count] = pp[count].x();
    y[count] = pp[count].y();

    count++;


    //每次添加点后计算length,更新lengthList
   // Widget_Length();
    update();
}

void PaintedWidget::clear()//曲线点清零
{
    count = 0;
    for(int i=0;i<100;i++)
    {
        x[i] = y[i] = 0;
        pp[i].setX(0);
        pp[i].setY(0);
    }
    spline.clearSpline();
    qDebug()<<pp[20]<<"arbitrary is zero"<<pp[21];
    car->hide();//汽车隐藏
    //update();
    animationPos = 0;
    animatStart = false;
}



void PaintedWidget::animation2()
{   QImage* img=new QImage;
    QImage*imgScaled=new QImage;
    img->load(":/images/car.png");
    *imgScaled=img->scaled(30,30,
                           Qt::KeepAspectRatio);

    car->setPixmap(QPixmap::fromImage(*imgScaled));
    car->setFixedSize(30,30);
    car->show();
    QPropertyAnimation *anim = new QPropertyAnimation(car,"pos");


    anim->setDuration(durationValue);

    if(!uniform){  //非匀速
        if(animationPos <= spline.dividCount/2){
            durationValue -= 30;
            if (durationValue<30)//设置不小于30的间隔距离
                durationValue = 30;
        }
        else
            durationValue += 30;
        timer->start(100);
    }

    QPoint temp1 = spline.dividPoint[animationPos];
    QPoint temp2 = spline.dividPoint[animationPos+1];
    anim->setStartValue(temp1);
    anim->setEndValue(temp2);


    qDebug()<<"start from:"<<temp1<<"to"<<temp2;

    anim->start();
}

void PaintedWidget::animationControl()
{
    if(animatStart){
        animation2();
        animationPos++;
        if(animationPos>spline.dividCount-2){
            animationPos = 0;
            animatStart = false;
        }
        qDebug()<<animationPos;
        qDebug()<<durationValue;
    }
}

float PaintedWidget::Widget_Length()
{
    update();
    return spline.Length();
}

int PaintedWidget::SearchLengthPosition(float length)//去掉
{
    int index = spline.SearchLength(length);
    qDebug()<<"position in lengthList:"<<index;
    return index;
}

void PaintedWidget::DividSpline(float length)//调用分割函数
{
    if(length<spline.lengthList[0]){
        //qDebug()<<"???????";
        if(QMessageBox::warning(this,
                                QObject::tr("Error"),
                                QObject::tr("Sorry!the length you input is smaller than the smallest unit."),
                                QMessageBox::Ok)==QMessageBox::Ok)
            length = spline.lengthList[0];
    }
    spline.Divid(length);
}

void PaintedWidget::animationStart(){
    animatStart = true;
    //animationControl();
}

void PaintedWidget::motion_choose1()
{
    uniform = true;
}

void PaintedWidget::motion_choose0()
{
    uniform = false;
}

void CSpline::CSplineInit(int x[], int y[], int n, int grain, float tension)
{
    int i,np;//初始化曲线
    np = n;
    QPoint jd[100];
    QPoint *knots;

    allCount = 0;
    lengthList[0] = 0;

    for(i=1;i<=np;i++)
    {
        jd[i].setX(x[i-1]);
        jd[i].setY(y[i-1]);
    }
    jd[0].setX(x[0]);
    jd[0].setY(y[0]);
    jd[np+1].setX(x[np-1]);
    jd[np+1].setY(y[np-1]);
    np=np+2;
//    for(i=0;i<=np;i++)
//    {
//        jd[i].setX(x[i-1]);
//        jd[i].setY(y[i-1]);
//    }

    knots = jd;
    CubicSpline(np,knots,grain,tension);
}

void CSpline::CubicSpline(int n, QPoint* knots, int grain, float tension)//计算cardinal 差值后的曲线
{
    QPoint  *k0, *kml, *k1, *k2;
    int i,j;
    float u[80];  //u的数组长度要和grain的范围对应
    GetCardinalMatrix(tension);
    for(i=0; i<=grain; i++)
        u[i]=((float)i)/grain;
    kml = knots;
    k0 = kml+1;
    k1 = k0+1;
    k2 = k1+1;
    for(i=1;i<n-2;i++)
    {
        for(j=0;j<=grain;j++)
        {
            s[allCount].setX(Matrix(kml->x(),k0->x(),k1->x(),k2->x(),u[j]));
            s[allCount].setY(Matrix(kml->y(),k0->y(),k1->y(),k2->y(),u[j]));
            //qDebug()<<s[allCount]<<" Cubic inserting";
            allCount++;
        }
        k0++;kml++;k1++;k2++;
    }
}

void CSpline::GetCardinalMatrix(float a1)//或得chadinal 曲线的差值矩阵
{
    m[0]=-a1;  m[1]=2-a1; m[2]=a1-2;   m[3]=a1;
    m[4]=2*a1; m[5]=a1-3; m[6]=3-2*a1; m[7]=-a1;
    m[8]=-a1;  m[9]=0;    m[10]=a1;    m[11]=0;
    m[12]=0;   m[13]=1;   m[14]=0;     m[15]=0;
}

float CSpline::Matrix(float a, float b, float c, float d, float u)//三次样条曲线差值计算
{
    float p0,p1,p2,p3;
    p0=m[0]*a + m[1]*b + m[2]*c + m[3]*d;
    p1=m[4]*a + m[5]*b + m[6]*c + m[7]*d;
    p2=m[8]*a + m[9]*b + m[10]*c + m[11]*d;
    p3=m[12]*a + m[13]*b + m[14]*c + m[15]*d;
    return(p3+u*(p2+u*(p1+u*p0)));
}

void CSpline::clearSpline()//清空矩阵
{
    allCount = 0;
    for(int i=0;i<300;i++)
    {
        s[i].setX(0);
        s[i].setY(0);
    }
    for(int i=0;i<dividCount;i++)
    {
        dividPoint[i].setX(0);
        dividPoint[i].setY(0);
    }
    dividCount = 0;
}

float CSpline::Length()//创建长度表
{
    float length=0;
    for(int i=0;i<allCount-1;i++)
    {
        //qDebug()<<i<<":"<<s[i]<<" to "<<i+1<<s[i+1];
        float segment = 0;
        segment = pow((s[i+1].x()-s[i].x())*1.0,2)+pow((s[i+1].y()-s[i].y())*1.0,2);
        segment = sqrt(segment);  // 最小化至两个grain之间的线段长度
        length += segment;//用之叠加代表曲线长度
        lengthList[i] = length;//

    }
    qDebug()<<"total length:"<<length;
    if( allCount > 2 ){
        qDebug()<<"lengthList[allcount-2]:"<<lengthList[allCount-2];
        for(int i=0;i<allCount-1;i++)
            qDebug()<<"liengthlist["<<i<<"]:"<<lengthList[i];
    }
    return length;
}

int CSpline::SearchLength(float length)
{
    int a = 0;
    int b = (allCount-1);
    int mid = (a+b)/2;
    //qDebug()<<"allCount:"<<allCount;
    qDebug()<<"want to search:"<<length;
    if(allCount<=2){
        //qDebug()<<"allCount<=2";
        return -1;
    }
    if(length<lengthList[0] || length>lengthList[allCount-2]){
        //qDebug()<<"not in 0-allcount-2";
        return -1;
    }

    //二分法查找，对于指定的弧长 从弧长表中寻找对应位置
    while(length<lengthList[mid] || length>=lengthList[mid+1])
    {
        if(length>=lengthList[mid+1])
        {
            a = mid+1;
            mid = (a+b)/2;
        }
        else{
            b = mid;
            mid = (a+b)/2;
        }
        qDebug()<<"a:"<<a<<" b:"<<b<<" mid:"<<mid;

    }




    return mid;
}

void CSpline::Divid(float length)
{
    int mid = 0;
    int i=0;
    while(mid!=-1)
    {
        dividPoint[i]=s[mid];//将曲线按照二分法分割成许多小段
        i++;
        mid = SearchLength(length*i);//将曲线按弧长倍增的方式分成许多小段
        //qDebug()<<"find "<<length*i<<" in "<<mid;
    }
    dividPoint[i] = s[allCount-1];
    i++;
    dividCount = i;
   //test print
    for(int j=0;j<dividCount;j++)
    {
       qDebug()<<"dividPoint["<<j<<"]:"<<dividPoint[j];
    }
}





void Set::setDurationValue(int speed)
{
    durationValue = -50*speed + 1000;
    //durationValue = 1075;
    //timer->start(durationValue);
}


