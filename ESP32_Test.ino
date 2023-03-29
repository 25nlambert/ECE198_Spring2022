#include <SPI.h>
#include <SD.h>
#include "FS.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include "RTClib.h"
#include "DHT.h"

uint8_t  DHTPIN=4;
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


#define SD_SCK 33
#define SD_MISO 35
#define SD_MOSI 32
#define SD_CS 5

#define SEALEVELPRESSURE_HPA (1025.3984882666768)

int trigPin = 32;    // Trigger for ultrasonic
int echoPin = 33;    // Echo
double duration;


Adafruit_BMP3XX bmp;

RTC_PCF8523 rtc;

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

void writeFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
      #if ECHO_TO_SERIAL
        Serial.println("Failed to open file for writing");
      #endif
        return;
    }
    if(file.print(message)){
        //Serial.println("File written");
    } else {
      #if ECHO_TO_SERIAL
        Serial.println("Write failed");
      #endif
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, float message){//append with a float
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
      #if ECHO_TO_SERIAL
        Serial.println("Failed to open file for appending");
      #endif
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
      #if ECHO_TO_SERIAL
        Serial.println("Append failed");
      #endif
    }
    file.close();
}

void appendFileint(fs::FS &fs, const char * path, int message){//append with an int
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
      #if ECHO_TO_SERIAL
        Serial.println("Failed to open file for appending");
      #endif
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
      #if ECHO_TO_SERIAL
        Serial.println("Append failed");
      #endif
    }
    file.close();
}

void appendFilestr(fs::FS &fs, const char * path, const char * message){//append with a string
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
      #if ECHO_TO_SERIAL
        Serial.println("Failed to open file for appending");
      #endif
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
      #if ECHO_TO_SERIAL
        Serial.println("Append failed");
      #endif
    }
    file.close();
}


void setup(void)
{
  dht.begin();
  Serial.begin(115200);
   while (!Serial);
   #if ECHO_TO_SERIAL
  Serial.println("Adafruit BMP388 / BMP390 test");
   #endif

  if (!rtc.begin()){
    #if ECHO_TO_SERIAL
    Serial.println("Couldn't connect to rtc");
    #endif
  }
  pinMode(SD_CS, OUTPUT);
  if (!SD.begin(SD_CS)) {
    #if ECHO_TO_SERIAL
    Serial.println("Card failed, or not present");
    #endif
  }
  #if ECHO_TO_SERIAL
  Serial.println("card initialized.");
  #endif

  if (!SD.begin(SD_CS)) {
    #if ECHO_TO_SERIAL
    Serial.println("Card failed, or not present");
    #endif
  }
  #if ECHO_TO_SERIAL
  Serial.println("card initialized.");
  #endif

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  writeFile(SD, "/LOGGER00.CSV", "Datetime(EST),Speedtime,Temperature,Pressure,Altitude\n");//create the header of the CSV
#if ECHO_TO_SERIAL
  Serial.println("Datetime(EST),Speedtime,Temperature,Pressure,Altitude");
#endif //ECHO_TO_SERIAL

  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    #if ECHO_TO_SERIAL
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    #endif
    while (1);
  }

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);

  Wire.begin();  

}

void loop(void)
{


  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  #if ECHO_TO_SERIAL
  Serial.print("Sound: ");
  Serial.println(duration);
  #endif
  delay(100);

//begining of BMP
  DateTime now = rtc.now();
  
if (! bmp.performReading()) {
  #if ECHO_TO_SERIAL
    Serial.println("Failed to perform reading :(");
  #endif
    return;
  }
  #if ECHO_TO_SERIAL
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  #endif

  appendFileint(SD, "/LOGGER00.CSV", now.year());
  appendFilestr(SD, "/LOGGER00.CSV", "/");
  appendFileint(SD, "/LOGGER00.CSV", now.month());
  appendFilestr(SD, "/LOGGER00.CSV", "/");
  appendFileint(SD, "/LOGGER00.CSV", now.day());
  appendFilestr(SD, "/LOGGER00.CSV", " ");
  appendFileint(SD, "/LOGGER00.CSV", now.hour());
  appendFilestr(SD, "/LOGGER00.CSV", ":");
  appendFileint(SD, "/LOGGER00.CSV", now.minute());
  appendFilestr(SD, "/LOGGER00.CSV", ":");
  appendFileint(SD, "/LOGGER00.CSV", now.second());
  appendFilestr(SD, "/LOGGER00.CSV", ",");

  appendFile(SD, "/LOGGER00.CSV", duration);
  appendFilestr(SD, "/LOGGER00.CSV", ",");


  appendFile(SD, "/LOGGER00.CSV", bmp.temperature);
  appendFilestr(SD, "/LOGGER00.CSV", ",");
  appendFile(SD, "/LOGGER00.CSV", bmp.pressure);
  appendFilestr(SD, "/LOGGER00.CSV", ",");
  appendFile(SD, "/LOGGER00.CSV", bmp.readAltitude(SEALEVELPRESSURE_HPA));
  appendFilestr(SD, "/LOGGER00.CSV", "\n");


  float h=dht.readHumidity();
  float t = dht.readTemperature();
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print("Humidity: ");
  Serial.println(h);



  
#if ECHO_TO_SERIAL

  Serial.print(now.year(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.println('"');
#endif //ECHO_TO_SERIAL
  delay(2000);


#if ECHO_TO_SERIAL
  Serial.println();
#endif // ECHO_TO_SERIAL

}
