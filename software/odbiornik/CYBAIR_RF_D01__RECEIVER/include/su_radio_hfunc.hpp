#ifndef RADIO_CONTROLLER_XD 
#define RADIO_CONTROLLER_XD


#include <main.h>


#define CE_PIN 9
#define CSN_PIN 10
#define RX_433_PIN 2


#define NUM_CHANNELS 5
#define BAUD_RATE 115200 //IBUS
#define UPDATE_INTERVAL 50 //IBUS

enum{
    MODE_RADIO_24G = 0,
    MODE_RADIO_433M,
    MODE_STOP
};

enum{
    TYPE_RECEIVER = 0,
    TYPE_TRANSMITTER
};

typedef enum STATUS_BYTE{
    N_E_STOP = 0x80,
    RADIO_433_SELECTED = 0x40, //do wyjebania
    BUTTON_J1_P = 0x20,
    BUTTON_J2_P = 0x10,
    BUTTON_S1_P = 0x08,
    BUTTON_S2_P = 0x04,
    BUTTON_S3_P = 0x02,
    BUTTON_S4_P = 0x01
} STATUS_BYTE;

class su_radio
{
private:
    uint16_t button_states = 0;
    uint16_t joy_status;
    uint8_t rx_data[9]; 
    const uint64_t pipe_24GHz = 0xE8E8F0F0E1LL;
    uint8_t rx_data_length = sizeof(rx_data);

public:
    unsigned long time;
    void radio_init(RF24* radio_24G, RH_ASK* radio_433M);
    void receive_data(RF24* radio_24G, RH_ASK* radio_433M);
    void ibus_transmit_data(IBus* ibus_);
    void print_parse();


    
    
};

#endif