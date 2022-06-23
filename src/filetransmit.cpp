#include <QFileInfo>
#include "filetransmit.h"
#include <QDebug>

#define TRANS_TIME_OUT  (10)

static int QDebug_hex_dump(uint8_t *data, int size)
{
    QString strTemp;
    for(int i = 0; i < size; i++)
    {
        strTemp += QString().sprintf("%02x ", data[i]);
    }
    qDebug() << strTemp;
    return size;
}

static uint16_t crc16(uint8_t *p, int len)
{
	uint16_t crc16_poly[2] = {0, 0xA001}; 
    uint16_t crc = 0xffff;

    for(int i = len; i > 0; i--)
    {
        uint8_t ds = *p++;
        for(int j = 0; j < 8; j++)
        {
            crc = (crc >> 1) ^ crc16_poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }
    return crc;
}

FileTransmit::FileTransmit(QObject *parent) :
    file(new QFile),
    transmitTimer(new QTimer)
{
    fileSize = 0;
    fileCount = 0;
    usbHid = NULL;
    connect(transmitTimer, SIGNAL(timeout()), this, SLOT(transmitTimeOut()));
}

FileTransmit::~FileTransmit()
{
    this->requestInterruption();
    this->quit();
    this->wait();

    delete file;
    delete transmitTimer;
}

void FileTransmit::run()
{
    uint8_t sendBuffer[33] = {0xF0, 0x0B, 0x01, 0x09, 0x05, 0x04, 0x52, 0x28, 0x00, 0x01, 0xFF};
    uint8_t recvBuffer[32];
    int recvLength = sizeof(recvBuffer);
    uint8_t* sendPtr = reportId ? (sendBuffer+1) : sendBuffer;
    
    if(usbHid->transmitData(sendPtr, 32, recvBuffer, &recvLength, reportId) < 0)
    {
        goto trans_fail;
    }

    if(recvLength != sizeof(recvBuffer) || recvBuffer[2] != 0x01)
    {
        goto trans_fail;
    }

    while(!isInterruptionRequested() && fileCount < fileSize)
    {
        memset(sendBuffer+11, 0xFF, 16);
        fileCount += file->read((char *)(sendBuffer+11), 16);
        sendBuffer[4] = 20 + 3;
        sendBuffer[3] = sendBuffer[4] + 4;
        sendBuffer[1] = sendBuffer[3] + 2;
        sendBuffer[9] = recvBuffer[9];
        sendBuffer[10] = recvBuffer[10];
        
        uint16_t data_crc = crc16(sendBuffer+9, 18);
        sendBuffer[27] = data_crc & 0xFF;
        sendBuffer[28] = (data_crc >> 8) & 0xFF;

        recvLength = sizeof(recvBuffer);
        if(usbHid->transmitData(sendPtr, 32, recvBuffer, &recvLength, reportId) < 0)
        {
            goto trans_fail;
        }
 
        // qDebug() << "recvLength "<<QString("0x%1").arg(recvLength);
        // QDebug_hex_dump(recvBuffer, 32);

        if(recvLength != sizeof(recvBuffer) || recvBuffer[2] != 0x01)
        {
            goto trans_fail;
        }

        transmitProgress(fileCount * 100 / fileSize);
    }

    memset(sendBuffer+9, 0xFF, 18);
    sendBuffer[9] = 0x02;
    usbHid->transmitData(sendPtr, 32, recvBuffer, &recvLength, reportId);

trans_fail:
    file->close();
    transmitStatus(fileCount >= fileSize);
    fileSize = 0;
    fileCount = 0;    
}

void FileTransmit::setFileName(const QString &name)
{
    file->setFileName(name);
}

int FileTransmit::isTransmitting()
{
    return fileSize - fileCount;
}

bool FileTransmit::startTransmit(USBHIDDevice* hid, uint8_t id)
{
    if(hid == NULL)
        return false;

    if(file->open(QFile::ReadOnly) == false)
        return false;

    QFileInfo fileInfo(*file);

    fileSize = fileInfo.size();
    fileCount = 0;
    usbHid = hid;
    reportId = id;

    this->start();
    //transmitTimer->start(TRANS_TIME_OUT);
    return true;
}

void FileTransmit::stopTransmit()
{
    this->requestInterruption();
}

void FileTransmit::transmitTimeOut()
{
    transmitTimer->stop();
    stopTransmit();
}
