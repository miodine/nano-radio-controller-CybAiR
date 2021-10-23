#include <su_radio_hfunc.hpp>

void su_radio::radio_init(RF24* radio_24G, RH_ASK* radio_433M)
{
  radio_24G->begin();
  radio_24G->setDataRate(RF24_250KBPS);
  radio_24G->openReadingPipe(1, pipe_24GHz);
  radio_24G->startListening();
  radio_433M->init();
}

void su_radio::receive_data(RF24* radio_24G, RH_ASK* radio_433M)
{
  if (radio_24G->available())
  {
    {
      radio_24G->read(rx_data, rx_data_length);
      Serial.println("----------\r\n");
    }
  }
  if(radio_433M->recv(rx_data,&rx_data_length))
  {
      print_parse();
      Serial.println("----------\r\n");
  }
}

void su_radio::print_parse()
{
  if(rx_data[0] & N_E_STOP) Serial.println("STOPPED: NO\r\n");
  else Serial.println("STOPPED: YES\r\n");

  if(rx_data[0] & RADIO_433_SELECTED) Serial.println("RADIO 433: YES\r\n");
  else Serial.println("RADIO 433: NO\r\n");

  if(rx_data[0] & BUTTON_J1_P) Serial.println("JOY 1 SWITCH: 1\r\n");
  else Serial.println("JOY 1 SWITCH: 0\r\n");

  if(rx_data[0] & BUTTON_J2_P) Serial.println("JOY 2 SWITCH: 1\r\n");
  else Serial.println("JOY 2 SWITCH: 0\r\n");

  if(rx_data[0] & BUTTON_S1_P) Serial.println("OB 1 SWITCH: 1\r\n");
  else Serial.println("OB 1 SWITCH: 0\r\n");

  if(rx_data[0] & BUTTON_S2_P) Serial.println("OB 2 SWITCH: 1\r\n");
  else Serial.println("OB 2 SWITCH: 0\r\n");

  if(rx_data[0] & BUTTON_S3_P) Serial.println("OB 3 SWITCH: 1\r\n");
  else Serial.println("OB 3 SWITCH: 0\r\n");
  
  if(rx_data[0] & BUTTON_S4_P) Serial.println("OB 4 SWITCH: 1\r\n");
  else Serial.println("OB 3 SWITCH: 0\r\n");


  uint16_t joy_status = ((uint16_t)rx_data[1] << 8 | rx_data[2]);
  Serial.println("JOY 1 X: ");
  Serial.print(joy_status);
  Serial.println("\r\n");

  joy_status = ((uint16_t)rx_data[3] << 8 | rx_data[4]);
  Serial.println("JOY 1 Y: ");
  Serial.print(joy_status);
  Serial.println("\r\n");

  joy_status = ((uint16_t)rx_data[5] << 8 | rx_data[6]);
  Serial.println("JOY 2 X: ");
  Serial.print(joy_status);
  Serial.println("\r\n");

  joy_status = ((uint16_t)rx_data[7] << 8 | rx_data[8]);
  Serial.println("JOY 2 Y: ");
  Serial.print(joy_status);
  Serial.println("\r\n");
}


void su_radio::ibus_transmit_data(IBus* ibus_)
{

  ibus_->begin();

  //FLAGS AND BUTTONNS
  ibus_->write((uint16_t)rx_data[0]);

  //JOYSTICKS
  joy_status = ((uint16_t)rx_data[1] << 8 | rx_data[2]);
  ibus_->write(joy_status);

  joy_status = ((uint16_t)rx_data[3] << 8 | rx_data[4]);
  ibus_->write(joy_status);

  joy_status = ((uint16_t)rx_data[5] << 8 | rx_data[6]);
  ibus_->write(joy_status);

  joy_status = ((uint16_t)rx_data[7] << 8 | rx_data[8]);
  ibus_->write(joy_status);
  ibus_->end();



}

