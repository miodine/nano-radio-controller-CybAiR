#include <su_radio_hfunc.hpp>
#include <main.h>


RF24 r24G(CE_PIN, CSN_PIN);                     // Create a Radio 2.4 GHz
RH_ASK r433M(2000, RX_433_PIN, -1 , -1);        // Create a radio 433 MHz
IBus ibus(NUM_CHANNELS);

su_radio radiox;




void setup()   
{
  Serial.begin(BAUD_RATE);
  radiox.radio_init(&r24G, &r433M);
}

void loop()  
{
    radiox.receive_data(&r24G, &r433M);

    radiox.ibus_transmit_data(&ibus);
}

