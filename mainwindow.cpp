#include "mainwindow.h"
#include "ui_mainwindow.h"
/*
before using socketcan, you must setup the can socket in terminal
using >ifconfig -a to get the can device name("can0 as default")

sudo ip link set can0 down
sudo ip link set can0 type can bitrate 125000
sudo ip link set can0 up

*/
#define TO_SLAVE_HAND_PORT 12625
#define FROM_SLAVE_HAND_PORT 17362
char slave_hand_addr[20] = "192.168.1.140";


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    int i;
    ui->setupUi(this);
    setup_can();
    toSlaveHand = new QUdpSocket(this);
    fromSlaveHand = new QUdpSocket(this);
    fromSlaveHand->bind(QHostAddress::Any,FROM_SLAVE_HAND_PORT);//slave hand transmit force data with this port
    timer = new QTimer(this);
    timer->start(50);// send pos information to slave device every 50ms
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_out()));
    connect(fromSlaveHand,SIGNAL(readyRead()),this,SLOT(slave_hand_recv()));

    // setup ui
    LETable[0] = ui->lineEdit_l1;
    LETable[1] = ui->lineEdit_l2;
    LETable[2] = ui->lineEdit_l3;
    LETable[3] = ui->lineEdit_l4;
    LETable[4] = ui->lineEdit_l5;
    LETable[5] = ui->lineEdit_l6;
    LETable[6] = ui->lineEdit_l7;
    LETable[7] = ui->lineEdit_lh;

    LETable[8] = ui->lineEdit_r1;
    LETable[9] = ui->lineEdit_r2;
    LETable[10] = ui->lineEdit_r3;
    LETable[11] = ui->lineEdit_r4;
    LETable[12] = ui->lineEdit_r5;
    LETable[13] = ui->lineEdit_r6;
    LETable[14] = ui->lineEdit_r7;
    LETable[15] = ui->lineEdit_rh;



    for(i = 0;i < 16; i++){
        pb_reset[i] = new QPushButton("reset",this);
        int tmp_pos_left = LETable[i]->geometry().right();
        int tmp_pos_top = LETable[i]->geometry().bottom();

        pb_reset[i]->setGeometry(QRect(tmp_pos_left+20,tmp_pos_top,60,20));
        connect(pb_reset[i],SIGNAL(pressed()),this,SLOT(reset_but_clicked()));


    }




}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::can_frame_ready(){
    QCanBusFrame frame = candev->readFrame();
    unsigned int canid = frame.frameId();
    int len = frame.payload().length();
    int dat,mag;
    int i;
    QByteArray frame_payload = frame.payload();
    dat = (unsigned char)frame_payload[0] *256 + (unsigned char)frame_payload[1];
    mag = (unsigned char)frame_payload[2];

    //QString framestr;
    //framestr.sprintf("id = 0x%x, len = %d, %d, %d, %d",canid,len,frame_payload[0],frame_payload[1],frame_payload[2]);
    //qDebug()<<framestr<<endl;
    if(canid == 0x173)
        qDebug()<<dat;


    for(i = 0; i < 16; i++){
        if(canid == cfg_UploadID_list[i]){
            joint_data.joint_online[i] = true;
            if(mag){

                joint_data.joint_pos_raw[i] = dat;
                joint_data.joint_pos_valid[i] = true;

                if(joint_data.joint_pos_raw[i] >= cfg_Data_offset[i])
                    joint_data.joint_pos_abs[i] = joint_data.joint_pos_raw[i] - cfg_Data_offset[i];
                else
                    joint_data.joint_pos_abs[i] = joint_data.joint_pos_raw[i] + 4096 - cfg_Data_offset[i];

                //datstr.setNum(t);
                //LETable[i]->setText(datstr);
            }
            else{
                //LETable[i]->setText("Mag error!");
                joint_data.joint_pos_valid[i] = false;
            }
            break;
        }

    }





}

//read id configuration from the config file
void MainWindow::get_configuration(){
    int i;
    cfg = new QFile("paramlist.cfg");
    cfg->open(QIODevice::ReadOnly | QIODevice::Text);
    cfg->seek(0);
    QTextStream in(cfg);
    for(i = 0; i < 16 && !in.atEnd(); i++){
        QStringList list = in.readLine().split(',');
        cfg_UploadID_list[i] = list[1].toInt();
        cfg_Data_offset[i] = list[2].toInt();
        cfg_DownloadID_list[i] = list[3].toInt();
    }
    if(i != 16)
        qDebug()<<"bad configuration!";


}

