#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("MyAuto");
    m_serial = new QSerialPort;
    connect(m_serial,SIGNAL(readyRead()),this,SLOT(slot_RecvPortData()));
    ui->PBT_TEST_COM_CN8_38400->setEnabled(false);  //使其不可按，测试要一步一步来
    ui->PBT_TEST_TIME->setEnabled(false);
    ui->PBT_TEST_COM_CN8_9600->setEnabled(false);
    ui->PBT_RES->setEnabled(false); //查看结果不可选
    ui->PBT_XML->setEnabled(false); //生成XML文件不可选
    ui->PBT_REAL_TIME->setEnabled(false);   //查看板卡时间不可选
    ui->PBT_LAN_NTE->setEnabled(false); //测试网络不可选
    QMessageBox::about(this,tr("提示"),tr("要把串口线连接到板卡CN8上，也连好网线呦(＾Ｕ＾)"));
}

Widget::~Widget()
{
    delete ui;
}

// 设置串口号
void Widget::SetPortNumCmb()    //显示可用串口供用户选择
{
    ui->CBBOX_Port->clear();    //先把之前有的串口数据清空，防止本选择栏重复出现可用串口
    QStringList commPortList = GetEnableCommPortQt();
    if ( !commPortList.isEmpty() )
        ui->CBBOX_Port->addItems(commPortList);
}
//获取可用串口，显示在串口选择栏中
QStringList Widget::GetEnableCommPortQt()   //获取可用串口
{
    QStringList CommPortList;

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            CommPortList.append(serial.portName());
            serial.close(); //把获得的可用串口都先关掉
        }
    }

    return CommPortList;
}

void Widget::on_PBT_TEST_COM_CN8_115200_clicked()  //执行CN8测试115200功能
{
    Test_array_SEND1.clear();
    Test_array_SWITCH1.clear();
    ui->PBT_RES->setEnabled(true);  //让查看结果按钮可用，可查看结果了
    ui->PBT_XML->setEnabled(true);  //生成XML按钮可用，可生成XML文件了
    int baudreat = 115200;
    SetSerialPortParam(m_serial,baudreat);
    if(m_serial->isOpen())  //如果串口处于打开状态，就先关掉
    {
        m_serial->close();
        m_bOpen = false;    //此处为第一次打开串口，以后就不用再设置这么详细了
    }
    else    //现在就可以打开了
    {
    if( !m_serial->open(QIODevice::ReadWrite))  //打开失败
    {
        QMessageBox::critical(this,tr("Error"),tr("串口不存在或者被其他程序占用！"),QMessageBox::Ok);
        ui->PBT_XML->setEnabled(false);
        Test_array_SEND1 = ("NO TEST");
        Test_array_SWITCH1 = ("NO TEST");
        return;
    }
    }
    //转换文件夹命令
    Send_line=1;    //此时Send_line=1，表示此时发送的指令为转换文件夹
    QByteArray switchbyte("cd /opt/boa\n");
    m_serial->write(switchbyte);
    Sleep(100);    //睡眠1ms，保证读取数据完成，防止两次指令的数据返回紊乱
    QMessageBox::StandardButton btn1;
    btn1 = QMessageBox::question(this,"提示","文件夹转换已完成，进行读写测试？",QMessageBox::Yes|QMessageBox::No);
    if(btn1 == QMessageBox::Yes)
    {
    //写命令，发送1G&
    Send_line=2;    //此时Send_line=2，表示此时发送的指令为发送测试读写指令
    QByteArray sendbytes("echo \"1G&\"\n");
    m_serial->write(sendbytes);
    }
    //再读数据
    QMessageBox::StandardButton btn2;
    btn2 = QMessageBox::question(this,"提示","此测试项目完成，继续吗？",QMessageBox::Yes|QMessageBox::No);
    ui->TextE_RES_TIP->append("115200项目测试已完成");
    if(btn2 == QMessageBox::Yes)    //因为串口波特率本来就是115200，选择不继续不用重新设置波特率
    {
        ui->PBT_TEST_TIME->setEnabled(false);   //因为波特率已经变成38400了，所以不能进行日期测试
        ui->PBT_TEST_COM_CN8_38400->setEnabled(true);   //让38400按钮可用，可进行测试
        Send_line=3;    //此时Send_line=3,表示此时发送的指令为让板卡的波特率设置为38400
        QByteArray setbaudrate1("stty -F /dev/ttyS0 speed 38400 cs8 -parenb -cstopb\n");
        m_serial->write(setbaudrate1);
        QMessageBox::about(this,tr("提示"),tr("已可进行下一项测试"));
        m_serial->close();  //关闭串口，以便下面的函数改变测试程序里面串口的波特率
    }
    else
    {
        ui->PBT_LAN_NTE->setEnabled(true);
        ui->PBT_TEST_TIME->setEnabled(true);
    }
    //此处已经将本程序的串口关掉了，后面需要再打开才能使用
    ui->PBT_TEST_COM_CN8_115200->setEnabled(false); //已完成本测试，本按钮不可用
}

