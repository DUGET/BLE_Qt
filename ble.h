#ifndef BLE_H
#define BLE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>

class BLE : public QObject
{
    Q_OBJECT

    QString targetName;
    QBluetoothDeviceDiscoveryAgent* devDiscAgent;
    QBluetoothDeviceInfo* targetDevice;
    QLowEnergyController* bleCntrl;
    QLowEnergyService* bleService;
    QList<QLowEnergyCharacteristic> bleCharacteristics;

    void setUpService(const QBluetoothUuid &service);

public:
    BLE();
    ~BLE();

    QList<QBluetoothDeviceInfo*> devices;

    void startDiscovery();
    void startDiscovery(const QString targetName);
    void setUpConnection();
    void setDownConnection();
    void setLed(bool state, int charIndex);

signals:
    void connectionUpdateSignal(bool connectionState);
    void deviceListReady();


public slots:
    void connected();
    void disconnected();

private slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();
    void serviceDiscovered(const QBluetoothUuid &newService);
    void serviceScanDone();
    void serviceError(QLowEnergyController::Error newError);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void charRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void charWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue);
};

#endif // BLE_H
