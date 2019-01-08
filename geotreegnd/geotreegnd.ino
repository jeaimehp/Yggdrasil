#include "math.h"


#define EVENTNAME "geotreegnd"

/***
 *  D1 = SCL
 *  D0 = SDA
 *  A0 = Soil Moisture
 *  D3 = Rain Drip
 */


#define DRIP_SENSOR_PIN D3
#define SOIL_MOISTURE1_PIN A1
#define SOIL_MOISTURE2_PIN A2
#define SOIL_MOISTURE3_PIN A3

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
  pinMode(DRIP_SENSOR_PIN,INPUT_PULLDOWN);
  attachInterrupt(DRIP_SENSOR_PIN, drip_interrupt_handler, RISING);
  pinMode(SOIL_MOISTURE1_PIN, INPUT);
  pinMode(SOIL_MOISTURE2_PIN, INPUT);
  pinMode(SOIL_MOISTURE3_PIN, INPUT);
  Particle.publish("ground status", "init", PRIVATE);
}

void loop()
{
    slam = 0;


    // TODO: disable interrupts
    int rain = drips;   
    drips=0;
    // TODO: re-enable interrupts
   
    int moisture1 = analogRead(SOIL_MOISTURE1_PIN);
    int moisture2 = analogRead(SOIL_MOISTURE2_PIN);
    int moisture3 = analogRead(SOIL_MOISTURE3_PIN);
    
    // rain: tenth inch per second
    // slam: leaf activity units per second 
    // moisture: raw device output
    String json = String::format("{ \"rain\": %d, \"moisture1\":%i, \"moisture2\":%i, \"moisture3\":%i }",
                                      rain,         moisture1,        moisture2,        moisture3 );

    Particle.publish(EVENTNAME, json, PRIVATE);
    delay(10000);
}