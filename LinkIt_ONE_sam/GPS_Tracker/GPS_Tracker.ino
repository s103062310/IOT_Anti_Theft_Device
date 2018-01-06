#include <SPI.h>
#include "PubNub.h"

#include <LWiFi.h> 

#include <LGPS.h>
#include "GPSWaypoint.h"
#include "GPS_functions.h"

// import all the necessary files for WiFi connectivity
#include <LWiFiClient.h> // Add WiFi client
#define WIFI_AP "testWiFi" // provide your WIFI_AP name
#define WIFI_PASSWORD "password" //provide your WIFI password
#define WIFI_AUTH LWIFI_WPA


#define ledPin 13

GPSWaypoint* gpsPosition;


//define the required keys for using PubNub
char pubkey[] = "Publish Key";
char subkey[] = "Subscribe Key";
char channel[] = "gps_tracking";

void setup()
{
    pinMode(ledPin, OUTPUT);

    Serial.begin(9600);
    Serial.println("Serial setup");

    Serial.println("Connecting to AP");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
        Serial.println(" . ");
        delay(1000);
    }
    Serial.println("Connected to AP"); 

    PubNub.begin(pubkey, subkey);
    Serial.println("PubNub setup");

    LGPS.powerOn();
}

 
void flash(bool success)
{
     
    /* Flash LED three times. */
    for (int i = 0; i < 3; i++) {
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(100);
    }
}


void loop()
{
    Serial.println("Getting GPS Data");
    //Aquire GPS position
    char GPS_formatted[] = "GPS fixed";
    gpsPosition = new GPSWaypoint();
    gpsSentenceInfoStruct gpsDataStruct;
    getGPSData(gpsDataStruct, GPS_formatted, gpsPosition);
    Serial.println(" GPS Data aquired");


    char* buffer_latitude = new char[30];
    sprintf(buffer_latitude, "%2.6f", gpsPosition->latitude);

    char* buffer_longitude = new char[30];
    sprintf(buffer_longitude, "%2.6f", gpsPosition->longitude);

    String upload_GPS = "[{\"latlng\":[" + String(buffer_latitude) + "," + String(buffer_longitude)+ "], \"data\":\"hello1\"}]";
    
    const char* upload_char = upload_GPS.c_str();
     
    //Once Position is Aquired, upload it to PubNub
    LGPRSClient *client;

    
    Serial.println("publishing a message");
    client = PubNub.publish(channel, upload_char, 60);
    if (!client) {
        Serial.println("publishing error");
        delay(1000);
        return;
    }
    while (client->connected()) {
        while (client->connected() && !client->available()); // wait
        char c = client->read();
        Serial.print(c);
    }
    client->stop();
    Serial.println();
    flash(true);


    delay(5000);
}