void Widget::on_PBT_TEST_COM_CN8_38400_clicked()    //执行CN8测试38400功能
{
    Test_array_SWITCH2.clear();
    Test_array_SEND2.clear();
    int baudreat = 38400;
    SetSerialPortParam(m_serial,baudreat);
    //此处串口仍然关闭
    m_serial->close();  //先关闭串口
    if( !m_serial->open(QIODevice::ReadWrite))  //打开串口
    {
        QMessageBox::critical(this,tr("Error"),tr("串口不存在或者被其他程序占用！"),QMessageBox::Ok);
        return;
    }
    //此处串口已经打开且波特率已经设置为38400
    Send_line=4;    //此时Send_line=4，表示此时发送指令为检查波特率是否为38400
    QByteArray setbaudrate2("stty -F /dev/ttyS0 speed 38400 cs8 -parenb -cstopb\n");
    m_serial->write(setbaudrate2);
    QMessageBox::about(this,tr("提示"),tr("已向板卡发送转换波特率38400命令"));
    if(Test_array_SWITCH2 == "stty -F /dev/ttyS0 speed 38400 cs8 -parenb -cstopb\r\n38400\r\n[root@localhost boa]# ")
    {
    QMessageBox::about(this,tr("提示"),tr("波特率转换38400成功"));
    Send_line=5;    //此时send_line=5,表示此时发送的指令为收发指令
    QByteArray sendbytes("echo \"G2#\"\n"); //38400波特率下发送G2#
    m_serial->write(sendbytes);
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("波特率转换38400失败"),QMessageBox::Ok);
        m_serial->close();
        return;
    }
    ui->TextE_RES_TIP->append("38400测试完成");
    QMessageBox::StandardButton bt1;
    bt1 = QMessageBox::question(this,"提示","您还要继续测试串口吗？",QMessageBox::Yes|QMessageBox::No);
    if(bt1 == QMessageBox::Yes)
    {
        ui->PBT_TEST_COM_CN8_9600->setEnabled(true);
        Send_line=6;    //此时Send_line=6，表示此时发送指令为38400波特率转换为其他波特率
        QByteArray setbaudrate_9600("stty -F /dev/ttyS0 speed 9600 cs8 -parenb -cstopb\n");
        m_serial->write(setbaudrate_9600);
        QMessageBox::about(this,tr("提示"),tr("您已可进行9600波特率测试"));
        m_serial->close();
    }
    if(bt1 == QMessageBox::No)
    {
        Send_line=6;    //此时Send_line=6，表示此时发送指令为转换回115200波特率，以便其他测试的进行
        QByteArray setbaudrate_115200("stty -F /dev/ttyS0 speed 115200 cs8 -parenb -cstopb\n");
        m_serial->write(setbaudrate_115200);
        QMessageBox::about(this,tr("提示"),tr("您现在已经可以进行其他功能的测试"));
        m_serial->close();
        ui->PBT_TEST_TIME->setEnabled(true);
    }
    ui->PBT_TEST_COM_CN8_38400->setEnabled(false);  //本项目测试完成，本按钮不可选
}

