
#define SerialMon Serial

#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

#define SerialAT Serial1

#define DUMP_AT_COMMANDS

#define SMS_TARGET  "44xxxx"

#include <SPI.h>
#include <Wire.h>
#include <TinyGsmClient.h>
#include <Adafruit_BMP085.h>
#include <Ticker.h>
#include <ArduinoHttpClient.h>
#include <PubSubClient.h>

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


char apn[]  = "internet";
char user[] = "";
char pass[] = "";

//const char* broker = "broker.hivemq.com"; 

//const char* topicTemperature = "esp/temperature";

//const char* topicOutput1 = "esp/output1";



TwoWire I2CBME = TwoWire(0);
Adafruit_BMP085 bmp180;

TinyGsmClient client(modem);
//PubSubClient mqtt(client);


void czas() {
Serial.println("Acquire NTP Time");
modem.sendAT("+CNTP= \"194.146.251.100\",4,1,1");
modem.waitResponse();
modem.sendAT("+CNACT=1,\"internet\"");
modem.waitResponse();
modem.sendAT("+sapbr=1,1");
modem.waitResponse();
modem.sendAT("+CNTP");
}


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
delay(1000);
modem.sendAT("+CCLK?");
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
 
  SerialMon.begin(9600);
  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);


  //TEMP
  SerialMon.println("BMP180 initialized");
  float temperatureC = bmp180.readTemperature();
  String tempString = "Temp: " + String(temperatureC, 2) + "°C";
  SerialMon.println(tempString);
  
  delay(5000);

  
  modemPowerOn();
  SerialMon.println("SerialAT initialized");
  delay(5000);

  

  /*modem.gprsConnect(apn, user, pass);
  delay(100000);*/

  //mqtt.setServer(broker, 1883);

  //mqtt.connect("GsmClientN");

  //delay(10000);

  //mqtt.subscribe(topicOutput1);

 modemInfo();

  
  String smsMessage = "BattVoltage: " + String(modem.getBattVoltage()) + " mV\n" +
                    "Operator: " + modem.getOperator() + "\n" +
                    "Registration Status: " + modem.getRegistrationStatus() + "\n" +
                    "Signal Quality: " + modem.getSignalQuality() + "\n" +
                    "IMEI: " + modem.getIMEI() + "\n" +
                    "IMSI: " + modem.getIMSI() + "\n" +
                    tempString + "\n" +
                    "Kocham Cię Mocno";

                    
modem.sendSMS(SMS_TARGET, smsMessage);  

 
 delay(1000);
 // czas();
//TIME

}

void loop() {

  float temperatureC = bmp180.readTemperature();
  String tempString_serial = "Temp: " + String(temperatureC, 2) + "°C";
  SerialMon.println(tempString_serial);

  char tempString[8];
  dtostrf(temperatureC, 1, 2, tempString);

  //mqtt.publish(topicTemperature, tempString);

  delay(3000);

  while (SerialAT.available()) {
        SerialMon.write(SerialAT.read());
    }
    while (SerialMon.available()) {
        SerialAT.write(SerialMon.read());
    }
}
