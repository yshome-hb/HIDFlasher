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

    // usbHid = new USBHIDDevice(this, 0000, 0000);
    // USBHIDDevice::enumerate();

    // insertRowAction = new QAction(this);
    // insertRowAction->setObjectName(QString::fromUtf8("insertRowAction"));
    // removeRowAction = new QAction(this);
    // removeRowAction->setObjectName(QString::fromUtf8("removeRowAction"));
    // insertColumnAction = new QAction(this);
    // insertColumnAction->setObjectName(QString::fromUtf8("insertColumnAction"));
    // removeColumnAction = new QAction(this);
    // removeColumnAction->setObjectName(QString::fromUtf8("removeColumnAction"));
    // insertChildAction = new QAction(this);
    // insertChildAction->setObjectName(QString::fromUtf8("insertChildAction"));

    // insertRowAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I, R", nullptr));
    // removeRowAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R, R", nullptr));
    // insertColumnAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+I, C", nullptr));
    // removeColumnAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R, C", nullptr));
    // insertChildAction->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));

    // const QStringList headers({tr("Title"), tr("Description")});
    // QFile file(":/res/default.txt");
    // file.open(QIODevice::ReadOnly);
    // model = new TreeModel(headers, file.readAll());
    // file.close();

    // ui->viewTx->setModel(model);
    // for (int column = 0; column < model->columnCount(); ++column)
    //     ui->viewTx->resizeColumnToContents(column);

    // connect(ui->viewTx->selectionModel(), &QItemSelectionModel::selectionChanged,
    //         this, &MainWindow::updateActions);
    // connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    // connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    // connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    // connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    // connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);

    // lStatusTitle = new QLabel(this);
    // lStatusTitle->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    // lStatusTitle->setText("Last command status:");

    // lStatusValue = new QLabel(this);
    // lStatusValue->setPixmap(QPixmap(":/icon_ok.png", "PNG"));

    // lMsgCntValue = new QLabel(this);
    // lMsgCntValue->setText("Message count:     0");

    // ui->statusbar->addPermanentWidget(lStatusTitle);
    // ui->statusbar->addPermanentWidget(lStatusValue);
    // ui->statusbar->addWidget(lMsgCntValue);


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
        if(fileTransmit->startTransmit() == true)
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

MainWindow::~MainWindow()
{
    delete ui;
    delete fileTransmit;
}

