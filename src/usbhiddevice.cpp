#include "usbhiddevice.h"
#include <QDebug>

QList<USBHIDDevice::DevInfo> USBHIDDevice::devList;

USBHIDDevice::USBHIDDevice(QObject* parent, uint16_t vid, uint16_t pid, wchar_t* serial) : QObject(parent)
{
    hid_init();
    _vid = vid;
    _pid = pid;
    if (serial != NULL)
       wcscpy(_serial, serial);

}

void USBHIDDevice::setParams(uint16_t vid, uint16_t pid, wchar_t* serial)
{
    _vid = vid;
    _pid = pid;
    if (serial != NULL)
       wcscpy(_serial, serial);
}

void USBHIDDevice::setUsage(uint16_t upage, uint16_t usage)
{
    _usagePage = upage;
    _usage = usage;
}

int USBHIDDevice::isConnected()
{
    if(!enumerate(_vid, _pid))
        return 0;

    for(int i = 0; i < devList.size(); i++)
    {
        USBHIDDevice::DevInfo devInfo = devList.at(i);
        if((_usagePage > 0 && _usagePage != devInfo.usagePage)
         ||(_usage > 0 && _usage != devInfo.usage))
        {
            devList.removeAt(i);
        }
    }

    return devList.size();
}

bool USBHIDDevice::isOpened()
{
    return activeDevice != NULL;
}

bool USBHIDDevice::open(int index)
{
    if(index < 0)
    {
        if(wcslen(_serial) > 0)
            activeDevice = hid_open(_vid, _pid, _serial);
        else
            activeDevice = hid_open(_vid, _pid, NULL);
    }
    else
    {
        activeDevice = hid_open_path(devList.at(index).path.toUtf8().data());
    }

    return activeDevice != NULL;
}

void USBHIDDevice::close()
{
    if (activeDevice)
    {
        hid_close(activeDevice);
        activeDevice = NULL;
    }
}

int USBHIDDevice::write(uint8_t* data, uint16_t size, uint8_t reportId)
{
    if (activeDevice == NULL)
        return -1;

    outReportBuf[0] = reportId;
    memcpy(outReportBuf + 1, data, size);
    return hid_write(activeDevice, outReportBuf, 32);
}

int USBHIDDevice::read(uint8_t* data, uint16_t size, int timeout)
{
    if (activeDevice == NULL)
        return -1;

    return hid_read_timeout(activeDevice, data, size, timeout);
}

int USBHIDDevice::setFeature(uint8_t* data, uint16_t size, uint8_t reportId)
{
    if (activeDevice == NULL)
        return -1;

    outReportBuf[0] = reportId;
    memcpy(outReportBuf + 1, data, size);
    return hid_send_feature_report(activeDevice, outReportBuf, size + 1);
}

int USBHIDDevice::getFeature(uint8_t* data, uint16_t size, uint8_t reportId)
{
    if (activeDevice == NULL)
        return -1;

    inReportBuf[0] = reportId;
    int ret = hid_get_feature_report(activeDevice, inReportBuf, size + 1);
    if(ret > 0)
    {
        memcpy(data, inReportBuf + 1, ret - 1);
    }
    return (ret - 1);
}

int USBHIDDevice::transmitData(uint8_t *outData, int outLen, uint8_t *inData, int *inlen, uint8_t reportId, int timeout)
{
    if (activeDevice == NULL)
        return -1;

    outReportBuf[0] = reportId;
    memcpy(outReportBuf + 1, outData, outLen);
    int ret = hid_write(activeDevice, outReportBuf, outLen);
    if(ret < 0)
        return ret;

    ret = hid_read_timeout(activeDevice, inData, *inlen, timeout);
    if(ret > 0)
        *inlen = ret;

    return ret;
}

bool USBHIDDevice::enumerate(uint16_t vid, uint16_t pid)
{
    bool status = false;
    hid_device_info* devStrings = hid_enumerate(vid, pid);

    if (devStrings != NULL)
        status = true;

    devList.clear();
    while(devStrings != NULL)
    {
        devList.push_back(DevInfo(devStrings));
        devStrings = devStrings->next;
    }
    hid_free_enumeration(devStrings);

    return status;
}

QList<USBHIDDevice::DevInfo>& USBHIDDevice::getDevList()
{
    return devList;
}
