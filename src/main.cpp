
#define SerialMon Serial

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1

#define DUMP_AT_COMMANDS

#define SMS_TARGET  "45xxxxxxxxxxxxxxxxx"

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

#define BMP180_I2C_SDA 32
#define BMP180_I2C_SCL 33


#define UART_BAUD           9600
#define PIN_DTR             25
#define PIN_TX              27
#define PIN_RX              26
#define PWR_PIN             4

TwoWire I2CBME = TwoWire(0);
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

void ReadTemperature() {
  float temperatureC = bmp180.readTemperature();
  String TempString = "Temp: " + String(temperatureC, 2) + "°C";
  Serial.println(TempString);
}


void setup() {

  I2CBME.begin(BMP180_I2C_SDA, BMP180_I2C_SCL, 400000);
  bmp180.begin(0x77, &I2CBME);
  float temperatureC = bmp180.readTemperature();
  String TempString = "Temp: " + String(temperatureC, 2) + "°C";
  Serial.println(TempString);
  SerialMon.println("BMP180 initialized");
  delay(5000);

  SerialMon.begin(9600);
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);
  modemPowerOn();
  SerialMon.println("SerialAT initialized");
  //delay(10000);
  
  modem.getModemInfo();
  String smsMessage = "BattVoltage: " + String(modem.getBattVoltage()) + " mV\n" +
                    "Operator: " + modem.getOperator() + "\n" +
                    "Registration Status: " + modem.getRegistrationStatus() + "\n" +
                    "Signal Quality: " + modem.getSignalQuality() + "\n" +
                    "IMEI: " + modem.getIMEI() + "\n" +
                    "IMSI: " + modem.getIMSI()+ "\n" +
                    TempString;

  //modem.sendSMS(SMS_TARGET, smsMessage);

  
}

void loop() {
  while (SerialAT.available()) {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
        SerialAT.write(SerialMon.read());
    }
}