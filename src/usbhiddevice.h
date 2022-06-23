#ifndef USBHIDDEVICE_H
#define USBHIDDEVICE_H

#include "hidapi.h"
#include "stdint.h"
#include "QString"
#include "QList"

class USBHIDDevice : public QObject
{
    Q_OBJECT
public:
    struct DevInfo
    {
        DevInfo(hid_device_info* info)
        {
            path = QString::fromUtf8(info->path);
            vid = info->vendor_id;
            pid = info->product_id;
            serialNum = QString::fromWCharArray(info->serial_number);
            releaseNum = info->release_number;
            manufacturerStr = QString::fromWCharArray(info->manufacturer_string);
            productStr = QString::fromWCharArray(info->product_string);
            usagePage = info->usage_page;
            usage = info->usage;
            interfaceNum = info->interface_number;
        }
        QString path;           // Platform-specific device path
        uint16_t vid;           // Device Vendor ID
        uint16_t pid;           // Device Product ID
        QString serialNum;      // Serial Number
        uint16_t releaseNum;    // Device Release Number in BCD
        QString manufacturerStr;// Manufacturer String
        QString productStr;     // Product string
        uint16_t usagePage;     // Usage Page for this Device/Interface (Windows/Mac only).
        uint16_t usage;         // Usage for this Device/Interface (Windows/Mac only).
        int interfaceNum;       // The USB interface which this logical device represents.
    };

    USBHIDDevice(QObject* parent = 0, uint16_t vid = 0, uint16_t pid = 0, wchar_t* serial = NULL);
    virtual ~USBHIDDevice() {};

    void setParams(uint16_t vid = 0, uint16_t pid = 0, wchar_t* serial = NULL);
    void setUsage(uint16_t upage, uint16_t usage);
    int isConnected();
    bool isOpened();

    bool open(int index = -1);
    void close();
    int write(uint8_t* data, uint16_t size, uint8_t reportId = 0);
    int read(uint8_t* data, uint16_t size, int timeout = -1);
    int setFeature(uint8_t* data, uint16_t size, uint8_t reportId = 0);
    int getFeature(uint8_t* data, uint16_t size, uint8_t reportId = 0);
    int transmitData(uint8_t *outData, int outLen, uint8_t *inData, int *inlen, uint8_t reportId = 0, int timeout = -1);

    static bool enumerate(uint16_t vid = 0, uint16_t pid = 0);
    static QList<DevInfo>& getDevList();

private:
    static QList<DevInfo> devList;
    hid_device* activeDevice = NULL;
    uint16_t _vid, _pid, _usagePage, _usage;
    wchar_t _serial[32] = {0};

    uint8_t outReportBuf[65];
    uint8_t inReportBuf[65];
};

#endif // USBHIDDEVICE_H
