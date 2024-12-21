#include<Adafruit_MQTT.h>
#include<Adafruit_MQTT_Client.h>
#include<Wifi.h>  

#include<dd_led/dd_led.h>
#include<dd_thermistor/dd_thermistor.h>

#include<string.h>

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "username"
#define AIO_KEY "key"

#define LED_PIN 4
#define THERMISTOR_PIN 10

const char* ssid = "Your_SSID";       // Replace with your Wi-Fi network name
const char* password = "Your_PASSWORD"; // Replace with your Wi-Fi password

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe led_on_off_subscription = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledOnOff");
Adafruit_MQTT_Publish temperature_publisher = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");


void MQTT_connect();
void process_led_message(char *message);

Led led(LED_PIN);
Thermistor thermistor(THERMISTOR_PIN);

SemaphoreHandle_t xMutex;


void led_on_off_subscription_task(void *params){

  mqtt.subscribe(&led_on_off_subscription);

  Serial.println("Waiting for retained message...");
  
  mqtt.processPackets(5000); 

  if (led_on_off_subscription.lastread != nullptr) {
    process_led_message((char *)led_on_off_subscription.lastread);
  } else {
    Serial.println("No retained message found.");
  }

  Adafruit_MQTT_Subscribe *subscription;

  while(1){
    
    if (!mqtt.connected()) {
      MQTT_connect();
    }

    if((subscription = mqtt.readSubscription(200))) {
      if (subscription == &led_on_off_subscription) {
        process_led_message((char *)led_on_off_subscription.lastread);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }

}

void process_led_message(char *message){
  if(strcmp(message, "on") == 0){
    dd_led_turn_on(&led);
    Serial.println("Led on");
  }
  else if(strcmp((char *)led_on_off_subscription.lastread, "off") == 0){
    dd_led_turn_off(&led);
    Serial.println("Led off");
  }
  else 
    Serial.println("Invalid value");
}


void temperature_publisher_task(void *params){

   while(1){

    if (!mqtt.connected()) {
      MQTT_connect();
    }
    
    if (!temperature_publisher.publish((int)dd_thermistor_get_temp_C(&thermistor))) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }

    vTaskDelay(pdMS_TO_TICKS(60000));
  }
}


void setup() {
  
  xMutex = xSemaphoreCreateMutex();
  
  Serial.begin(115200); // Initialize serial communication
  delay(10);

  Serial.println();
  Serial.println("Connecting to Wi-Fi...");

  WiFi.begin(ssid, password); // Start Wi-Fi connection

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.println("IP address: ");

  Serial.println(WiFi.localIP()); // Print the IP address
  
  xTaskCreate(led_on_off_subscription_task, "Led on off Task", 2048, NULL, 1, NULL);
  xTaskCreate(temperature_publisher_task, "Temperature publisher Task", 2048, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {

}

void MQTT_connect() {
  if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
    int8_t ret;

    Serial.print("Connecting to MQTT... ");

    uint8_t retries = 3;

    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        mqtt.disconnect();
        vTaskDelay(pdMS_TO_TICKS(5000));  // wait 5 seconds
        retries--;

        if (retries == 0) {
          // basically die and wait for WDT to reset me
          while (1);
        }
    }

    Serial.println("MQTT Connected!");

    xSemaphoreGive(xMutex);
  }
}