void Widget::on_PBT_TEST_COM_CN8_9600_clicked() //执行9600波特率的测试功能
{
    Test_array_SEND_9600.clear();
    Test_array_SWITCH3.clear();
    int baudrate = 9600;
    SetSerialPortParam(m_serial,baudrate);
    //此处串口仍然关闭
    m_serial->close();  //先关闭串口
    if( !m_serial->open(QIODevice::ReadWrite))  //打开串口
    {
        QMessageBox::critical(this,tr("Error"),tr("串口不存在或者被其他程序占用！"),QMessageBox::Ok);
        return;
    }
    //此处串口已经打开且波特率已经设置为9600
    Send_line=7;    //此时Send_line=7，表示此时发送指令为检查波特率是否为9600
    QByteArray setbaudrate2("stty -F /dev/ttyS0 speed 9600 cs8 -parenb -cstopb\n");
    m_serial->write(setbaudrate2);
    QMessageBox::about(this,tr("提示"),tr("已向板卡发送转换波特率命令"));
    if(Test_array_SWITCH3 == "stty -F /dev/ttyS0 speed 9600 cs8 -parenb -cstopb\r\n9600\r\n[root@localhost boa]# ")
    {
    QMessageBox::about(this,tr("提示"),tr("波特率转换9600成功"));
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("波特率转换9600失败"),QMessageBox::Ok);
        m_serial->close();
        return;
    }
    Send_line=8;    //此时Send_line=8，表示此时发送指令为检测9600波特率
    QByteArray testbyte_9600("echo \"1Am!\"\n");
    m_serial->write(testbyte_9600);
    QMessageBox::about(this,tr("提示"),tr("已发送测试指令"));
    ui->TextE_RES_TIP->append("9600测试已完成");
    Send_line=9;    //此时Send_line=9，表示此时发送指令为转换回115200波特率，以便其他测试的进行
    QByteArray setbaudrate_115200("stty -F /dev/ttyS0 speed 115200 cs8 -parenb -cstopb\n");
    m_serial->write(setbaudrate_115200);
    QMessageBox::about(this,tr("提示"),tr("您现在已经可以进行其他功能的测试"));
    m_serial->close();
    ui->PBT_TEST_TIME->setEnabled(true);    //此时波特率已经变回115200，可以进行其他测试
    ui->PBT_LAN_NTE->setEnabled(true);
    ui->PBT_TEST_COM_CN8_9600->setEnabled(false);   //本项目测试完成，本按钮不可选
}

