#include <NimBLEDevice.h>
#include "blehid.h"

cBLEHID BLEHID;

static bool _isconnected = false;
static bool _isscanning = false;
static bool _iskeyboard = false;
static bool _ismouse = false;
static bool _isgamepad = false;

static const NimBLEAdvertisedDevice *_keyboard = nullptr;
static NimBLEScan *_scan = nullptr;
static NimBLEClient *_client = nullptr;

static void (*scanResult)(bool found) = nullptr;

static void (*_keyboardReport)(size_t len, uint8_t *data);
static void (*_gamepadReport)(size_t len, uint8_t *data);

// ------------------------------------------------------------------------------

class ClientCallbacks : public NimBLEClientCallbacks
{
public:
    explicit ClientCallbacks() {}
    void onConnect(NimBLEClient *pClient) override;
    void onDisconnect(NimBLEClient *pClient, int reason) override;
    void onPassKeyEntry(NimBLEConnInfo &connInfo) override;
    void onConfirmPasskey(NimBLEConnInfo &connInfo, uint32_t passkey) override;
    void onAuthenticationComplete(NimBLEConnInfo &connInfo) override;
};
static ClientCallbacks clientCallbacks;

void ClientCallbacks::onConnect(NimBLEClient *)
{
    _isconnected = true;
}

void ClientCallbacks::onDisconnect(NimBLEClient *, int reason)
{
    _isconnected = false;
    Serial.printf("Disconnected, reason=%d\n", reason);
    // clearMonitorSubscriptions();
}

void ClientCallbacks::onPassKeyEntry(NimBLEConnInfo &connInfo)
{
    Serial.println("Passkey requested; injecting default test value.");
    NimBLEDevice::injectPassKey(connInfo, 123456);
}

void ClientCallbacks::onConfirmPasskey(NimBLEConnInfo &connInfo, uint32_t passkey)
{
    Serial.print("Confirming passkey ");
    Serial.println(passkey);
    NimBLEDevice::injectConfirmPasskey(connInfo, true);
}

void ClientCallbacks::onAuthenticationComplete(NimBLEConnInfo &connInfo)
{
    if (!connInfo.isEncrypted())
    {
        Serial.println("Authentication complete, but link is not encrypted.");
        NimBLEClient *client = NimBLEDevice::getClientByHandle(connInfo.getConnHandle());
        if (client != nullptr)
        {
            client->disconnect();
        }
    }
    else
    {
        Serial.println("Authentication complete.");
    }
}

// ------------------------------------------------------------------------------

class ScanCallbacks : public NimBLEScanCallbacks
{
public:
    explicit ScanCallbacks() {}
    void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override;
    void onScanEnd(const NimBLEScanResults &results, int reason) override;
};
static ScanCallbacks scanCallbacks;

void ScanCallbacks::onResult(const NimBLEAdvertisedDevice *advertisedDevice)
{
    // Serial.println("Found something");

    if (_keyboard != nullptr)
        return;

    if (!advertisedDevice->isConnectable())
        return;

    if (!advertisedDevice->haveServiceUUID())
        return;

    // Only accept devices advertising the HID service (0x1812)
    if (!advertisedDevice->isAdvertisingService(NimBLEUUID("1812")))
        return;

    if (!advertisedDevice->haveAppearance())
        return;

    auto appearance = advertisedDevice->getAppearance();
    Serial.printf("Found appearance %04x\n", appearance);
    if (appearance == 0x3c1)
    {
        _keyboard = advertisedDevice;
        _iskeyboard = true;
    }

    if (::scanResult != nullptr)
        ::scanResult(true);
    _scan->stop();
    _isscanning = false;
}

void ScanCallbacks::onScanEnd(const NimBLEScanResults &results, int reason)
{
    _isscanning = false;
    // Serial.println("Scanning finished.");

    if (_keyboard != nullptr)
        return;

    if (::scanResult != nullptr)
        ::scanResult(false);
}

void cBLEHID::init()
{
    NimBLEDevice::init("BLEHID");
    NimBLEDevice::setPower(3);
    NimBLEDevice::setSecurityAuth(true, false, true);
    NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
}

void cBLEHID::scan(void (*scanResult)(bool found), uint32_t durationMs)
{
    if (_isscanning)
        return;

    ::scanResult = scanResult;

    _scan = NimBLEDevice::getScan();
    _scan->setScanCallbacks(&scanCallbacks, true);
    _scan->setInterval(100);
    _scan->setWindow(100);
    _scan->setActiveScan(true);

    _isscanning = true;

    // Serial.println("Scanning...");

    _scan->start(durationMs, false, true);
}

bool cBLEHID::connect()
{
    NimBLEDevice::getScan()->stop();

    auto address = _keyboard->getAddress();

    if (NimBLEDevice::getCreatedClientCount())
    {
        _client = NimBLEDevice::getClientByPeerAddress(address);
        if (_client != nullptr && _client->isConnected())
        {
            Serial.println("Already connected.");
            return true;
        }
        if (_client == nullptr)
        {
            _client = NimBLEDevice::getDisconnectedClient();
        }
    }

    if (_client == nullptr)
    {
        _client = NimBLEDevice::createClient();
        if (_client == nullptr)
        {
            Serial.println("Failed to create client.");
            return false;
        }
        _client->setClientCallbacks(&clientCallbacks, false);
        _client->setConnectionParams(12, 12, 0, 150);
        _client->setConnectTimeout(5000);
    }

    if (!_client->connect(address, false))
    {
        Serial.println("Connect failed.");
        return false;
    }

    return true;
}

void cBLEHID::disconnect()
{
    if (!_isconnected)
        return;
    _client->disconnect();
    _client = nullptr;
    _isconnected = false;
}

