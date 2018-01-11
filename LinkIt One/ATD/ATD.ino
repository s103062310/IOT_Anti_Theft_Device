#include "Wire.h"
// I2Cdev and MPU9250 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU9250.h"
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU9250 accelgyro;
I2Cdev   I2C_M;

uint8_t buffer_m[6];
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t   mx, my, mz;
float heading;
float tiltheading;
float Axyz[3];
float Gxyz[3];
float Mxyz[3];
float oldAxyz[3];

#define sample_num_mdate  5000      

volatile float mx_sample[3];
volatile float my_sample[3];
volatile float mz_sample[3];

static float mx_centre = 0;
static float my_centre = 0;
static float mz_centre = 0;

#include <LGPS.h>

gpsSentenceInfoStruct info;
char buff[256];
typedef struct gps_info{
  int hour, minute, second;
  double latitude_value;
  char latitude;
  double latitude_output;
  double longtitude_value;
  char longtitude;
  double longtitude_output;
  double altitude_value;
  int fix_quality;
  int satellites;
};
gps_info GPS_INFO;


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();

  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  // it's really up to you depending on your project)
  Serial.begin(38400);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU9250 connection successful" : "MPU9250 connection failed");
  
  delay(1000);
  Serial.println("     ");
 
  //Mxyz_init_calibrated ();
  pinMode(13, OUTPUT);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ..."); 
  delay(3000);
  
}

void loop() 
{   
  
  getAccel_Data();
  //getGyro_Data();
  //getCompassDate_calibrated(); // compass data has been calibrated here 
  getHeading();       //before we use this function we should run 'getCompassDate_calibrated()' frist, so that we can get calibrated data ,then we can get correct angle .          
  getTiltHeading();           
 
  Serial.println("Acceleration(g) of X,Y,Z:");
  Serial.print(Axyz[0]); 
  Serial.print(",");
  Serial.print(Axyz[1]); 
  Serial.print(",");
  Serial.println(Axyz[2]);
  char buff[256];
  sprintf(buff, "%f\n", abs(Axyz[0]-oldAxyz[0])+abs(Axyz[1]-oldAxyz[1])+abs(Axyz[2]-oldAxyz[2]));
  Serial.println(buff);
  if( abs(Axyz[0]-oldAxyz[0])+abs(Axyz[1]-oldAxyz[1])+abs(Axyz[2]-oldAxyz[2])>0.2 ){
    digitalWrite(13, HIGH);
    LGPS.getData(&info);
    parseGPGGA((const char*)info.GPGGA);

    //TODO:upload latitude_output and longtitude_output to MCS
    
  }else{
    digitalWrite(13, LOW);
  }
   for(int i=0; i<3; i++)oldAxyz[i]=Axyz[i];

  delay(300);
  
}


void getHeading(void)
{
  heading=180*atan2(Mxyz[1],Mxyz[0])/PI;
  if(heading <0) heading +=360;
}

void getTiltHeading(void)
{
  float pitch = asin(-Axyz[0]);
  float roll = asin(Axyz[1]/cos(pitch));

  float xh = Mxyz[0] * cos(pitch) + Mxyz[2] * sin(pitch);
  float yh = Mxyz[0] * sin(roll) * sin(pitch) + Mxyz[1] * cos(roll) - Mxyz[2] * sin(roll) * cos(pitch);
  float zh = -Mxyz[0] * cos(roll) * sin(pitch) + Mxyz[1] * sin(roll) + Mxyz[2] * cos(roll) * cos(pitch);
  tiltheading = 180 * atan2(yh, xh)/PI;
  if(yh<0)    tiltheading +=360;
}


void getAccel_Data(void)
{
  accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  Axyz[0] = (double) ax / 16384;//16384  LSB/g
  Axyz[1] = (double) ay / 16384;
  Axyz[2] = (double) az / 16384; 
}

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
  sscanf(GPGGAstr,"$GPGGA,%2d%2d%2d%*4s,%lf,%c,%lf,%c,%d,%d,,%lf,%*s",&GPS_INFO.hour,&GPS_INFO.minute,&GPS_INFO.second,&GPS_INFO.latitude_value,&GPS_INFO.latitude,&GPS_INFO.longtitude_value,&GPS_INFO.longtitude,&GPS_INFO.fix_quality,&GPS_INFO.satellites,&GPS_INFO.altitude_value);
  sprintf(buff, "UTC time    : %02d:%02d:%02d", GPS_INFO.hour, GPS_INFO.minute, GPS_INFO.second);
  Serial.println(buff);
  sprintf(buff, "Latitude    : %3dd %2d' %2d\" %c", (int)GPS_INFO.latitude_value/100, (int)GPS_INFO.latitude_value%100, (int)((GPS_INFO.latitude_value-(int)GPS_INFO.latitude_value)*60.0), GPS_INFO.latitude);
  Serial.println(buff);

  GPS_INFO.latitude_output = (int)GPS_INFO.latitude_value/100;
  double temp = GPS_INFO.latitude_value - (int)GPS_INFO.latitude_value/100*100;
  temp = temp/60.0;
  GPS_INFO.latitude_output+=temp;
  sprintf(buff, "output      : %lf", GPS_INFO.latitude_output);
  Serial.println(buff);
  
  sprintf(buff, "Longitude   : %3dd %2d' %2d\" %c", (int)GPS_INFO.longtitude_value/100, (int)GPS_INFO.longtitude_value%100, (int)((GPS_INFO.longtitude_value-(int)GPS_INFO.longtitude_value)*60.0), GPS_INFO.longtitude);
  Serial.println(buff);
  
  GPS_INFO.longtitude_output = (int)GPS_INFO.longtitude_value/100;
  temp = GPS_INFO.longtitude_value - (int)GPS_INFO.longtitude_value/100*100;
  temp = temp/60.0;
  GPS_INFO.longtitude_output+=temp;
  sprintf(buff, "output      : %lf", GPS_INFO.longtitude_output);
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
