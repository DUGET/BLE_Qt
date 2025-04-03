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

    ui->ledRadBtn->setDisabled(true);
    ui->devicesTable->horizontalHeader()->setStretchLastSection(true);
    ui->devicesTable->insertColumn(0);
    ui->devicesTable->setHorizontalHeaderLabels(QStringList{"Found devices"});
    ui->waitIndicator->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

STATE_DEFINE(MainWindow, Idle, NoEventData)
{
    qDebug() << "Idle state";

    ui->devNameEdit->setDisabled(false);
    ui->connectBtn->setText("Connect");
    ui->connectBtn->setDisabled(false);
    ui->scanBtn->setDisabled(false);
    ui->waitIndicator->disable();
}

STATE_DEFINE(MainWindow, Scanning, NoEventData)
{
    qDebug() << "Scanning state";

    ui->devNameEdit->setDisabled(true);
    ui->connectBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);
    ui->waitIndicator->enable();

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

    ui->devNameEdit->setDisabled(true);
    ui->connectBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);
    ui->waitIndicator->enable();

    ble->startDiscovery(data->name);
}

STATE_DEFINE(MainWindow, Connected, NoEventData)
{
    qDebug() << "Connected state";

    ui->connectBtn->setText("Disconnect");
    ui->connectBtn->setDisabled(false);
    ui->ledRadBtn->setDisabled(false);
    ui->scanBtn->setDisabled(true);
    ui->waitIndicator->disable();
}

STATE_DEFINE(MainWindow, Disconnecting, DeviceNameData)
{
    qDebug() << "Disconnecting state";
    ui->ledRadBtn->setDisabled(true);
    ui->scanBtn->setDisabled(true);
    ui->connectBtn->setDisabled(true);

    ble->setDownConnection();
}

void MainWindow::on_connectBtn_clicked()
{
    DeviceNameData* deviceName = new DeviceNameData(ui->devNameEdit->text());

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
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(ST_CONNECTED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(ST_IDLE)
    END_TRANSITION_MAP(NULL)
}

void MainWindow::on_deviceListReady()
{
    QTableWidget* table = ui->devicesTable;

    // Clear table
    table->clearContents();
    while(table->rowCount() != 0)
    {
        table->removeRow(0);
    }

    // Insert name of devices into the table
    for(auto &devInfo : *ble->getDeviceList())
    {
        table->insertRow(table->rowCount());
        table->setItem(table->rowCount()-1, 0, new QTableWidgetItem(devInfo->name()));
    }

    if(GetCurrentState() == ST_CONNECTING)
    {
        ble->setUpConnection();
        return;
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
    // Explicit check in order not to allocate extra memory
    if(GetCurrentState() != ST_IDLE)
    {
        return;
    }

    DeviceNameData* deviceName = new DeviceNameData(item->text());

    //TODO Pass device name
    BEGIN_TRANSITION_MAP
        TRANSITION_MAP_ENTRY(ST_CONNECTING)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)
    END_TRANSITION_MAP(deviceName)
}