static std::vector<uint16_t> _subscribedHandles;

static NimBLERemoteCharacteristic *findHidCharacteristicByHandle(uint16_t handle)
{
    if (!_isconnected)
    {
        return nullptr;
    }

    NimBLERemoteService *hid = _client->getService(NimBLEUUID((uint16_t)0x1812));
    if (hid == nullptr)
    {
        return nullptr;
    }

    const auto &chars = hid->getCharacteristics(true);
    for (auto *chr : chars)
    {
        if (chr != nullptr && chr->getHandle() == handle)
        {
            return chr;
        }
    }

    return nullptr;
}

static void clearMonitorSubscriptions()
{
    if (!_isconnected)
    {
        _subscribedHandles.clear();
        return;
    }

    for (uint16_t handle : _subscribedHandles)
    {
        NimBLERemoteCharacteristic *chr = findHidCharacteristicByHandle(handle);
        if (chr != nullptr)
        {
            chr->unsubscribe(true);
        }
    }

    _subscribedHandles.clear();
}

static bool isHidInputReportCharacteristic(NimBLERemoteCharacteristic *characteristic, uint8_t *reportIdOut)
{
    if (characteristic == nullptr)
    {
        return false;
    }

    if (!characteristic->getUUID().equals(NimBLEUUID((uint16_t)0x2A4D)))
    {
        return false;
    }

    const auto &descriptors = characteristic->getDescriptors(true);
    for (auto *desc : descriptors)
    {
        if (desc == nullptr)
        {
            continue;
        }
        if (!desc->getUUID().equals(NimBLEUUID((uint16_t)0x2908)))
        {
            continue;
        }

        std::string value = desc->readValue();
        if (value.size() < 2)
        {
            continue;
        }

        const uint8_t reportId = static_cast<uint8_t>(value[0]);
        const uint8_t reportType = static_cast<uint8_t>(value[1]);
        if (reportIdOut != nullptr)
        {
            *reportIdOut = reportId;
        }
        return reportType == 1;
    }

    return false;
}

static void notifyCallback(NimBLERemoteCharacteristic *characteristic, uint8_t *data, size_t length, bool isNotify)
{
    Serial.printf("%s:", characteristic->getUUID().toString().c_str());
    _keyboardReport(length, data);
}

bool cBLEHID::listenReports(
    void (*keyboardReport)(size_t len, uint8_t *data),
    void (*gamepadReport)(size_t len, uint8_t *data))
{
    if (!isConnected())
        return false;

    clearMonitorSubscriptions();

    _keyboardReport = keyboardReport;
    _gamepadReport = gamepadReport;

    NimBLERemoteService *hid = _client->getService(NimBLEUUID((uint16_t)0x1812));
    if (hid == nullptr)
    {
        Serial.println("HID service not found.");
        return false;
    }

    bool subscribed = false;
    const auto &chars = hid->getCharacteristics(true);
    for (auto *chr : chars)
    {
        if (chr == nullptr)
        {
            continue;
        }

        uint8_t reportId = 0;
        const bool isBootKeyboard = chr->getUUID().equals(NimBLEUUID((uint16_t)0x2A22));
        const bool isInputReport = isHidInputReportCharacteristic(chr, &reportId);
        if (!isBootKeyboard && !isInputReport)
        {
            continue;
        }

        Serial.print("Inspecting ");
        Serial.print(chr->getUUID().toString().c_str());
        Serial.print(" notify=");
        Serial.print(chr->canNotify() ? "y" : "n");
        Serial.print(" indicate=");
        Serial.println(chr->canIndicate() ? "y" : "n");

        if (chr->canNotify())
        {
            if (chr->subscribe(true, notifyCallback))
            {
                subscribed = true;
                _subscribedHandles.push_back(chr->getHandle());
                Serial.print("Subscribed notify ");
                Serial.println(chr->getUUID().toString().c_str());
            }
            else
            {
                Serial.print("Failed subscribe notify ");
                Serial.println(chr->getUUID().toString().c_str());
            }
        }
        else if (chr->canIndicate())
        {
            if (chr->subscribe(false, notifyCallback))
            {
                subscribed = true;
                _subscribedHandles.push_back(chr->getHandle());
                Serial.print("Subscribed indicate ");
                Serial.println(chr->getUUID().toString().c_str());
            }
            else
            {
                Serial.print("Failed subscribe indicate ");
                Serial.println(chr->getUUID().toString().c_str());
            }
        }
    }

    if (!subscribed)
    {
        Serial.println("No HID characteristics were subscribable.");
    }
    return subscribed;
}

std::vector<uint8_t> cBLEHID::getHIDmap()
{
    NimBLERemoteCharacteristic *mapChar = nullptr;
    NimBLERemoteService *hid = nullptr;
    static std::vector<uint8_t> result = {};
    result.clear();

    if (!isConnected())
    {
        return result;
    }

    hid = _client->getService(NimBLEUUID((uint16_t)0x1812));
    if (hid == nullptr)
    {
        return result;
    }

    mapChar = hid->getCharacteristic(NimBLEUUID((uint16_t)0x2A4B));
    if (mapChar == nullptr)
    {
        return result;
    }

    std::string value = mapChar->readValue();
    if (value.empty())
    {
        return result;
    }

    auto byteArray = reinterpret_cast<const uint8_t*>(value.data());
    result = std::vector<uint8_t>(byteArray, byteArray + value.size());
    return result;
}

bool cBLEHID::isConnected() { return _isconnected; }
bool cBLEHID::isScanning() { return _isscanning; }
bool cBLEHID::isKeyboard() { return _iskeyboard; }
bool cBLEHID::isGamePad() { return _isgamepad; }
