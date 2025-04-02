#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , StateMachine(ST_MAX_STATES)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ble = new BLE();
    connect(ble, &BLE::connectionUpdateSignal, this, &MainWindow::on_connectionUpdate);
    connect(ble, &BLE::deviceListReady, this, &MainWindow::on_deviceListReady);
    connect(ui->devNameEdit, &QLineEdit::editingFinished, this, &MainWindow::on_connectBtn_clicked);

    deviceName = new DeviceNameData("");

    ui->ledRadBtn->setDisabled(true);
    ui->devicesTable->horizontalHeader()->setStretchLastSection(true);
    ui->devicesTable->insertColumn(0);
    ui->devicesTable->setHorizontalHeaderLabels(QStringList{"Found devices"});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectBtn_clicked()
{
    qDebug() << "On connect button";
    deviceName->name = ui->devNameEdit->text();

    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(ST_CONNECTING)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(ST_DISCONNECTING)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
    END_TRANSITION_MAP(deviceName)
}


void MainWindow::on_ledRadBtn_clicked(bool checked)
{
    // Hardcoded
    ble->setLed(checked, 1);
}

void MainWindow::on_connectionUpdate(bool connectionState)
{
    qDebug() << "On connection update";

    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)
        TRANSITION_MAP_ENTRY(ST_CONNECTED)
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)
        TRANSITION_MAP_ENTRY(ST_IDLE)
    END_TRANSITION_MAP(NULL)
}

void MainWindow::on_deviceListReady()
{
    qDebug() << "On device list ready";

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

    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(ST_IDLE)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
    END_TRANSITION_MAP(NULL)
}


void MainWindow::on_scanBtn_clicked()
{
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(ST_SCANNING)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
    END_TRANSITION_MAP(NULL)
}

void MainWindow::on_devicesTable_itemDoubleClicked(QTableWidgetItem *item)
{
    // Explicit check in order not to allocate memory
    if(GetCurrentState() == ST_CONNECTED)
    {
        return;
    }

    deviceName->name = item->text();

    //TODO Pass device name
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(ST_CONNECTING)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
    END_TRANSITION_MAP(deviceName)
}

STATE_DEFINE(MainWindow, Idle, NoEventData)
{
    qDebug() << "Idle state";
    ui->connectBtn->setText("Connect");
    ui->connectBtn->setDisabled(false);
    ui->scanBtn->setDisabled(false);
}

STATE_DEFINE(MainWindow, Scanning, NoEventData)
{
    qDebug() << "Scanning state";
    ui->connectBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);

    ble->startDiscovery();
}

STATE_DEFINE(MainWindow, Connecting, DeviceNameData)
{
    qDebug() << "Connecting state";
    if(data->name.contains(" ") || data->name == "")
    {
        InternalEvent(ST_IDLE);
        return;
    }

    ui->connectBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);
    ble->startDiscovery(data->name);
}

STATE_DEFINE(MainWindow, Connected, NoEventData)
{
    qDebug() << "Connected state";
    ui->connectBtn->setText("Disconnect");
    ui->connectBtn->setDisabled(false);
    ui->ledRadBtn->setDisabled(false);
    ui->scanBtn->setDisabled(true);
}

STATE_DEFINE(MainWindow, Disconnecting, DeviceNameData)
{
    qDebug() << "Disconnecting state";
    ui->ledRadBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);
    ui->connectBtn->setDisabled(true);

    ble->setDownConnection();
    InternalEvent(ST_IDLE);
}
