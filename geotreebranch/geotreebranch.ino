// This #include statement was automatically added by the Particle IDE.
#include <SparkFunMMA8452Q.h>
#include "math.h"

MMA8452Q accel; 


#define EVENTNAME "geotreebranch"

/***
 *  D1 = SCL
 *  D0 = SDA
 *  A0 = Soil Moisture
 *  D3 = Rain Drip
 */

////////////////////////////
// Sketch Output Settings //
////////////////////////////
#define P Add or subtract 
// a declination to get a more accurate heading. Calculate 
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -8.58 // Declination (degrees) in Boulder, CO.

#define DRIP_SENSOR_PIN D3
#define SOIL_MOISTURE_PIN A0


int epoch=0;

float ax,ay,az,slam=0;

volatile int drips = 0;
volatile long last_drip = 0;

void drip_interrupt_handler() {
    if (last_drip-millis() > 100) drips++;
    last_drip = millis();
}



void setup() 
{
  
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
  if (!accel.begin(SCALE_4G, ODR_100))
  {
      Particle.publish("status", "failed to init", PRIVATE);
      delay(1000);
  } else {
      Particle.publish("status", "9dof init");
  }
  pinMode(DRIP_SENSOR_PIN,INPUT_PULLDOWN);
  attachInterrupt(DRIP_SENSOR_PIN, drip_interrupt_handler, RISING);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  if (accel.available()) {
      accel.read();
      ax = accel.cx;
      ay = accel.cy;
      az = accel.cz;
  }

}

void loop()
{
    slam = 0;
    for (int i=0;i<100;i++) {  
        if (accel.available()) {
            accel.read();
            float ax0 = ax;
            float ay0 = ay;
            float az0 = az;
            ax = accel.cx;
            ay = accel.cy;
            az = accel.cz;
            float dax = ax-ax0; 
            float day = ay-ay0;
            float daz = az-az0;
            float dot_product = dax*dax + day*day + daz*daz;  // squared amplitude of jerk
            
            // slam: summed leaf activity metric = sum of dot product 
            slam += dot_product; // integrate squared amplitude of jerk
        }
        delay(100);
    }

    // TODO: disable interrupts
    int rain = drips;   
    drips=0;
    // TODO: re-enable interrupts
   
    int moisture =  analogRead(SOIL_MOISTURE_PIN);
    
    // rain: tenth inch per second
    // slam: leaf activity units per second 
    // moisture: raw device output
    String json = String::format("{ \"rain\":%d, \"slam\":%f }",
                                     rain,        slam/10.f );

    Particle.publish(EVENTNAME, json, PRIVATE);
}