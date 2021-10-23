
#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RH_ASK.h>
#include <stdint.h>


#define CE_PIN   9
#define CSN_PIN 10

#define JOYSTICK_1_X A0
#define JOYSTICK_1_Y A1
#define JOYSTICK_2_X A2
#define JOYSTICK_2_Y A3

#define R433_TX 1
#define STATUS_LED 5

const uint64_t pipe = 0xE8E8F0F0E1LL; // dla 2.4 GHz

enum{
    MODE_RADIO_24G = 0,
    MODE_RADIO_433M,
    MODE_STOP
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

RF24 radio_24G(CE_PIN, CSN_PIN);                      // 2.4 GHz 
RH_ASK radio_433M(2000, -1, R433_TX, -1);             // 433 MHz

unsigned long time_prev  = 0;
unsigned long time_now;
unsigned long time_delta = 50;
unsigned long ping_time = 0;
unsigned long ping_delta_time;


uint16_t joy_readout = 0;
uint8_t mode_ = MODE_RADIO_433M;

uint8_t tx_data[9] = {0, };


unsigned long dx1 = 0;
unsigned long dx2 = 0;

void update_status_byte(uint8_t* datum, STATUS_BYTE status)
{
  *datum |= status;
}

void evaluate_emergency_stop()
{
    mode_ = MODE_STOP;
    digitalWrite(A4,HIGH);
}

void evaluate_band_change()
{
  if( mode_ != MODE_STOP)
  {
    if(analogRead(A6) < 500) mode_ = MODE_RADIO_24G;
    else mode_ = MODE_RADIO_433M;
  }
}


void setup()  
{
  //INIT 2.4GHz MODULE
  radio_24G.begin();
  radio_24G.setDataRate(RF24_250KBPS);
  radio_24G.openWritingPipe(pipe);

  //INIT BUTTONS
  pinMode(7, INPUT_PULLUP); //D7 BUTTON
  pinMode(8, INPUT_PULLUP); //D8 BUTTON

  pinMode(6, INPUT);
  pinMode(5,INPUT);
  pinMode(4, INPUT);
  pinMode(3,INPUT);
  

  pinMode(2, INPUT_PULLUP);

  
  attachInterrupt(digitalPinToInterrupt(2), evaluate_band_change, LOW);
  //STATUS 
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  
  if(!radio_433M.init()){}
  
}


void loop() 
{
  tx_data[0] = 0x00;
  if(mode_ != MODE_STOP) 
  {
    update_status_byte(&tx_data[0], N_E_STOP);
  }


  evaluate_band_change();

  joy_readout = (uint16_t)analogRead(JOYSTICK_1_X);
  tx_data[1] = (uint8_t)(joy_readout >> 8);
  tx_data[2] = (uint8_t)joy_readout;
  
  joy_readout = (uint16_t)analogRead(JOYSTICK_1_Y);
  tx_data[3] = (uint8_t)(joy_readout >> 8);
  tx_data[4] = (uint8_t)joy_readout;

  joy_readout = (uint16_t)analogRead(JOYSTICK_2_X);
  tx_data[5] = (uint8_t)(joy_readout >> 8);
  tx_data[6] = (uint8_t)joy_readout;

  joy_readout = (uint16_t)analogRead(JOYSTICK_2_Y);
  tx_data[7] = (uint8_t)(joy_readout >> 8);
  tx_data[8] = (uint8_t)joy_readout;

  if(digitalRead(7)==LOW) update_status_byte(&tx_data[0],BUTTON_J1_P);  
  if(digitalRead(8)==LOW) update_status_byte(&tx_data[0],BUTTON_J2_P); 

  if(analogRead(3)==HIGH) update_status_byte(&tx_data[0],BUTTON_S1_P); 
  if(digitalRead(4)==HIGH) update_status_byte(&tx_data[0],BUTTON_S2_P); 
  if(digitalRead(5)==HIGH) update_status_byte(&tx_data[0],BUTTON_S3_P); 
  if(digitalRead(6)==HIGH) update_status_byte(&tx_data[0],BUTTON_S4_P); 


  switch (mode_)
  {
  case MODE_RADIO_24G:
    digitalWrite(A5,HIGH);
    radio_24G.write(tx_data, sizeof(tx_data));
    digitalWrite(A5,LOW);
    break;
    
  case MODE_RADIO_433M:
    digitalWrite(A5,HIGH);
    update_status_byte(&tx_data[0], RADIO_433_SELECTED);
    radio_433M.send(tx_data, sizeof(tx_data));
    radio_433M.waitPacketSent();
    digitalWrite(A5,LOW);
    break;

  default: //CASE ESTOP -> COMUNICATE IT ON BOTH CHANNELS
    radio_24G.write(tx_data, sizeof(tx_data));
    update_status_byte(&tx_data[0], RADIO_433_SELECTED);
    radio_433M.send(tx_data, sizeof(tx_data));
    radio_433M.waitPacketSent();
    break;
  }

  time_now = millis();

  while((time_now - time_prev)<time_delta)
  {
    time_now = millis();
  }
  time_prev = millis();
}