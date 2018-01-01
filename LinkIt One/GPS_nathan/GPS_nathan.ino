#include <LGPS.h>

gpsSentenceInfoStruct info;
char buff[256];
typedef struct gps_info{
  int hour, minute, second;
  float latitude_value;
  char latitude;
  float longtitude_value;
  char longtitude;
  float altitude_value;
  int fix_quality;
  int satellites;
};
gps_info GPS_INFO;

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  sscanf(GPGGAstr,"$GPGGA,%2d%2d%2d%*4s,%f,%c,%f,%c,%d,%d,,%f,%*s",&GPS_INFO.hour,&GPS_INFO.minute,&GPS_INFO.second,&GPS_INFO.latitude_value,&GPS_INFO.latitude,&GPS_INFO.longtitude_value,&GPS_INFO.longtitude,&GPS_INFO.fix_quality,&GPS_INFO.satellites,&GPS_INFO.altitude_value);
  sprintf(buff, "UTC time    : %02d:%02d:%02d", GPS_INFO.hour, GPS_INFO.minute, GPS_INFO.second);
  Serial.println(buff);
  sprintf(buff, "Latitude    : %3dd %2d' %2d\" %c", (int)GPS_INFO.latitude_value/100, (int)GPS_INFO.latitude_value%100, (int)((GPS_INFO.latitude_value-(int)GPS_INFO.latitude_value)*60.0), GPS_INFO.latitude);
  Serial.println(buff);
  sprintf(buff, "Longitude   : %3dd %2d' %2d\" %c", (int)GPS_INFO.longtitude_value/100, (int)GPS_INFO.longtitude_value%100, (int)((GPS_INFO.longtitude_value-(int)GPS_INFO.longtitude_value)*60.0), GPS_INFO.longtitude);
  Serial.println(buff);
  Serial.print("Altitude    : ");
  Serial.println(GPS_INFO.altitude_value);
  if(GPS_INFO.fix_quality==0)Serial.println("Fix quality : GPS Invalid");//未定位
  else if(GPS_INFO.fix_quality==1)Serial.println("Fix quality : GPS fix");//非差分定位
  else if(GPS_INFO.fix_quality==2)Serial.println("Fix quality : DGPS fix");//差分定位
  Serial.print("Satellite(s): ");
  Serial.println(GPS_INFO.satellites);
  Serial.println("----------------------------");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ..."); 
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  LGPS.getData(&info);
  //Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  delay(2000);
}

/*reference
install:
http://yehnan.blogspot.tw/2015/03/linkit-one_17.html
https://docs.labs.mediatek.com/resource/linkit-one/en/getting-started/get-started-on-windows
GPS:
http://aprs.gids.nl/nmea/#gga
http://b8807053.pixnet.net/blog/post/3610870-gps%E8%B3%87%E6%96%99%E6%A0%BC%E5%BC%8F
http://labs.mediatek.com/api/linkit-one/frames.html?frmname=topic&frmfile=index.html
http://yehnan.blogspot.tw/2015/03/linkit-onegps.html
*/
