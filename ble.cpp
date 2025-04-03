#include "ble.h"

BLE::BLE() :
    targetDevice(nullptr)
{
    devDiscAgent = new QBluetoothDeviceDiscoveryAgent(this);
    devDiscAgent->setLowEnergyDiscoveryTimeout(3000);

    connect(devDiscAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLE::deviceDiscovered);
    connect(devDiscAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &BLE::scanError);
    connect(devDiscAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLE::scanFinished);
    connect(devDiscAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BLE::scanFinished);
}

BLE::~BLE()
{
    if(bleCntrl != nullptr) delete bleCntrl;
    if(devDiscAgent != nullptr) delete devDiscAgent;
    // Program crashes if uncommented
    // if(targetDevice != nullptr) delete targetDevice;
}

void BLE::startDiscovery(const QString targetName)
{
    this->targetName = targetName;
    qDebug() << "Starting discovery of:" << targetName;
    devices.clear();
    devDiscAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLE::startDiscovery()
{
    devices.clear();
    devDiscAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLE::setUpConnection()
{
    bleCntrl = QLowEnergyController::createCentral(*targetDevice, this);

    connect(bleCntrl, &QLowEnergyController::serviceDiscovered, this, &BLE::serviceDiscovered);
    connect(bleCntrl, &QLowEnergyController::discoveryFinished, this, &BLE::serviceScanDone);
    connect(bleCntrl, &QLowEnergyController::errorOccurred, this, &BLE::serviceError);
    connect(bleCntrl, &QLowEnergyController::connected, this, &BLE::connected);
    connect(bleCntrl, &QLowEnergyController::disconnected, this, &BLE::disconnected);

    qDebug() << "Establishing connection with:" << targetName;
    bleCntrl->connectToDevice();
}

void BLE::setDownConnection()
{
    bleCntrl->disconnectFromDevice();
    delete bleCntrl;
}

void BLE::setUpService(const QBluetoothUuid &service)
{
    qDebug() << "Setting up service";

    bleService = bleCntrl->createServiceObject(service, this);

    connect(bleService, &QLowEnergyService::characteristicRead, this, &BLE::charRead);
    connect(bleService, &QLowEnergyService::characteristicWritten, this, &BLE::charWritten);
    connect(bleService, &QLowEnergyService::stateChanged, this, &BLE::serviceStateChanged);

    bleService->discoverDetails();
}

void BLE::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    if(newState == QLowEnergyService::ServiceState::RemoteServiceDiscovered)
    {
        qDebug() << "Remote service discovered";

        bleCharacteristics = bleService->characteristics();

        if(bleCharacteristics.empty())
        {
            qDebug() << "Charachteristic list is empty";
            return;
        }

        for (const auto &item : std::as_const(bleCharacteristics)) {
            qDebug() << item.uuid().toByteArray();
        }
    }
}

void BLE::setLed(bool state, int charIndex)
{
    QByteArray data;
    data.resize(2);

    data[0] = 0x00;
    if(state)
    {
        data[1] = 0x01;
    }
    else
    {
        data[1] = 0x00;
    }

    bleService->writeCharacteristic(bleCharacteristics[charIndex], data);
}

void BLE::charRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qDebug() << "charRead";
 }

void BLE::charWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qDebug() << "charWritten";
}

void BLE::serviceDiscovered(const QBluetoothUuid &newService)
{
    if(newService.toUInt16() == 384)
    {
        qDebug("Service discovered");
        setUpService(newService);
    }
}

void BLE::serviceScanDone()
{
    qDebug("Service scan done");

}

void BLE::serviceError(QLowEnergyController::Error newError)
{
    qDebug("Service error");
}

void BLE::connected()
{
    qDebug() << "Connected to:" << targetName;
    bleCntrl->discoverServices();
    emit connectionUpdateSignal(true);
}

void BLE::disconnected()
{
    qDebug() << "Disconnected from:" << targetName;

    delete targetDevice;
    emit connectionUpdateSignal(false);
}

void BLE::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    if(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        auto devInfo = new QBluetoothDeviceInfo(device);

        auto it = std::find_if(devices.begin(), devices.end(), [devInfo](QBluetoothDeviceInfo *dev)
        {
            return devInfo->address() == dev->address();
        });

        if (it == devices.end()) {
            devices.append(devInfo);
        } else {
            auto oldDev = *it;
            *it = devInfo;
            delete oldDev;
        }

        if(targetName == nullptr)
        {
            return;
        }

        if(device.name().contains(targetName))
        {
            targetDevice = new QBluetoothDeviceInfo(device);
            devDiscAgent->stop();
        }
    }
}

void BLE::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if(error == QBluetoothDeviceDiscoveryAgent::NoError)
    {
        qDebug("NoError");
    }
    qDebug("Scan error");
}

void BLE::scanFinished()
{
    emit deviceListReady();

    if(targetDevice != nullptr)
    {
        qDebug() << "Found:" << targetName;

    }
    else
    {
        qDebug() << "Could not find:" << targetName;
    }
}