void MainWindow::save_configuration(){
    int i;
    //cfg file is already opened
    //cfg = new QFile("paramlist.cfg");
    //cfg->open(QIODevice::ReadOnly | QIODevice::Text);
    cfg->seek(0);
    QTextStream out(cfg);
    for(i = 0; i < 16 ; i++){
        out << "ID"<< i << ","
            << cfg_UploadID_list[i] << ","
            << cfg_Data_offset[i]   << ","
            << cfg_DownloadID_list[i] << endl;
    }

}
void MainWindow::timer_out(){
    // send pos information to slave device every 10ms
    int i;
    int a,b;
//    char c;
    a = joint_data.joint_pos_abs[7];
    b = (a - 1885)*100/192;
    if(b<0){
        b = 0;
    }
    else if (b>230){
        b = 230;
    }
    post_data[2] = b;
    //qDebug()<<b;
 //   qDebug()<<c;
  //  toSlaveHand->writeDatagram((char*)(&joint_data), sizeof(JOINT_DAT_TYPE), QHostAddress(slave_hand_addr),16666);
//    toSlaveHand->writeDatagram((char*)(&joint_data), sizeof(JOINT_DAT_TYPE), QHostAddress(slave_hand_addr),TO_SLAVE_HAND_PORT);

    post_data[0] = 0x66;
    post_data[1] = 0xB0;
//    post_data[2] = (char)joint_data.joint_pos_abs[7];
    post_data[3] = ui->lineEdit_v->text().toUInt();
    post_data[4] = ui->lineEdit_F->text().toUInt();
//     qDebug()<< post_data[3];
    toSlaveHand->writeDatagram((char*)post_data, sizeof(int)*5, QHostAddress(slave_hand_addr),16666);



// debug force feedback with slider
    if(ui->cb_LH->isChecked()){
        QCanBusFrame frame;
        QByteArray frame_payload;
        frame_payload[0] = 1;//1 for LH, 2 for RH
        frame_payload[1] = ui->sld_LH->value();
        frame_payload.resize(2);
        frame.setFrameId(cfg_DownloadID_list[7]);
        frame.setPayload(frame_payload);

        candev->writeFrame(frame);

    }

    if(ui->cb_RH->isChecked()){
        QCanBusFrame frame;
        QByteArray frame_payload;
        frame_payload[0] = 2;//1 for LH, 2 for RH
        frame_payload[1] = ui->sld_RH->value();
        frame_payload.resize(2);
        frame.setFrameId(cfg_DownloadID_list[15]);
        frame.setPayload(frame_payload);

        candev->writeFrame(frame);

    }
// show joint position
    for(i = 0; i < 16; i++){
        if(joint_data.joint_online[i]){
            joint_data.joint_online[i] = false;
            if(joint_data.joint_pos_valid[i]){
                QString datstr;
                datstr.setNum(joint_data.joint_pos_abs[i]);
                LETable[i]->setText(datstr);
            }
            else{
                LETable[i]->setText("Mag error!");
            }
        }
        else{

            LETable[i]->setText("Joint offline");
        }
    }

}

void MainWindow::setup_can(){
    int i;
    //pcan_dev->pcan_init();

    return;
    // need to run with root privilege
    //system("ip link set can0 down");
    //system("ip link set can0 type can bitrate 50000");
    //system("ip link set can0 up");
    if(QCanBus::instance()->plugins().contains("socketcan")){
    //if(QCanBus::instance()->plugins().contains("peakcan")){

        qDebug()<<"socketcan plugins available!";
        candev = QCanBus::instance()->createDevice("socketcan","can0");
        //candev = QCanBus::instance()->createDevice("peakcan","usb0");

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

    // initialize the data struct
    for(i = 0; i < 16; i++){
        joint_data.joint_pos_raw[i] = 0;
        joint_data.joint_pos_abs[i] = 0;
        joint_data.joint_force[i] = 0;
        joint_data.joint_pos_valid[i] = false;
        joint_data.joint_online[i] = false;

    }

}

void MainWindow::slave_hand_recv(){
    JOINT_DAT_TYPE recv_frame;
    fromSlaveHand->readDatagram((char*)(&recv_frame),sizeof(JOINT_DAT_TYPE));
     qDebug() << recv_frame.joint_force;
    QCanBusFrame frame;
    QByteArray frame_payload;

    frame_payload[0] = 1;//1 for LH, 2 for RH
    frame_payload[1] = recv_frame.joint_force[7];
    frame_payload.resize(2);
    frame.setFrameId(cfg_DownloadID_list[7]);
    frame.setPayload(frame_payload);

    candev->writeFrame(frame);

    frame_payload[0] = 2;//1 for LH, 2 for RH
    frame_payload[1] = recv_frame.joint_force[15];
    frame_payload.resize(2);
    frame.setFrameId(cfg_DownloadID_list[15]);
    frame.setPayload(frame_payload);

    candev->writeFrame(frame);

}

void MainWindow::reset_but_clicked(){
    int i;
    for(i = 0; i < 16; i ++){
        if(pb_reset[i]->isDown()){
            //qDebug()<<"push button"<<i<<"is clicked";
            cfg_Data_offset[i] = joint_data.joint_pos_raw[i];

        }

    }

}

void MainWindow::on_pb_save_cfg_clicked()
{
    save_configuration();
}
