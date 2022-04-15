#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filetransmit.h"
#include "usbhiddevice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_fileBrowse_clicked(void);
    void on_transButton_clicked(void);

private:
    Ui::MainWindow *ui;
    //USBHIDDevice* usbHid;
    FileTransmit *fileTransmit;

    bool transButtonStatus;
};

#endif // MAINWINDOW_H
