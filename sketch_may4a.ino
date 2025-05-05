#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi & MQTT
const char* ssid = "iPhone (8)";
const char* password = "palacios";
const char* mqtt_server = "34.xxx.xxx.xxx"; // Change here

WiFiClient espClient;
PubSubClient client(espClient);

// LCD at I2C address 0x27 (commonly used for 16x2 displays)
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Global variables at the top of your sketch
int p1Wins = 0;
int p1Losses = 0;
int draws = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  lcd.clear();

  if (strcmp(topic, "game/result") == 0) {
    if (message.indexOf("Player 1 wins") >= 0) {
      p1Wins++;
    } else if (message.indexOf("Player 2 wins") >= 0) {
      p1Losses++;
    } else if (message.indexOf("Draw") >= 0 || message.indexOf("draw") >= 0) {
      draws++;
    }

    // Display stats
    lcd.setCursor(0, 0);
    lcd.print("W:");
    lcd.print(p1Wins);
    lcd.print(" L:");
    lcd.print(p1Losses);

    lcd.setCursor(0, 1);
    lcd.print("Draws:");
    lcd.print(draws);

  } else if (strcmp(topic, "player1/move") == 0) {
    lcd.setCursor(0, 0);
    lcd.print("P1 moved:");
    lcd.setCursor(0, 1);
    lcd.print(message);
  } else if (strcmp(topic, "player2/move") == 0) {
    lcd.setCursor(0, 0);
    lcd.print("P2 moved:");
    lcd.setCursor(0, 1);
    lcd.print(message);
  }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe("player1/move");
      client.subscribe("player2/move");
      client.subscribe("game/result");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
