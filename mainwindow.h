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

class MainWindow : public QMainWindow, public StateMachine
{
    Q_OBJECT

    enum States
    {
        ST_IDLE,
        ST_SCAN,
        ST_CONNECT,
        ST_MAX_STATES
    };

    STATE_DECLARE(MainWindow, 	Idle,			NoEventData)
    STATE_DECLARE(MainWindow, 	Scan,			NoEventData)
    STATE_DECLARE(MainWindow, 	Connect,		NoEventData)

    BEGIN_STATE_MAP
        STATE_MAP_ENTRY({&Idle})
        STATE_MAP_ENTRY({&Scan})
        STATE_MAP_ENTRY({&Connect})
    END_STATE_MAP

    bool connectionState;

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
