
#define SerialMon Serial

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1

#define DUMP_AT_COMMANDS

#define SMS_TARGET  "+48501508680"

#include <SPI.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include <Adafruit_BMP085.h>
#include <Ticker.h>

#ifdef DUMP_AT_COMMANDS

#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


#define UART_BAUD           9600
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4

Adafruit_BMP085 bmp180;

void modemInfo(){
modem.getBattVoltage();
delay(1000);
modem.getOperator();
delay(1000);
modem.getRegistrationStatus();
delay(1000);
modem.getSignalQuality();
delay(1000);
modem.getIMEI();
delay(1000);
modem.getIMSI();
delay(1000);
modem.getModemInfo();
delay(1000);
modem.getRegistrationStatus();
delay(1000);
modem.getOperator();
}

void modemPowerOn()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1000);    //Datasheet Ton mintues = 1S
    digitalWrite(PWR_PIN, HIGH);
}


void modemPowerOff()
{
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);
    delay(1500);    //Datasheet Ton mintues = 1.2S
    digitalWrite(PWR_PIN, HIGH);
}






void setup() {
  Serial.begin(9600);
  SerialMon.begin(9600);
  modemPowerOn();
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  delay(10000);
  modem.getModemInfo();
  String smsMessage = "BattVoltage: " + String(modem.getBattVoltage()) + " mV\n" +
                    "Operator: " + modem.getOperator() + "\n" +
                    "Registration Status: " + modem.getRegistrationStatus() + "\n" +
                    "Signal Quality: " + modem.getSignalQuality() + "\n" +
                    "IMEI: " + modem.getIMEI() + "\n" +
                    "IMSI: " + modem.getIMSI();





  modem.sendSMS(SMS_TARGET, smsMessage);
}

void loop() {
  delay(5);
}