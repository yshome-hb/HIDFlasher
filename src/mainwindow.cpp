#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), 
    ui(new Ui::MainWindow),
    usbHid(new USBHIDDevice),
    fileTransmit(new FileTransmit)
{
    ui->setupUi(this);

    transButtonStatus = false;

    usbHid->setUsage(0, 0);

    connect(fileTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(fileTransmit, SIGNAL(transmitStatus(int)), this, SLOT(transmitStatus(int)));
}

void MainWindow::on_connectButton_clicked(void)
{
    if(usbHid->isOpened())
        usbHid->close();

    usbHid->setParams(ui->vendorId->text().toUShort(nullptr, 16), ui->productId->text().toUShort(nullptr, 16));

    int device_num = usbHid->isConnected();
    if(!device_num)
    {
        QMessageBox::warning(this, u8"失败", u8"未发现设备！", u8"关闭");
        ui->devinfoLabel->setText(u8"未发现设备！");
        return;
    }

    uint8_t reportId = ui->reportId->text().toUShort(nullptr, 10);
    uint8_t outData[32] = {0x00};
    uint8_t inData[32];
    int inLen = sizeof(inData);
    QString errorTitle = u8"未发现设备！";
    for(int i = 0; i < device_num; i++)
    {
        if(!usbHid->open(i))
        {
            continue;
        }

        if(usbHid->transmitData(outData, sizeof(outData), inData, &inLen, reportId) < 0)
        {
            usbHid->close();
            continue;  
        }

        if(inLen != sizeof(inData) || inData[0] != reportId)
        {
            usbHid->close();
            continue;  
        }

        errorTitle = nullptr;
        break;
    }

    if(errorTitle == nullptr)
    {
        ui->devinfoLabel->setText((char *)inData+1);
        ui->sendButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this, u8"失败", u8"设备打开失败", u8"关闭");
        ui->devinfoLabel->setText(errorTitle);
    }
}

void MainWindow::on_fileBrowse_clicked(void)
{
    ui->filePath->setText(QFileDialog::getOpenFileName(this, u8"打开文件", ".", u8"任意文件 (*.bin*)"));

    if(ui->filePath->text().isEmpty() != true){
        ui->transButton->setEnabled(true);
    }else{
        ui->transButton->setDisabled(true);
    }
}

void MainWindow::on_transButton_clicked(void)
{
    if(transButtonStatus == false)
    {
        qDebug() << "start";

        fileTransmit->setFileName(ui->filePath->text());
        if(fileTransmit->startTransmit(usbHid, ui->reportId->text().toUShort(nullptr, 10)) == true)
        {
            transButtonStatus = true;
            ui->transButton->setText(u8"取消");
            ui->sendButton->setEnabled(false);
        }
        else
        {
            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");
        }
    }
    else
    {
        qDebug() << "stop";
        fileTransmit->stopTransmit();
        transButtonStatus = false;
        ui->transButton->setText(u8"开始");
    }
}

void MainWindow::transmitProgress(int progress)
{
    ui->transProgress->setValue(progress);
}

void MainWindow::transmitStatus(int status)
{
    if(status)
    {
        QMessageBox::warning(this, u8"成功", u8"文件发送成功！", u8"关闭");
    }
    else
    {
        QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");
    }

    ui->transProgress->setValue(0);
    transButtonStatus = false;
    ui->transButton->setText(u8"开始");
    ui->devinfoLabel->setText(u8"未发现设备！");
}

void MainWindow::on_sendButton_clicked(void)
{
    uint8_t outData[32] = {0x00};
    uint8_t inData[32];
    int inLen = sizeof(inData);

    if(!usbHid->isOpened())
    {
        QMessageBox::warning(this, u8"失败", u8"未连接设备！", u8"关闭");
        return;
    }
    
    QString outText = ui->outReportEdit->text();

    qDebug() << outText;

    outText.remove(QRegExp("\\s"));
    QStringList outList = outText.split(",");
    for(int i = 0; i < outList.size(); i++)
    {
        QString outStr = outList.at(i);
        if(outStr.startsWith("0x") || outStr.startsWith("0X"))
            outData[i] = outStr.toUShort(nullptr, 16);
        else
            outData[i] = outStr.toUShort(nullptr, 10);

    }

    if(usbHid->transmitData(outData, sizeof(outData), inData, &inLen, ui->reportId->text().toUShort(nullptr, 10)) < 0)
    {
        QMessageBox::warning(this, u8"失败", u8"数据发送失败！", u8"关闭");
        return;
    }

    QString inText = "";
    for(int i = 0; i < inLen - 1; i++)
    {
        inText += QString("%1,").arg(inData[i], 2, 16, QLatin1Char('0'));
    }
    inText += QString("%1").arg(inData[inLen-1], 2, 16, QLatin1Char('0'));
    ui->inReportEdit->appendPlainText(inText);
}

void MainWindow::on_clearButton_clicked(void)
{
    ui->inReportEdit->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileTransmit;
}