void Widget::on_PBT_TEST_TIME_clicked() //执行日期测试功能
{
    Test_array_LTime.clear();
    ui->PBT_RES->setEnabled(true);
    ui->PBT_XML->setEnabled(true);
    Test_array_SWITCH4.clear(); //执行前要先把对应的指令返回区清空，以防之前执行过本模块，进而发生混乱
    int baudrate = 115200;
    SetSerialPortParam(m_serial,baudrate);
    //此处串口仍然关闭
    m_serial->close();  //先关闭串口
    if( !m_serial->open(QIODevice::ReadWrite))  //打开串口
    {
        QMessageBox::critical(this,tr("Error"),tr("串口不存在或者被其他程序占用！"),QMessageBox::Ok);
        ui->PBT_XML->setEnabled(false);
        Test_array_LTime = ("NO TEST");
        return;
    }
    //此处串口已经打开且波特率已经设置为115200
    Send_line=10;    //此时Send_line=10，表示此时发送指令为检查波特率是否为115200
    QByteArray setbaudrate2("stty -F /dev/ttyS0 speed 115200 cs8 -parenb -cstopb\n");
    m_serial->write(setbaudrate2);
    QMessageBox::about(this,tr("提示"),tr("已向板卡发送检测波特率命令"));
    if(Test_array_SWITCH4 == "stty -F /dev/ttyS0 speed 115200 cs8 -parenb -cstopb\r\n115200\r\n[root@localhost boa]# ")
    {
    QMessageBox::about(this,tr("提示"),tr("现在波特率正确，您可以进行本项测试"));
    ui->PBT_LAN_NTE->setEnabled(true);
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("波特率转换115200失败，本测试终止"),QMessageBox::Ok);
        m_serial->close();
        return;
    }
    ui->PBT_TEST_COM_CN8_9600->setEnabled(false);
    ui->PBT_TEST_COM_CN8_38400->setEnabled(false);  //由于波特率已经变成了115200，所以这两个测试已经不能进行了
    Send_line=11;   //此时Send_line=11，表示此时发送指令为获取系统时间
    QByteArray get3A4000time("date +%Y-%m-%d-%H-%M-%S\n");
    m_serial->write(get3A4000time);
    QMessageBox::about(this,tr("提示"),tr("已获取系统时间"));
    QDateTime GETTIME = QDateTime::currentDateTime();
    QString getLtime = GETTIME.toString("yyyy-MM-dd-hh-mm-ss");
    MY_AUTO_TIME = getLtime.toLatin1();
    MY_AUTO_TIME.prepend("date +%Y-%m-%d-%H-%M-%S\r\n");
    MY_AUTO_TIME.append("\r\n[root@localhost boa]# ");  //在得到的系统时间中加上前后缀以实现和获得的办卡时间格式相同
    qDebug() << MY_AUTO_TIME;
    ui->TextE_RES_TIP->append("日期测试已完成");
    m_serial->close();  //关掉串口
    ui->PBT_TEST_TIME->setEnabled(false);   //本项目测试完成，本按钮不可选
}
void Widget::on_PBT_LAN_NTE_clicked()   //执行测试LAN接口的网络功能测试
{
    int baudrate = 115200;
    SetSerialPortParam(m_serial,baudrate);
    m_serial->close();  //先关闭串口
    if( !m_serial->open(QIODevice::ReadWrite))  //打开串口
    {
        QMessageBox::critical(this,tr("Error"),tr("串口不存在或者被其他程序占用！"),QMessageBox::Ok);
        ui->PBT_XML->setEnabled(false);
        Test_array_LTime = ("NO TEST");
        return;
    }
    //倘若可以进行本项测试，那么串口波特率一定被设置为115200
    Send_line = 12; //此时Send_line=12，表示此时发送指令为检查网口是否成功连接
    QByteArray Test_lan_connect("ping 192.168.2.30 -s 65500 -c 1\n");
    m_serial->write(Test_lan_connect);
    QMessageBox::about(this,tr("提示"),tr("已向板卡发送检测网口命令"));
    if(Test_array_lan_connect.contains("1 packets transmitted, 1 received, 0% packet loss") == true)
    {
        QMessageBox::about(this,tr("提示"),tr("网口设置正确，,您可以进行本项测试"));
        ui->PBT_LAN_NTE->setEnabled(true);
    }
    else
    {
        QMessageBox::critical(this,tr("Error"),tr("网口连接错误，本测试终止"),QMessageBox::Ok);
        m_serial->close();
        return; //网口设置错误直接退出本项测试
    }
    //设置倒计时装置
    msgbox = new QMessageBox(this);
    msgbox->setGeometry(QRect(200, 150, 400, 300));
    okbtn     = new QPushButton("确定");
    msgbox->addButton(okbtn, QMessageBox::AcceptRole);

    timer_countdown = new QTimer(this);
    connect(timer_countdown, SIGNAL(timeout()), this, SLOT(count_time()));

    Send_line = 13; //此时Send_line=13，表示此时发送指令为测试
    QByteArray Test_lan_state("ping 192.168.2.30 -s 65500 -c 200\n");
    m_serial->write(Test_lan_state);
    ui->PBT_close->setEnabled(false);
    ui->PBT_TEST_TIME->setEnabled(false);
    ui->PBT_LAN_NTE->setEnabled(false);
    ui->PBT_RES->setEnabled(false);
    ui->PBT_XML->setEnabled(false);
    ui->PBT_SEARCH->setEnabled(false);
    ui->CBBOX_Port->setEnabled(false);
    ui->PBT_TEST_COM_CN8_115200->setEnabled(false); //让现在所有可选的按钮都不可选，以防用户操作
    ui->TextE_RES_TIP->append("测试进行中，请不要在本窗口进行任何操作");
    ui->TextE_RES_TIP->setEnabled(false);
    timer_countdown->start(INTERVAL*1000);
    msgbox_time_ = 0.0;
    msgbox->exec();
    if (msgbox->clickedButton() == okbtn)
    {
      qDebug()<<"确定被按下了";
      timer_countdown->stop();
      if(msgbox_time_ <= 210.0)
      QMessageBox::critical(this,tr("错误！"),tr("提前按下按钮，本项目测试失败"),QMessageBox::Ok);
    }
    //其实只需要设置3分20秒，但是为了保证接收数据的完整可信，所以设置3分30秒
    ui->TextE_RES_TIP->setEnabled(true);
    ui->PBT_close->setEnabled(true);
    ui->PBT_TEST_TIME->setEnabled(true);
    ui->PBT_LAN_NTE->setEnabled(true);
    ui->PBT_RES->setEnabled(true);
    ui->PBT_XML->setEnabled(true);
    ui->PBT_SEARCH->setEnabled(true);
    ui->CBBOX_Port->setEnabled(true);   //重新使能原来可用的按钮
    //下面进行对获得数据的操作处理
    if(Test_array_SEND_lan.contains("200 packets transmitted, 200 received, 0% packet loss"))
    {
        ui->TextE_RES_TIP->append("测试无丢包");
        bool state;
        QByteArray Test_lea = Test_array_SEND_lan.right(50);
        qDebug() << "最后的有效东西是" <<Test_lea;
        QByteArray Test_lan_res = Test_lea.mid(6,5);
        qDebug() << "结果判断数" <<Test_lan_res;
        if(Test_lan_res.toInt(&state,10) < 2)
            qDebug() << "成功";
        r[5] = 1;
    }
    else
    {
        ui->TextE_RES_TIP->append("本次测试有丢包，不合格！");
        r[5] = 2;
    }
    ui->TextE_RES_TIP->append("LAN接口网络设置测试完成");
    ui->TextE_RES_TIP->append("您现在已可查看测试结果");
    ui->PBT_LAN_NTE->setEnabled(false); //本项目测试完成，本项目不可选
    m_serial->close();
}

