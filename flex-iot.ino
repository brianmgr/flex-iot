#include <math.h>

// Modem software serial connection
#include <SoftwareSerial.h>
#include "SIM800Twilio.hpp"
#define RX_PIN 10
#define TX_PIN 11 
#define MODEM_BAUD 9600
SIM800Twilio modem(RX_PIN, TX_PIN, MODEM_BAUD);

// Temperature and humidity sensor
#include "DHT.h"
#define DHTPIN 6 // Sensor data pin
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// IC2 SSD1306 OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH  128


// Pop text onto OLED
void popText(String string) {
  display.invertDisplay(true);
  display.setTextSize(2);
  display.setTextColor(WHITE);  
  display.setCursor(18,9);
  display.clearDisplay();
  display.println(string);
  display.display();
  display.invertDisplay(false);
  delay(1000);
}

void setup() 
{
  // Initialize serial monitor
  Serial.begin(115200);
  
  // Initialize software serial to modem
  modem.begin();
  
  // Initialize sensor
  dht.begin();

  // Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.display();
  
}

void loop() {

  delay(300);
  
  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity(); // Humidity
  float f = dht.readTemperature(true); // Temp Fahrenheit (isFahrenheit = true)
  float hif = dht.computeHeatIndex(f, h); // Feels like

  // Display text
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5,8);
  display.println("Temperature: " + String(f) + "f");
  display.setCursor(5,18);
  display.println("Feels like:  " + String(hif) + "f");
  display.display();

  delay(700);

  Serial.println("Temp: " + String(f));
  
  // Check for overheat, send command if so
  if (f > float(100)){
    Serial.println("Triggered: " + String(f));
    String command = "Overheat detected!";
    for (int i=0; i<3; ++i){
      popText("OVERHEAT");
      popText("REPORTED");
    }
    modem.send_command(command);
    // Delay to lessen amount of alerts
    delay(10000);
  }
}
