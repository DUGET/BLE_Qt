#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "StateMachine/statemachine.h"
#include "ble.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class DeviceNameData : public EventData
{
public:
    DeviceNameData(QString name)
    {
        this->name = name;
    }

    QString name;
};

class MainWindow : public QMainWindow, public StateMachine
{
    Q_OBJECT

    enum States
    {
        ST_IDLE,
        ST_SCANNING,
        ST_CONNECTING,
        ST_CONNECTED,
        ST_DISCONNECTING,
        ST_MAX_STATES
    };

    STATE_DECLARE(MainWindow, 	Idle,			NoEventData)
    STATE_DECLARE(MainWindow, 	Scanning,		NoEventData)
    STATE_DECLARE(MainWindow, 	Connecting,		DeviceNameData)
    STATE_DECLARE(MainWindow, 	Connected,		NoEventData)
    STATE_DECLARE(MainWindow, 	Disconnecting,	DeviceNameData)

    BEGIN_STATE_MAP
        STATE_MAP_ENTRY({&Idle})
        STATE_MAP_ENTRY({&Scanning})
        STATE_MAP_ENTRY({&Connecting})
        STATE_MAP_ENTRY({&Connected})
        STATE_MAP_ENTRY({&Disconnecting})
    END_STATE_MAP

    DeviceNameData* deviceName;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectBtn_clicked();
    void on_ledRadBtn_clicked(bool checked);
    void on_connectionUpdate(bool connectionState);
    void on_deviceListReady();
    void on_scanBtn_clicked();
    void on_devicesTable_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    BLE* ble;
};
#endif // MAINWINDOW_H
