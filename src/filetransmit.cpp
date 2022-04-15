#include <QFileInfo>
#include "filetransmit.h"
#include <QDebug>

#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

FileTransmit::FileTransmit(QObject *parent) :
    file(new QFile),
    readTimer(new QTimer),
    writeTimer(new QTimer)
{
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimeOut()));
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeTimeOut()));
}

FileTransmit::~FileTransmit()
{
    this->requestInterruption();
    this->quit();
    this->wait();

    delete file;
    delete readTimer;
    delete writeTimer;
}

void FileTransmit::run()
{
    while(1)
    {
        sleep(1);
    qDebug() << "thread";
        if(isInterruptionRequested())
        {
           break;
        }
    }
    //emit runDone();
}

void FileTransmit::setFileName(const QString &name)
{
    file->setFileName(name);
}

bool FileTransmit::startTransmit()
{
    if(file->open(QFIle::ReadOnly) == false)
        return false;

    QFileInfo fileInfo(*file);

    fileSize = fileInfo.size();
    
    this->start();
    // progress = 0;
    // status   = StatusEstablish;
    return true;
}

void FileTransmit::stopTransmit()
{
    this->requestInterruption();
    // file->close();
    // abort();
    // status = StatusAbort;
    // writeTimer->start(WRITE_TIME_OUT);
}

int FileTransmit::getTransmitProgress()
{
    return progress;
}

int FileTransmit::getTransmitStatus()
{
    return status;
}

void FileTransmit::readTimeOut()
{
    readTimer->stop();

    //transmit();

    // if((status == StatusEstablish) || (status == StatusTransmit))
    // {
    //     readTimer->start(READ_TIME_OUT);
    // }
}

void FileTransmit::writeTimeOut()
{
    // writeTimer->stop();
    // serialPort->close();
    // transmitStatus(status);
}

int FileTransmit::callback(int status, uint8_t *buff, uint32_t *len)
{
    // switch(status)
    // {
    //     case StatusEstablish:
    //     {
    //         if(file->open(QFile::ReadOnly) == true)
    //         {
    //             QFileInfo fileInfo(*file);

    //             fileSize  = fileInfo.size();
    //             fileCount = 0;

    //             strcpy((char *)buff, fileInfo.fileName().toLocal8Bit().data());
    //             strcpy((char *)buff + fileInfo.fileName().toLocal8Bit().size() + 1, QByteArray::number(fileInfo.size()).data());

    //             *len = YMODEM_PACKET_SIZE;

    //             FileTransmit::status = StatusEstablish;

    //             transmitStatus(StatusEstablish);

    //             return CodeAck;
    //         }
    //         else
    //         {
    //             YmodemFileTransmit::status = StatusError;

    //             writeTimer->start(WRITE_TIME_OUT);

    //             return CodeCan;
    //         }
    //     }

    //     case StatusTransmit:
    //     {
    //         if(fileSize != fileCount)
    //         {
    //             if((fileSize - fileCount) > YMODEM_PACKET_SIZE)
    //             {
    //                 fileCount += file->read((char *)buff, YMODEM_PACKET_1K_SIZE);

    //                 *len = YMODEM_PACKET_1K_SIZE;
    //             }
    //             else
    //             {
    //                 fileCount += file->read((char *)buff, YMODEM_PACKET_SIZE);

    //                 *len = YMODEM_PACKET_SIZE;
    //             }

    //             progress = (int)(fileCount * 100 / fileSize);

    //             YmodemFileTransmit::status = StatusTransmit;

    //             transmitProgress(progress);
    //             transmitStatus(StatusTransmit);

    //             return CodeAck;
    //         }
    //         else
    //         {
    //             YmodemFileTransmit::status = StatusTransmit;

    //             transmitStatus(StatusTransmit);

    //             return CodeEot;
    //         }
    //     }

    //     case StatusFinish:
    //     {
    //         file->close();

    //         YmodemFileTransmit::status = StatusFinish;

    //         writeTimer->start(WRITE_TIME_OUT);

    //         return CodeAck;
    //     }

    //     case StatusAbort:
    //     {
    //         file->close();

    //         YmodemFileTransmit::status = StatusAbort;

    //         writeTimer->start(WRITE_TIME_OUT);

    //         return CodeCan;
    //     }

    //     case StatusTimeout:
    //     {
    //         YmodemFileTransmit::status = StatusTimeout;

    //         writeTimer->start(WRITE_TIME_OUT);

    //         return CodeCan;
    //     }

    //     default:
    //     {
    //         file->close();

    //         YmodemFileTransmit::status = StatusError;

    //         writeTimer->start(WRITE_TIME_OUT);

    //         return CodeCan;
    //     }
    // }
    
    return 0;
}
