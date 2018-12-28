#include "pcan_basic.h"

pcan_basic::pcan_basic(QObject *parent) : QObject(parent)
{
    pcan_handle = NULL;
}

bool pcan_basic::pcan_init(){

    pcan_handle = LINUX_CAN_Open("/dev/pcan32",O_RDWR);//CAN_Open(HW_USB,1);
    if(!pcan_handle){
        qDebug("PCAN open failed!");
        return false;
    }
    else{
        return true;
    }


}
