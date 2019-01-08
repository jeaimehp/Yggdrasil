// This #include statement was automatically added by the Particle IDE.
#include <SparkFunMMA8452Q.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_SI1145.h>

// This #include statement was automatically added by the Particle IDE.
#include <AssetTracker.h>

#define     SLEEP_MINUTES       15
#define     DHT_TYPE            DHT22
#define     DHT_PIN             A0

int led = D7; //built in LED

long sleep_duration = SLEEP_MINUTES * 60 * 1000;

Adafruit_SI1145 light = Adafruit_SI1145();
AssetTracker tracker = AssetTracker();
DHT dht(DHT_PIN, DHT_TYPE);
FuelGauge fuel;

String publish_json;

bool uvstatus = false;

void send_reading() {
   	//get sensor readings
    float temperature = dht.getTempCelcius();
    float humidity = dht.getHumidity();
    float visible = 0;
    float ir = 0;
    float uv = 0;
    float battery = fuel.getSoC();
    String latlon = tracker.readLatLon();
    
    if (uvstatus) {
        visible = light.readVisible();
        ir = light.readIR();
        uv = light.readUV();
    }
    
    if (isnan(temperature)) {
        temperature = 0.0;
    }
    
    if (isnan(humidity)) {
        humidity = 0.0;
    }
    
    
    char latlon_array[30];
    for (int i = 0; i < 30; i++) {
        latlon_array[i] = 0;
    }
    latlon.toCharArray(latlon_array, 30);
    
	//pack data into JSON
	publish_json = String::format("{\"t\":%3.2f,\"h\":%3.2f,\"vis\":%.0f,\"ir\":%.0f,\"uv\":%.0f,\"bat\":%.0f,\"gps\":\"%s\"}", temperature, humidity, visible, ir, uv, battery, latlon_array);


	//print to Serial port
	Serial.print("Bytes: ");
	Serial.println(publish_json.length());
	Serial.print("JSON: " );
	Serial.println(publish_json);

	//publish event (picked up by webhook, which will send data on to Google Sheet)
    Particle.publish("geotreetnrh", publish_json, PRIVATE); 
}

void blink() {
    //quick blink to show we are sending data
    digitalWrite(led, HIGH);
    delay(100);
    digitalWrite(led, LOW);
    delay(100);
}

void setup() {
    Serial.begin(9600);
    pinMode(led, OUTPUT);
    
    tracker.begin();

    // Enable the GPS module. Defaults to off to save power.
    // Takes 1.5s or so because of delays.
    tracker.gpsOn();
    
    tracker.updateGPS();



    // Set cellular ping for keepalive to the same as the sleep/wake cycle
    Particle.keepAlive(sleep_duration);
    
    uvstatus = light.begin();
    

}

void loop() {
    
    if (uvstatus) {
    	blink();
    	send_reading();
    	blink();        
    } else {
        // Sensors didn't init properly!
        blink();
        blink();
        blink();
        blink();
        blink();
    }
    
    
	delay(sleep_duration);
//	System.sleep(SLEEP_, sleep_duration, SLEEP_NETWORK_STANDBY);

}
