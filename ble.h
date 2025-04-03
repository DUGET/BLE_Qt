#ifndef BLE_H
#define BLE_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>

class BLE : public QObject
{
    Q_OBJECT

public:
    BLE();
    ~BLE();

    void startDiscovery();
    void startDiscovery(const QString targetName);
    void setUpConnection();
    void setDownConnection();
    void setLed(bool state, int charIndex);
    QList<QBluetoothDeviceInfo*>* getDeviceList();

private:
    void setUpService(const QBluetoothUuid &service);

    QString m_targetName;
    QBluetoothDeviceDiscoveryAgent* m_devDiscAgent;
    QBluetoothDeviceInfo* m_targetDevice = nullptr;
    QLowEnergyController* m_bleCntrl;
    QLowEnergyService* m_bleService;
    QList<QLowEnergyCharacteristic> m_bleCharacteristics;
    QList<QBluetoothDeviceInfo*> m_devices;

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
