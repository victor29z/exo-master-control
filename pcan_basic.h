#ifndef PCAN_BASIC_H
#define PCAN_BASIC_H

#include <QObject>

#include <QLibrary>
#include <QDebug>
#include <fcntl.h>

extern "C"{
    #include <libpcan.h>
}
class pcan_basic : public QObject
{
    Q_OBJECT
public:
    explicit pcan_basic(QObject *parent = nullptr);
    bool InitState;
    HANDLE pcan_handle;
    bool pcan_init();
signals:

public slots:
};

#endif // PCAN_BASIC_H
