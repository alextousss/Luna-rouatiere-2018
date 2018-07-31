#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <SD.h>
#include <SPI.h>
#include <Servo.h>
#include "spectrometer.hpp"
#include "camera.hpp"
#include "ultrasonic.hpp"
#include "temp_sensor.hpp"
#include "statusled.hpp"
#include "constants.hpp"

template<class T> inline Print &operator <<(Print &obj, T arg) 
{ 
    obj.print(arg); 
    return obj; 
} // Nous permet d'utiliser l'affichage avec les opérateurs C++, 
  // Ex : Serial << "Hello world! : " << 28 << "\n"; 

unsigned long millisSince(unsigned long timestamp)
{
    return millis() - timestamp;
}

struct SensorsSample
{
    unsigned int timestamp;     // En Kelvins
    unsigned int external_temperature;
    unsigned int converter_temperature;
    unsigned int battery_temperature;
    unsigned int camera_temperature;
    unsigned int internal_temperature;
    unsigned int pressure;                 // En Pascals 
    unsigned int humidity;                 // En %
    unsigned int internal_ultrasonic_time;      // En microecondes
    unsigned int external_ultrasonic_time;
};

enum CamState {  
    INACTIVE,
    FIRST_LAUNCHED,
    FIRST_STOPPED, 
    SECOND_LAUNCHED, 
    SECOND_STOPPED, 
    THIRD_LAUNCHED
};

File results_file;
unsigned int sensors_data_file_index = 0;
unsigned int spectrometer_data_file_index = 0;

Servo maizena_servo;

Adafruit_BME280 bme; 
TempSensors ds18(PIN_DS18);
StatusLed status_led(PIN_STATUS_LED, 200);
UltrasonicSensor internal_hcsr(PIN_TRIG_1, PIN_ECHO_1);
UltrasonicSensor external_hcsr(PIN_TRIG_2, PIN_ECHO_2);
Camera lg360(PIN_SERVO_CAMERA);
Spectrometer spectrometer(PIN_SERVO_SPECTROMETER, PIN_PHOTODIODE);
SensorsSample sensors_samples[MAX_SENSORS_BUFFER];

CamState cam_state = FIRST_LAUNCHED;

unsigned int maizena_counter = 0;

unsigned int sensors_index = 0;
unsigned long last_sensors_feeding = 0;
unsigned long last_spectrometer_use = 0;



void setupBME280()
{
    Wire.setSCL(19);
    Wire.setSDA(18);
    Wire.begin();
    if( !bme.begin(0x76) )
    {
        Serial << "Error configuring BME280\n";
        status_led.error();
        for(;;);
    } 
}

void setupSDCard()
{
    if(!SD.begin(BUILTIN_SDCARD))
    {
        Serial << "Error accessing SD Card\n";
        status_led.error();
        for(;;);
    }  

    while( 
        SD.exists(
            String( 
                String(PREFIX_SENSORS) + String(++sensors_data_file_index) 
            ).c_str()
        )
    );         // Ici on fait une boucle qui place dans sensors_data_file_index
                     // le nombre (X) de la forme sensors_dataX sur lequel on peut écrire
                     // sans écraser un fichier
}


String serializeSample(SensorsSample* sample)
{
    String line = "";
    line += String(sample->timestamp)                + String(";");
    line += String(sample->external_temperature)     + String(";");
    line += String(sample->converter_temperature)    + String(";");
    line += String(sample->battery_temperature)      + String(";");
    line += String(sample->camera_temperature)       + String(";");
    line += String(sample->internal_temperature)     + String(";");
    line += String(sample->pressure)                 + String(";");
    line += String(sample->humidity)                 + String(";");
    line += String(sample->internal_ultrasonic_time) + String(";");
    line += String(sample->external_ultrasonic_time) + String(";");
    Serial.println(line);
    return line;
}

void sendDataToKikiwi()
{
    // Viens vérifier si la Kikiwi nous envoie Xon
    // Si c'est le cas, on envoie le dernier sample
    // Stoqué dans le buffer
    if( Serial5.available() )
    {
        String key = String(Serial5.read());
        if( key == String(17) )
        {
            Serial << "Kiwi : Xon" << endl;
            String response = serializeSample( &sensors_samples[sensors_index] );
            Serial5.println( response );
        }
    }
}

void writeSamplesToSD()
{
    Serial << "Writing " << sizeof(sensors_samples) << " bytes to SD card ..\n";
    String results_path = String(PREFIX_SENSORS) + String(sensors_data_file_index);
    results_file = SD.open(results_path.c_str(), FILE_WRITE);

    if( !results_file )
    {
        Serial << "Error accessing sensors data file !\n";
        status_led.error();
        // Pas de boucle infinie ici .. On va éviter de bloquer l'acquisition parce que la carte SD n'a pas fonctionné à un moment..
        return;
    }
     
    for( unsigned int i = 0 ; i < MAX_SENSORS_BUFFER ; i++ )
    {
        String line = serializeSample( &sensors_samples[i] );
        results_file.println(line);
    }

    results_file.close();
}

