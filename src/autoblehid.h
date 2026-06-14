class AutoBleHid
{
public:
    void begin(void (*hidReport)(size_t length, uint8_t* data));
    void update();
};

extern AutoBleHid AUTOBLEHID;