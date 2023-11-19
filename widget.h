#ifndef WIDGET_H
#define WIDGET_H
#define TOTAL_TIME 210.0    //此处为LAN接口测试的倒计时参数
#define INTERVAL 0.1

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <synchapi.h>
#include <QMessageBox>
#include <QByteArray>
#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QThread>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void SetPortNumCmb();       // 设置串口号
    QStringList GetEnableCommPortQt();      // 获取计算机可用串口 Qt方式
    bool SetSerialPortParam(QSerialPort *serial,int Baudrate);   // 设置串口参数，失败返回false，成功返回true
    void judge();    //实现最终判断
    void createXML(int test_array[]);   //生成测试报告

private slots:
    void on_PBT_TEST_COM_CN8_115200_clicked(); //按下开始测试即可发送命令，执行115200串口测试操作

    void slot_RecvPortData();   //接收串口数据

    void on_PBT_SEARCH_clicked();   //寻找可用串口以供用户选择

    void on_PBT_RES_clicked();  //生成结果按钮

    void on_PBT_XML_clicked();  //生成XML文件按钮

    void on_PBT_TEST_COM_CN8_38400_clicked();   //执行38400串口测试操作

    void on_PBT_TEST_COM_CN8_9600_clicked();    //执行9600串口测试操作

    void on_PBT_TEST_TIME_clicked();    //执行读取板卡时间操作

    void on_PBT_REAL_TIME_clicked();    //执行显示板卡时间操作

    void on_PBT_LAN_NTE_clicked();  //执行测试LAN网络设置操作

    void count_time();  //执行计时

private:
    Ui::Widget *ui;

    QSerialPort *m_serial;  //实例化串口对象
    qint64 m_nReadBuffSize; //串口缓冲区大小
    bool m_bOpen;   //标识串口状态
    QByteArray Test_array_SWITCH1 = ("NO TEST");   //存放转换文件夹指令的返回值
    QByteArray Test_array_SEND1 = ("NO TEST");    //存放发送115200指令的返回值
    QByteArray Test_array_SWITCH2 = ("NO TEST");  //存放转换波特率为38400的返回值
    QByteArray Test_array_SEND2 = ("NO TEST");    //存放发送38400指令的返回值
    QByteArray Test_array_SWITCH3 = ("NO TEST");  //存放38400转换返回值
    QByteArray Test_array_SEND_9600 = ("NO TEST");    //存放9600测试指令返回值
    QByteArray Test_array_SWITCH4 = ("NO TEST");  //存放串口检测完后是否转换为115200返回值
    QByteArray Wish = {"echo \"1G&\"\r\n1G&\r\n[root@localhost boa]# "};
    QByteArray MY_AUTO_TIME;    //存放系统时间
    QByteArray Test_array_LTime = ("NO TEST");    //存放板卡时间
    QByteArray Test_array_lan_connect;  //存放检查LAN指令返回值
    QByteArray Test_array_SEND_lan = ("NO TEST");   //存放网口测试返回值
    int r[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int Send_line = 0;  //表示发送的指令位置
    QTimer*      timer_countdown;   //为计时器窗口及按钮参数
    QMessageBox* msgbox;
    QPushButton* okbtn;
    double       msgbox_time_;
};

#endif // WIDGET_H
