#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), 
    ui(new Ui::MainWindow),
    fileTransmit(new FileTransmit)
{
    ui->setupUi(this);

    transButtonStatus = false;

    usbHid = new USBHIDDevice(this, 0x05ac, 0x0256);
    usbHid->setUsage(0xFF60, 0x0061);

    connect(fileTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(fileTransmit, SIGNAL(transmitStatus(int)), this, SLOT(transmitStatus(int)));
}

void MainWindow::on_connectButton_clicked(void)
{
    if(!usbHid->isConnected())
    {
        QMessageBox::warning(this, u8"失败", u8"未发现设备！", u8"关闭");
        return;
    }

    if(!usbHid->open())
    {
        QMessageBox::warning(this, u8"失败", u8"设备打开失败", u8"关闭");
        return;        
    }

    int ret = 0;
    uint8_t outData[32] = {0xA0};
    uint8_t inData[32];
    
    ret = usbHid->write(outData, 32);
    if(ret < 0)
    {
        usbHid->close();
        QMessageBox::warning(this, u8"失败", u8"无法读取设备信息", u8"关闭");
        return;  
    }

    ret = usbHid->read(inData, 32);
    if((ret < 0) || (inData[0] != 0xA0))
    {
        usbHid->close();
        QMessageBox::warning(this, u8"失败", u8"无法读取设备信息", u8"关闭");
        return;  
    }

    ui->devinfoLabel->setText((char *)inData+1);
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
    if(transButtonStatus == false){
        qDebug() << "start";

        fileTransmit->setFileName(ui->filePath->text());
        if(fileTransmit->startTransmit(usbHid) == true)
        {
            transButtonStatus = true;
            ui->transButton->setText(u8"取消");
        }
        else
        {
            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");
        }
    }else{
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
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileTransmit;
}

