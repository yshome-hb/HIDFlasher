#ifndef FILETRANSMIT_H
#define FILETRANSMIT_H

#include <QFile>
#include <QTimer>
#include <QObject>
#include <QThread>
#include "usbhiddevice.h"

class FileTransmit : public QThread
{
    Q_OBJECT

public:
    explicit FileTransmit(QObject *parent = 0);
    ~FileTransmit();

    void setFileName(const QString &name);

    int isTransmitting();
    bool startTransmit(USBHIDDevice* hid = NULL, uint8_t id = 0);
    void stopTransmit();

protected:
    void run();

signals:
    void transmitProgress(int progress);
    void transmitStatus(int status);

private slots:
    void transmitTimeOut();

private:
    QFile *file;
    QTimer *transmitTimer;
    USBHIDDevice* usbHid;

    uint8_t reportId;
    uint64_t fileSize;
    uint64_t fileCount;
};

#endif // FILETRANSMIT_H
