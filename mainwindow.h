#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCanBus>
#include <QDebug>
#include <QByteArray>
#include <QString>
#include <QFile>
#include <QUdpSocket>
#include <QTimer>
#include "joint_data_type.h"
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
private:
    Ui::MainWindow *ui;
    QCanBusDevice *candev;
    QFile *cfg;
    unsigned int cfg_UploadID_list[14];
    unsigned int cfg_DownloadID_list[14];
    unsigned int cfg_Data_offset[14];
    void get_configuration(void);
    void setup_can(void);
    QUdpSocket* toSlaveHand;
    QUdpSocket* fromSlaveHand;
    QTimer* timer;
    bool isCanDeviceConnected;
    JOINT_DAT_TYPE joint_data;
};


#endif // MAINWINDOW_H