//设置串口参数,成功返回true，失败返回false
bool Widget::SetSerialPortParam(QSerialPort *serial,int Baudrate)   //设置串口参数
{
    QString strPorNum = ui->CBBOX_Port->currentText();  //设置串口号，这个需要根据用户自己选择
    if(strPorNum == tr(""))
        return false;
    serial->setPortName(strPorNum); //设置串口名称
    serial->setBaudRate(Baudrate);  //设置波特率
    serial->setParity(QSerialPort::NoParity);   //设置奇偶校验位
    serial->setDataBits(QSerialPort::Data8);    //设置数据位
    serial->setStopBits(QSerialPort::OneStop);  //设置停止位，一位停止位
    serial->setFlowControl(QSerialPort::NoFlowControl); //设置流控制为：无

    return true;
}

void Widget::on_PBT_SEARCH_clicked()    //点击检测可用串口
{
    SetPortNumCmb();    //把获取到的可用串口显示出来
}
void Widget::slot_RecvPortData()    //接收返回值
{
    Sleep(100); //便于一次接受完返回值
    QByteArray bytes = m_serial->readAll();
    if (Send_line == 1)
    {
        Test_array_SWITCH1.append(bytes);
        qDebug() << "转换文件夹指令返回值：" << Test_array_SWITCH1;
    }
    if (Send_line == 2)
    {
        Test_array_SEND1.append(bytes);
        qDebug() << "发送指令一返回值：" << Test_array_SEND1;
    }
    if (Send_line == 4)
    {
        Test_array_SWITCH2.append(bytes);
        qDebug() << "转换波特率38400是否成功返回值" << Test_array_SWITCH2;
    }
    if (Send_line == 5)
    {
        Test_array_SEND2.append(bytes);
        qDebug() << "发送指令二返回值" << Test_array_SEND2;
    }
    if (Send_line == 7)
    {
        Test_array_SWITCH3.append(bytes);
        qDebug() << "38400转换返回值" << Test_array_SWITCH3;
    }
    if (Send_line == 8)
    {
        Test_array_SEND_9600.append(bytes);
        qDebug() << "发送指令三返回值" << Test_array_SEND_9600;
    }
    if (Send_line == 10)
    {
        Test_array_SWITCH4.append(bytes);
        qDebug() << "日期测试115200返回值" << Test_array_SWITCH4;
    }
    if (Send_line == 11)
    {
        Test_array_LTime.append(bytes);
        qDebug() << "3A4000板卡时间返回值" << Test_array_LTime;
    }
    if (Send_line == 12)
    {
        Test_array_lan_connect.append(bytes);
        qDebug() << "检测网口连接返回值" << Test_array_lan_connect;
    }
    if (Send_line == 13)
    {
        Test_array_SEND_lan.append(bytes);
        qDebug() << "检测网口返回值" << Test_array_SEND_lan;
    }
}
void Widget::judge() //实现最终的结果判断
{
    if(Test_array_SWITCH1 == "cd /opt/boa\r\n[root@localhost boa]# ")
    {
        ui->label_RES_SWITCH->setText("SWITCH SUCCESS");
        r[0] = 1;
    }
    else if(Test_array_SWITCH1 == "NO TEST")
        ui->label_RES_SWITCH->setText("NO SWITCH");
    else
    {
        ui->label_RES_SWITCH->setText("SWITCH FAIL");
        r[0] = 2;
    }
    if(Test_array_SEND1 == "echo \"1G&\"\r\n1G&\r\n[root@localhost boa]# ")
    {
        ui->label_RES_115200->setText("PASS");
        r[1] = 1;
    }
    else if(Test_array_SEND1 == "NO TEST")
        ui->label_RES_115200->setText("NO TEST");
    else
    {
        ui->label_RES_115200->setText("NG");
        r[1] = 2;
    }
    if(Test_array_SEND2 == "echo \"G2#\"\r\nG2#\r\n[root@localhost boa]# ")
    {
        ui->label_RES_38400->setText("PASS");
        r[2] = 1;
    }
    else if(Test_array_SEND2 == "NO TEST")
        ui->label_RES_38400->setText("NO TEST");
    else
    {
        ui->label_RES_38400->setText("NG");
        r[2] = 2;
    }
    if(Test_array_SEND_9600 == "echo \"1Am!\"\r\n1Am!\r\n[root@localhost boa]# ")
    {
        ui->label_RES_9600->setText("PASS");
        r[3] = 1;
    }
    else if(Test_array_SEND_9600 == "NO TEST")
        ui->label_RES_9600->setText("NO TEST");
    else
    {
        ui->label_RES_9600->setText("NG");
        r[3] = 2;
    }
    if(Test_array_LTime == MY_AUTO_TIME)
    {
        ui->label_RES_TIME->setText("PASS");
        r[4] = 1;
    }
    else if(Test_array_LTime == "NO TEST")
        ui->label_RES_TIME->setText("NO TEST");
    else
    {
        ui->label_RES_TIME->setText("NG");
        r[4] = 2;
    }
    if(r[5] == 1)
        ui->label_RES_LAN->setText("PASS");
    else if(r[5] == 2)
        ui->label_RES_LAN->setText("NG");
    if(Test_array_SEND_lan == "NO TEST")
        ui->label_RES_LAN->setText("NO TEST");
}

