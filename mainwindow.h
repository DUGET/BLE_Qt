#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "ble.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

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
