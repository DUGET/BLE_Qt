#include "ble.h"

BLE::BLE()
{
    m_devDiscAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_devDiscAgent->setLowEnergyDiscoveryTimeout(3000);

    connect(m_devDiscAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLE::deviceDiscovered);
    connect(m_devDiscAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &BLE::scanError);
    connect(m_devDiscAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLE::scanFinished);
    connect(m_devDiscAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BLE::scanFinished);
}

BLE::~BLE()
{
    if(m_bleCntrl != nullptr) delete m_bleCntrl;
    if(m_devDiscAgent != nullptr) delete m_devDiscAgent;
    // Program crashes if uncommented
    // if(targetDevice != nullptr) delete targetDevice;
}

void BLE::startDiscovery()
{
    m_targetName = "";
    m_devices.clear();
    m_devDiscAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLE::startDiscovery(const QString targetName)
{
    this->m_targetName = targetName;
    qDebug() << "Starting discovery of:" << targetName;
    m_devices.clear();
    m_devDiscAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLE::setUpConnection()
{
    m_bleCntrl = QLowEnergyController::createCentral(*m_targetDevice, this);

    connect(m_bleCntrl, &QLowEnergyController::serviceDiscovered, this, &BLE::serviceDiscovered);
    connect(m_bleCntrl, &QLowEnergyController::discoveryFinished, this, &BLE::serviceScanDone);
    connect(m_bleCntrl, &QLowEnergyController::errorOccurred, this, &BLE::serviceError);
    connect(m_bleCntrl, &QLowEnergyController::connected, this, &BLE::connected);
    connect(m_bleCntrl, &QLowEnergyController::disconnected, this, &BLE::disconnected);

    qDebug() << "Establishing connection with:" << m_targetName;
    m_bleCntrl->connectToDevice();
}

void BLE::setDownConnection()
{
    m_bleCntrl->disconnectFromDevice();
    delete m_bleCntrl;
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

    m_bleService->writeCharacteristic(m_bleCharacteristics[charIndex], data);
}

QList<QBluetoothDeviceInfo*>* BLE::getDeviceList()
{
    return &m_devices;
}

void BLE::setUpService(const QBluetoothUuid &service)
{
    qDebug() << "Setting up service";

    m_bleService = m_bleCntrl->createServiceObject(service, this);

    connect(m_bleService, &QLowEnergyService::characteristicRead, this, &BLE::charRead);
    connect(m_bleService, &QLowEnergyService::characteristicWritten, this, &BLE::charWritten);
    connect(m_bleService, &QLowEnergyService::stateChanged, this, &BLE::serviceStateChanged);

    m_bleService->discoverDetails();
}

void BLE::connected()
{
    qDebug() << "Connected to:" << m_targetName;
    m_bleCntrl->discoverServices();
    emit connectionUpdateSignal(true);
}

void BLE::disconnected()
{
    qDebug() << "Disconnected from:" << m_targetName;

    delete m_targetDevice;
    emit connectionUpdateSignal(false);
}

void BLE::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    if(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
    {
        auto devInfo = new QBluetoothDeviceInfo(device);

        auto it = std::find_if(m_devices.begin(), m_devices.end(), [devInfo](QBluetoothDeviceInfo *dev)
                               {
                                   return devInfo->address() == dev->address();
                               });

        if (it == m_devices.end()) {
            m_devices.append(devInfo);
        } else {
            auto oldDev = *it;
            *it = devInfo;
            delete oldDev;
        }

        if(m_targetName == "")
        {
            return;
        }

        if(device.name().contains(m_targetName))
        {
            m_targetDevice = new QBluetoothDeviceInfo(device);
            m_devDiscAgent->stop();
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

    if(m_targetDevice != nullptr)
    {
        qDebug() << "Found:" << m_targetName;

    }
    else
    {
        qDebug() << "Could not find:" << m_targetName;
    }
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

void BLE::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    if(newState == QLowEnergyService::ServiceState::RemoteServiceDiscovered)
    {
        qDebug() << "Remote service discovered";

        m_bleCharacteristics = m_bleService->characteristics();

        if(m_bleCharacteristics.empty())
        {
            qDebug() << "Charachteristic list is empty";
            return;
        }

        for (const auto &item : std::as_const(m_bleCharacteristics)) {
            qDebug() << item.uuid().toByteArray();
        }
    }
}

void BLE::charRead(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qDebug() << "charRead";
 }

void BLE::charWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
    qDebug() << "charWritten";
}
