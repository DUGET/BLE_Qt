#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , StateMachine(ST_MAX_STATES)
    , connectionState(false)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ble = new BLE();
    connect(ble, &BLE::connectionUpdateSignal, this, &MainWindow::on_connectionUpdate);
    connect(ble, &BLE::deviceListReady, this, &MainWindow::on_deviceListReady);
    connect(ui->devNameEdit, &QLineEdit::editingFinished, this, &MainWindow::on_connectBtn_clicked);

    ui->ledRadBtn->setDisabled(true);
    ui->devicesTable->horizontalHeader()->setStretchLastSection(true);
    ui->devicesTable->insertColumn(0);
    ui->devicesTable->setHorizontalHeaderLabels(QStringList{"Found devices"});
}

MainWindow::~MainWindow()
{
    delete ui;
}

STATE_DEFINE(MainWindow, Idle, NoEventData)
{

}

STATE_DEFINE(MainWindow, Scan, NoEventData)
{

}

STATE_DEFINE(MainWindow, Connect, NoEventData)
{

}

void MainWindow::on_connectBtn_clicked()
{
    if(connectionState)
    {
        ui->connectBtn->toggleDisabled();
        ble->setDownConnection();
        return;
    }

    auto devNameEditString = ui->devNameEdit->text();

    if(devNameEditString.contains(" ") || devNameEditString == "")
    {
        return;
    }

    ui->connectBtn->toggleDisabled();
    ui->scanBtn->setDisabled(true);
    ble->startDiscovery(ui->devNameEdit->text());
}


void MainWindow::on_ledRadBtn_clicked(bool checked)
{
    // Hardcoded
    ble->setLed(checked, 1);
}

void MainWindow::on_connectionUpdate(bool connectionState)
{
    ui->connectBtn->toggleDisabled();
    this->connectionState = connectionState;

    if(connectionState)
    {
        ui->connectBtn->setText("Disconnect");
        ui->ledRadBtn->setDisabled(false);
        ui->scanBtn->setDisabled(true);
    }
    else
    {
        ui->connectBtn->setText("Connect");
        ui->ledRadBtn->setDisabled(true);
        ui->scanBtn->setDisabled(false);
    }
}

void MainWindow::on_deviceListReady()
{
    ui->connectBtn->toggleDisabled();
    ui->scanBtn->setDisabled(false);
    QTableWidget* table = ui->devicesTable;

    // Clear table
    table->clearContents();
    while(table->rowCount() != 0)
    {
        table->removeRow(0);
    }

    // Insert name of devices into the table
    for(auto &devInfo : ble->devices)
    {
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(devInfo->name()));
    }
}


void MainWindow::on_scanBtn_clicked()
{
    if(connectionState)
    {
        return;
    }
    else
    {
        ui->connectBtn->toggleDisabled();
        ui->scanBtn->setDisabled(true);
    }

    ble->startDiscovery();
}

void MainWindow::on_devicesTable_itemDoubleClicked(QTableWidgetItem *item)
{
    qDebug() << item->text();
    ble->startDiscovery(item->text());
}

