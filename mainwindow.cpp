#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
before using socketcan, you must setup the can socket in terminal
using >ifconfig -a to get the can device name("can0 as default")

>sudo ip link set can0 down
>sudo ip link set can0 type can bitrate 50000
>sudo ip link set can0 up

*/


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setup_can();
    toSlaveHand = new QUdpSocket(this);
    fromSlaveHand = new QUdpSocket(this);
    fromSlaveHand->bind(QHostAddress::Any,17362);//slave hand transmit force data with this port
    timer = new QTimer(this);
    timer->start(10);//10ms
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
    connect(fromSlaveHand,SIGNAL(readyRead()),this,SLOT(slave_hand_recv()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::can_frame_ready(){
    QCanBusFrame frame = candev->readFrame();
    int canid = frame.frameId();
    int len = frame.payload().length();
    int dat,mag;
    QByteArray frame_payload = frame.payload();
    dat = (unsigned char)frame_payload[0] *256 + (unsigned char)frame_payload[1];
    mag = (unsigned char)frame_payload[2];
    QString datstr,framestr;
    datstr.setNum(dat);
    //framestr.sprintf("id = 0x%x, len = %d, %d, %d, %d",canid,len,frame_payload[0],frame_payload[1],frame_payload[2]);
    qDebug()<<framestr<<endl;

    if(canid == cfg_UploadID_list[0]){
        if(mag){
            ui->lineEdit_l1->setText(datstr);
        }
        else{
            ui->lineEdit_l1->setText("Mag error!");
        }
    }




}


void MainWindow::get_configuration(){
    int i;
    cfg = new QFile("paramlist.cfg");
    cfg->open(QIODevice::ReadOnly | QIODevice::Text);
    cfg->seek(0);
    QTextStream in(cfg);
    for(i = 0; i < 14 && !in.atEnd(); i++){
        QStringList list = in.readLine().split(',');
        cfg_UploadID_list[i] = list[1].toInt();
        cfg_Data_offset[i] = list[2].toInt();
        cfg_DownloadID_list[i] = list[3].toInt();
    }
    if(i != 14)
        qDebug()<<"configuration bad!";


}

void MainWindow::timer_out(){


}

void MainWindow::setup_can(){
    if(QCanBus::instance()->plugins().contains("socketcan")){

        qDebug()<<"socketcan plugins available!";
        candev = QCanBus::instance()->createDevice("socketcan","can0");
        if(candev->connectDevice()){
            qDebug()<<"can device is connected!";
            ui->statusBar->showMessage("can device connected");
            connect(candev,SIGNAL(framesReceived()),this,SLOT(can_frame_ready()));
            isCanDeviceConnected = true;
        }
        else{
            qDebug()<<"can device connection failed!";
            isCanDeviceConnected = false;
        }

        get_configuration();


    }
    else
        isCanDeviceConnected = false;

}

void MainWindow::slave_hand_recv(){
    QCanBusFrame frame;
    frame.setFrameId(0);

}
