#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

// Network credentials
const char *SSID = "ESP8266-AP";
const char *PSWD = "ASAPBABY";

// Set LED PIN
const int LED_PIN = D1;

// DHT sensor
#include <DHT.h>
const int DHT_PIN = D2;
DHT dht(DHT_PIN, DHT22);
float temperature;
char temp_buf[8];
float humidity;
char humi_buf[8];

/*******************
   Step Motor
*/
#include <Stepper.h>

#define STEPS 2048  // 32 steps * gear reduction ratio 64
#define IN1 D3      // DO NOT PLUG WHEN BOOT
#define IN2 D4      // DO NOT PLUG WHEN BOOT
#define IN3 D5
#define IN4 D6
Stepper stepper(STEPS, IN1, IN3, IN2, IN4);

const int step_speed = 15;  // [5, 15]
int step_num = STEPS / 12;
int step_delay = 200;
int step_dir = 1;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


/******************************** 
 *  Callback
 */
void on_root_request(AsyncWebServerRequest *request);
void on_css_request(AsyncWebServerRequest *request);
void on_script_request(AsyncWebServerRequest *request);
void on_img_on_request(AsyncWebServerRequest *request);
void on_img_off_request(AsyncWebServerRequest *request);
void on_img_temperature_request(AsyncWebServerRequest *request);
void on_img_humidity_request(AsyncWebServerRequest *request);
void on_turn_on_led_request(AsyncWebServerRequest *request);
void on_turn_off_led_request(AsyncWebServerRequest *request);
void on_led_state_request(AsyncWebServerRequest *request);
void on_temperature_request(AsyncWebServerRequest *request);
void on_humidity_request(AsyncWebServerRequest *request);
void on_rotate_request(AsyncWebServerRequest *request);

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  //  DHT sensor
  pinMode(DHT_PIN, INPUT_PULLUP);
  dht.begin();

  // Step motor
  stepper.setSpeed(step_speed);

  // initializing SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Error while mounting SPIFFS");
    while (1)
      ;
  }

  // WiFi softAP
  WiFi.softAP(SSID, PSWD);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("http://");
  Serial.print(myIP);
  Serial.println("/");

  server.on("/", HTTP_GET, on_root_request);
  server.on("/style.css", HTTP_GET, on_css_request);
  server.on("/script.js", HTTP_GET, on_script_request);
  server.on("/images/on.png", HTTP_GET, on_img_on_request);
  server.on("/images/off.png", HTTP_GET, on_img_off_request);
  server.on("/images/temperature.png", HTTP_GET, on_img_temperature_request);
  server.on("/images/humidity.png", HTTP_GET, on_img_humidity_request);
  server.on("/led_on", HTTP_GET, on_turn_on_led_request);
  server.on("/led_off", HTTP_GET, on_turn_off_led_request);
  server.on("/led_state", HTTP_GET, on_led_state_request);
  server.on("/temperature", HTTP_GET, on_temperature_request);
  server.on("/humidity", HTTP_GET, on_humidity_request);
  server.on("/rotate", HTTP_GET, on_rotate_request);

  // Start server
  server.begin();
}

void loop() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature)) temperature = 0.0;
  if (isnan(humidity)) humidity = 0.0;
  delay(200);
  stepper.step(step_num * step_dir);
  delay(step_delay);
}
/*********************************************
    Callback function
*/
void on_root_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html");
}

void on_css_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/style.css", "text/css");
}
void on_script_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/script.js", "text/javascript");
}
void on_img_on_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/images/on.png", "image/png");
}
void on_img_off_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/images/off.png", "image/png");
}
void on_img_temperature_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/images/temperature.png", "image/png");
}
void on_img_humidity_request(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/images/humidity.png", "image/png");
}

void on_turn_on_led_request(AsyncWebServerRequest *request) {
  digitalWrite(LED_PIN, HIGH);
  request->send(200, "text/plain", "LED turned on");
}

void on_turn_off_led_request(AsyncWebServerRequest *request) {
  digitalWrite(LED_PIN, LOW);
  request->send(200, "text/plain", "LED turned off");
}

void on_led_state_request(AsyncWebServerRequest *request) {
  request->send(200, "text/plain", digitalRead(LED_PIN) ? "HIGH" : "LOW");
}

void on_temperature_request(AsyncWebServerRequest *request) {
  dtostrf(temperature, 6, 2, temp_buf);
  request->send(200, "text/plain", temp_buf);
}

void on_humidity_request(AsyncWebServerRequest *request) {
  dtostrf(humidity, 6, 2, humi_buf);
  request->send(200, "text/plain", humi_buf);
}

void on_rotate_request(AsyncWebServerRequest *request) {
  if (request->hasParam("degree") && request->hasParam("ccw")) {
    request->send(200, "text/plain", "Received successed!");
    int degree = request->getParam("degree")->value().toInt();
    step_num = degree / 360.0 * STEPS;
    String ccw = request->getParam("ccw")->value();
    if (ccw == "true") {
      step_dir = -1;
    } else {
      step_dir = 1;
    }
  } else {
    request->send(400, "text/plain", "Missing degree and ccw parameter");
    Serial.println("Missing degree and ccw parameter");
  }
}