void Widget::on_PBT_RES_clicked()   //当用户选择显示结果
{
    judge();
    ui->PBT_REAL_TIME->setEnabled(true);
    ui->PBT_XML->setEnabled(true);
}
void Widget::createXML(int test_array[])    //打开修改或者创建一个xml文件，即测试报告
{
    QFile file("MyAutoTest.xml"); //文件存在则打开没有则创建
    if (!file.open(QFileDevice::ReadWrite | QFileDevice::Truncate))
    {
        QMessageBox::information(NULL,"提示","文件打开或创建失败！");
                return;
    }
    QDomDocument doc;   //创建一个xml类
    QDomProcessingInstruction instruction;//创建xml处理类，通常用于处理第一行描述信息
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");   //创建xml头部格式
    doc.appendChild(instruction);//添加到xml文件中
    QDomProcessingInstruction renderer; //添加渲染处理指令，使本文件可以显示
    renderer = doc.createProcessingInstruction("xml-stylesheet", "type=\"text/css\" href=\"style.css\"");   //创建渲染指令
    doc.appendChild(renderer);

    //在本项目中，description\text\test_array\result的编号一致

    //创建根节点
    QDomElement root = doc.createElement("TestItem");
    doc.appendChild(root);

    /***********************************/
    //添加带属性的子节点0
    QDomElement item0 = doc.createElement("Item0");
    //给节点创建属性
    item0.setAttribute("ID",1);
    item0.setAttribute("NAME","SWITCH FILE POCKET");//参数一是字符串，参数二往下是任意值
    item0.setAttribute("转换数","1");

    //创建子元素
    QDomElement description0 = doc.createElement("description0");

    //设置尖括号中的值
    QDomText text0 = doc.createTextNode("转换结果，只有PASS以下测试才有效！");
    //添加关系
    description0.appendChild(text0);
    item0.appendChild(description0);

    //添加子元素二[1]
    if(test_array[0] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result0 = doc.createTextNode("无设备或未测试");
        state.appendChild(result0);
        item0.appendChild(state);
    }
    if(test_array[0] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result0 = doc.createTextNode("PASS");
        state.appendChild(result0);
        item0.appendChild(state);
    }
    if(test_array[0] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result0 = doc.createTextNode("NG");
        state.appendChild(result0);
        item0.appendChild(state);
    }
    root.appendChild(item0);

    //添加带属性的子节点1
    QDomElement item1 = doc.createElement("Item1");
    item1.setAttribute("ID",2);
    item1.setAttribute("NAME","SERIAL_CN8 TEST");//参数一是字符串，参数二往下是任意值
    item1.setAttribute("测试项目数","3");
    QDomElement description1 = doc.createElement("Description1");
    QDomText text1 = doc.createTextNode("baud112500");
    description1.appendChild(text1);
    item1.appendChild(description1);
    if(test_array[1] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result1 = doc.createTextNode("无设备或未测试");
        state.appendChild(result1);
        item1.appendChild(state);
    }
    if(test_array[1] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result1 = doc.createTextNode("PASS");
        state.appendChild(result1);
        item1.appendChild(state);
    }
    if(test_array[1] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result1 = doc.createTextNode("NG");
        state.appendChild(result1);
        item1.appendChild(state);
    }
    QDomElement description2 = doc.createElement("Description2");
    QDomText text2 = doc.createTextNode("baud38400");
    description2.appendChild(text2);
    item1.appendChild(description2);
    if(test_array[2] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result2 = doc.createTextNode("无设备或未测试");
        state.appendChild(result2);
        item1.appendChild(state);
    }
    if(test_array[2] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result2 = doc.createTextNode("PASS");
        state.appendChild(result2);
        item1.appendChild(state);
    }
    if(test_array[2] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result2 = doc.createTextNode("NG");
        state.appendChild(result2);
        item1.appendChild(state);
    }
    QDomElement description3 = doc.createElement("Description3");
    QDomText text3 = doc.createTextNode("baud9600");
    description3.appendChild(text3);
    item1.appendChild(description3);
    if(test_array[3] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result3 = doc.createTextNode("无设备或未测试");
        state.appendChild(result3);
        item1.appendChild(state);
    }
    if(test_array[3] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result3 = doc.createTextNode("PASS");
        state.appendChild(result3);
        item1.appendChild(state);
    }
    if(test_array[3] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result3 = doc.createTextNode("NG");
        state.appendChild(result3);
        item1.appendChild(state);
    }
    root.appendChild(item1);

    //添加带属性的子节点2
    QDomElement item2 = doc.createElement("Item2");
    //给节点创建属性
    item2.setAttribute("ID",3);
    item2.setAttribute("NAME","TIME_TEST");//参数一是字符串，参数二往下是任意值
    item2.setAttribute("测试项目数","1");

    //创建子元素
    QDomElement description4 = doc.createElement("description4");

    //设置尖括号中的值
    QDomText text4 = doc.createTextNode("板卡时间与系统时间是否一致");
    //添加关系
    description4.appendChild(text4);
    item2.appendChild(description4);

    //添加子元素二[1]
    if(test_array[4] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result4 = doc.createTextNode("无设备或未测试");
        state.appendChild(result4);
        item2.appendChild(state);
    }
    if(test_array[4] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result4 = doc.createTextNode("PASS");
        state.appendChild(result4);
        item2.appendChild(state);
    }
    if(test_array[4] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result4 = doc.createTextNode("NG");
        state.appendChild(result4);
        item2.appendChild(state);
    }
    root.appendChild(item2);

    //添加带属性的子节点
    QDomElement item3 = doc.createElement("Item3");
    //给节点创建属性
    item3.setAttribute("ID",4);
    item3.setAttribute("NAME","LAN_TEST");//参数一是字符串，参数二往下是任意值
    item3.setAttribute("测试项目数","1");

    //创建子元素
    QDomElement description5 = doc.createElement("description5");

    //设置尖括号中的值
    QDomText text5 = doc.createTextNode("LAN接口网络设置测试");
    //添加关系
    description5.appendChild(text5);
    item3.appendChild(description5);

    //添加子元素二[1]
    if(test_array[5] == 0)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result5 = doc.createTextNode("无设备或未测试");
        state.appendChild(result5);
        item3.appendChild(state);
    }
    if(test_array[5] == 1)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result5 = doc.createTextNode("PASS");
        state.appendChild(result5);
        item3.appendChild(state);
    }
    if(test_array[5] == 2)
    {
        QDomElement state = doc.createElement("state"); //测试的状态
        QDomText result5 = doc.createTextNode("NG");
        state.appendChild(result5);
        item3.appendChild(state);
    }
    root.appendChild(item3);
    /*********************************************/
    //将其写入到xml文件中
    QTextStream stream(&file);
    doc.save(stream,4); //缩进四格
    file.close();   //关闭文件
}

void Widget::on_PBT_XML_clicked()    //点击生成XML文件
{
    createXML(r);
    ui->label_XML->setText("已生成XML测试报告");
}

void Widget::on_PBT_REAL_TIME_clicked() //显示板卡时间
{
    if(Test_array_LTime == "NO TEST")
    {
        ui->label_realtime->setText("NO TEST");
    }
    else    //去掉板卡时间返回值的前后缀
    {
        Test_array_LTime.remove(0,25);
        Test_array_LTime.remove(19,26);
        ui->label_realtime->setText(Test_array_LTime);
        qDebug() << Test_array_LTime;
    }
    ui->PBT_REAL_TIME->setEnabled(false);
}

void Widget::count_time()   //倒计时装置
{
  double letf_time = TOTAL_TIME - msgbox_time_;
  if( letf_time <= 0 )
  {
    okbtn->click();
    return;
  }
  else
  {
    QString output;
    output.asprintf("本窗口将于%2.1f秒后自动关闭，请不要操作！ ", letf_time);
    msgbox->setText(output);
  }
  msgbox_time_ += INTERVAL;
}
