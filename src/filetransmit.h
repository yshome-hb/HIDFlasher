#ifndef FILETRANSMIT_H
#define FILETRANSMIT_H

#include <QFile>
#include <QTimer>
#include <QObject>
#include <QThread>

class FileTransmit : public QThread
{
    Q_OBJECT

public:
    explicit FileTransmit(QObject *parent = 0);
    ~FileTransmit();

    void setFileName(const QString &name);

    bool startTransmit();
    void stopTransmit();

    int getTransmitProgress();
    int getTransmitStatus();

protected:
    void run();

signals:
    void transmitProgress(int progress);
    void transmitStatus(int status);

private slots:
    void readTimeOut();
    void writeTimeOut();

private:
    int callback(int status, uint8_t *buff, uint32_t *len);

    uint32_t read(uint8_t *buff, uint32_t len);
    uint32_t write(uint8_t *buff, uint32_t len);

    QFile       *file;
    QTimer      *readTimer;
    QTimer      *writeTimer;

    int      progress;
    int status;
    uint64_t fileSize;
    uint64_t fileCount;
};

#endif // FILETRANSMIT_H