void feedSensorsSamples()
{
    last_sensors_feeding = millis();
    sensors_samples[sensors_index] = 
    {
        millis() / 1000,
        (unsigned int) (ds18.getCelcius(INDEX_EXTERNAL) + 273.15f),
        (unsigned int) (ds18.getCelcius(INDEX_CONVERTER) + 273.15f),
        (unsigned int) (ds18.getCelcius(INDEX_BATTERY) + 273.15f),
        (unsigned int) (ds18.getCelcius(INDEX_CAMERA) + 273.15f),
        (unsigned int) (bme.readTemperature() + 273.15f), // En Kelvins pour éviter les valeurs négatives
        (unsigned int) (bme.readPressure()),
        (unsigned int) (bme.readHumidity()),
        internal_hcsr.getRTT(),
        external_hcsr.getRTT(),
    };
    if( ++sensors_index == MAX_SENSORS_BUFFER ) // Si notre buffer est plein, 
    {                                           // on vient reinitialiser l'index et vider son contenu sur la SD
        sensors_index = 0;
        writeSamplesToSD();
    }
}

void useSpectrometer()
{
    Serial << "Use spectrometer" << endl;
    last_spectrometer_use = millis();
    unsigned int values[SPECTROMETER_N_VALUES];
    spectrometer.getValues(values);

    String results_path = String(PREFIX_SPECTROMETER) + String(spectrometer_data_file_index);
    results_file = SD.open(results_path.c_str(), FILE_WRITE);
    while( 
        SD.exists(
            String( 
                String(PREFIX_SPECTROMETER) + String(++spectrometer_data_file_index) 
            ).c_str()
        )
    );
   
    String line;    
    for( unsigned int i = 0 ; i < SPECTROMETER_N_VALUES ; i++ )
        line += String(i + SPECTROMETER_START_ANGLE)
              + String(", ")
              + String(values[i])
              + String("\n");
    results_file.println(line);

    results_file.close();
}

void launchMaizena()
{
  maizena_servo.attach(PIN_SERVO_MAIZENA);
  maizena_servo.write(255);
  delay(15000);
  maizena_servo.write(0);
  delay(100);
  maizena_servo.detach();
}

void setup()
{
    Serial.begin(115200);  // Connexion série avec l'ordinateur
    Serial5.begin(115200); // Connexion série avec la Kikiwi
    setupBME280();
    setupSDCard();
    lg360.record();
    Serial << "Init finished with success !\n";
}

  
void loop()
{
    if( cam_state == INACTIVE && millis() > MILLIS_FIRST_CAM_LAUNCH )
    {
        lg360.record();
        cam_state = FIRST_LAUNCHED;
        Serial << "FIRST LAUNCHED" << endl;
    }
    if( cam_state == FIRST_LAUNCHED  && millis() > MILLIS_FIRST_CAM_STOP )
    {
        lg360.stop();
        cam_state = FIRST_STOPPED;
        Serial << "FIRST STOPPED" << endl;
    }
    if( cam_state == FIRST_STOPPED && millis() > MILLIS_SECOND_CAM_LAUNCH )
    {
        lg360.record();
        cam_state = SECOND_LAUNCHED;
        Serial << "SECOND LAUNCHED" << endl;
    }
    if( cam_state == SECOND_LAUNCHED && millis() > MILLIS_SECOND_CAM_STOP )
    {
        lg360.stop();
        cam_state = SECOND_STOPPED;
        Serial << "SECOND STOPPED" << endl;
    }
    if( cam_state == SECOND_STOPPED && millis() > MILLIS_THIRD_CAM_LAUNCH )
    {
        lg360.record();
        cam_state = THIRD_LAUNCHED;
        Serial << "THIRD LAUNCHED" << endl; 
    }

    if( maizena_counter == 0 && millis()  > 2400000L )
    {
      launchMaizena();
      maizena_counter++;
    }
    if( maizena_counter == 1 && millis() > 4200000L )
    {
      launchMaizena();
      maizena_counter++;
    }

    ds18.requestTemperatures();
    status_led.updateBlink(); // Si on n'appelle pas cette fonction la led ne clignote plus
    if( millisSince(last_sensors_feeding) > (1.0F / SENSORS_REFRESH_RATE) * 1000.0F)
        feedSensorsSamples();
    
    if( millisSince(last_spectrometer_use) > SPECTROMETER_REFRESH_PERIOD )
        useSpectrometer();
    
    sendDataToKikiwi();     
}
