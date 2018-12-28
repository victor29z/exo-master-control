#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCanBus>
#include <QCanBusDeviceInfo>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QUdpSocket>
#include <QTimer>
#include <QLineEdit>
#include <QPushButton>
#include "joint_data_type.h"
#include "stdlib.h"
#include "pcan_basic.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void can_frame_ready();
    void timer_out();
    void slave_hand_recv();
    void reset_but_clicked();
private slots:
    void on_pb_save_cfg_clicked();

private:
    Ui::MainWindow *ui;
    QCanBusDevice *candev;
    QFile *cfg;
    unsigned int cfg_UploadID_list[16];
    unsigned int cfg_DownloadID_list[16];
    unsigned int cfg_Data_offset[16];
    void get_configuration(void);
    void save_configuration(void);
    void setup_can(void);
    QUdpSocket* toSlaveHand;
    QUdpSocket* fromSlaveHand;
    QTimer* timer;
    bool isCanDeviceConnected;
    JOINT_DAT_TYPE joint_data;
    QLineEdit * LETable[16];
    QPushButton* pb_reset[16];
    pcan_basic* pcan_dev;
    unsigned char post_data[5];
};


#endif // MAINWINDOW_H
