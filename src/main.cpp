#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ===== WIFI SETTINGS =====
const char* ssid = "SINTEKINDO LT 2";
const char* password = "sinarteknologiindonesia";

// ===== MQTT SETTINGS =====
const char* mqtt_server = "157.66.208.78";
const int mqtt_port = 1883;
const char* mqtt_topic = "motor_robot";

// ===== L298N GPIO SETTINGS =====
const int MotorKiri_IN1 = 18; 
const int MotorKiri_IN2 = 19; 
const int MotorKanan_IN3 = 23; 
const int MotorKanan_IN4 = 5; 

const int LED_SEIN_KIRI = 33;   // LED Kuning Kiri
const int LED_SEIN_KANAN = 32;  // LED Kuning Kanan

WiFiClient espClient;
PubSubClient client(espClient);

// ===== STATE MANAGEMENT =====
enum DriveMode { STOPPED, FORWARD, BACKWARD };
DriveMode currentMode = STOPPED;
bool isTurning = false;

// ===== LED SEIN STATE =====
bool seinKiriOn = false;   // Status LED Sein Kiri
bool seinKananOn = false;  // Status LED Sein Kanan

unsigned long lastBlinkTime = 0;
bool blinkState = false;
const int BLINK_INTERVAL = 399; 

// ===== FUNGSI LED CONTROL =====
void toggle_sein_kiri() {
  seinKiriOn = !seinKiriOn;  // Toggle state
  Serial.println(seinKiriOn ? "💡 Sein Kiri: ON (Kedip)" : "💡 Sein Kiri: OFF");
  
  // Jika dimatikan, matikan LED langsung
  if (!seinKiriOn) {
    digitalWrite(LED_SEIN_KIRI, LOW);
  }
}

void toggle_sein_kanan() {
  seinKananOn = !seinKananOn;  // Toggle state
  Serial.println(seinKananOn ? "💡 Sein Kanan: ON (Kedip)" : "💡 Sein Kanan: OFF");
  
  // Jika dimatikan, matikan LED langsung
  if (!seinKananOn) {
    digitalWrite(LED_SEIN_KANAN, LOW);
  }
}
void sein_off_all() {
  seinKiriOn = false;
  seinKananOn = false;
  digitalWrite(LED_SEIN_KIRI, LOW);
  digitalWrite(LED_SEIN_KANAN, LOW);
  Serial.println("💡 Semua Sein: OFF");
}

// ===== FUNGSI KONTROL GERAKAN =====

void motor_maju() {
  digitalWrite(MotorKiri_IN1, HIGH);
  digitalWrite(MotorKiri_IN2, LOW);
  digitalWrite(MotorKanan_IN3, HIGH);
  digitalWrite(MotorKanan_IN4, LOW); 
  Serial.println("🚗 Motor MAJU");
}

void motor_mundur() {
  digitalWrite(MotorKiri_IN1, LOW);
  digitalWrite(MotorKiri_IN2, HIGH);
  digitalWrite(MotorKanan_IN3, LOW);
  digitalWrite(MotorKanan_IN4, HIGH);
  Serial.println("🏃 Motor MUNDUR");
}

void motor_berhenti() {
  digitalWrite(MotorKiri_IN1, LOW);
  digitalWrite(MotorKiri_IN2, LOW);
  digitalWrite(MotorKanan_IN3, LOW);
  digitalWrite(MotorKanan_IN4, LOW);
  Serial.println("🛑 Motor BERHENTI");
}

void motor_belok_kanan() {
  // Motor Kiri MAJU/MUNDUR (tergantung mode), Motor Kanan BERHENTI
  if (currentMode == FORWARD) {
    digitalWrite(MotorKiri_IN1, HIGH);
    digitalWrite(MotorKiri_IN2, LOW);
  } else if (currentMode == BACKWARD) {
    digitalWrite(MotorKiri_IN1, LOW);
    digitalWrite(MotorKiri_IN2, HIGH);
  }
  
  digitalWrite(MotorKanan_IN3, LOW);
  digitalWrite(MotorKanan_IN4, LOW);
  
  Serial.println("↪️ BELOK KANAN");
}

void motor_belok_kiri() {
  // Motor Kanan MAJU/MUNDUR (tergantung mode), Motor Kiri BERHENTI
  digitalWrite(MotorKiri_IN1, LOW);
  digitalWrite(MotorKiri_IN2, LOW);
  
  if (currentMode == FORWARD) {
    digitalWrite(MotorKanan_IN3, HIGH);
    digitalWrite(MotorKanan_IN4, LOW);
  } else if (currentMode == BACKWARD) {
    digitalWrite(MotorKanan_IN3, LOW);
    digitalWrite(MotorKanan_IN4, HIGH);
  }
  
  Serial.println("↩️ BELOK KIRI");
}

void restoreMode() {
  // Kembali ke mode sebelumnya setelah belok
  if (currentMode == FORWARD) {
    motor_maju();
  } else if (currentMode == BACKWARD) {
    motor_mundur();
  } else {
    motor_berhenti();
  }
}

// ===== SETUP WIFI =====
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// ===== MQTT CALLBACK =====
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  // Kontrol LED Sein (Toggle Manual)
  if (message == "sein_kiri") {
    toggle_sein_kiri();
    
  } else if (message == "sein_kanan") {
    toggle_sein_kanan();
    
  } else if (message == "sein_off") {
    sein_off_all();
  
  // Kontrol Mode Utama (Persistent)
  } else if (message == "maju") {
    currentMode = FORWARD;
    isTurning = false;
    motor_maju();
    
  } else if (message == "mundur") {
    currentMode = BACKWARD;
    isTurning = false;
    motor_mundur();
    
  } else if (message == "berhenti") {
    currentMode = STOPPED;
    isTurning = false;
    motor_berhenti();
  
  // Kontrol Belok (Temporary - Press)
  } else if (message == "kanan_press") {
    if (currentMode != STOPPED) {
      isTurning = true;
      motor_belok_kanan();
    }
    
  } else if (message == "kiri_press") {
    if (currentMode != STOPPED) {
      isTurning = true;
      motor_belok_kiri();
    }
  
  // Kontrol Belok (Release - Kembali ke mode)
  } else if (message == "kanan_release" || message == "kiri_release") {
    if (isTurning) {
      isTurning = false;
      restoreMode();
    }
    
  } else {
    Serial.println("❌ Perintah tidak dikenali");
  }
}

// ===== RECONNECT MQTT =====
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP32MotorClient-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
      Serial.println("Subscribed to: " + String(mqtt_topic));
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== ESP32 MQTT MOTOR + LED SEIN CONTROL ===");
  
  pinMode(MotorKiri_IN1, OUTPUT);
  pinMode(MotorKiri_IN2, OUTPUT);
  pinMode(MotorKanan_IN3, OUTPUT);
  pinMode(MotorKanan_IN4, OUTPUT);
  
  // Setup LED Sein
  pinMode(LED_SEIN_KIRI, OUTPUT);
  pinMode(LED_SEIN_KANAN, OUTPUT);
  
  motor_berhenti();
  sein_off_all();
  
  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  Serial.println("=== SETUP COMPLETE ===");
}

// ===== LOOP =====
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
   unsigned long currentMillis = millis();
  
  // Cek apakah sudah waktunya untuk toggle LED
  if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentMillis;
    blinkState = !blinkState;  // Toggle state (ON/OFF)
    
    // Sein Kiri - Kedip jika aktif
    if (seinKiriOn) {
      digitalWrite(LED_SEIN_KIRI, blinkState ? HIGH : LOW);
    }
    
    // Sein Kanan - Kedip jika aktif
    if (seinKananOn) {
      digitalWrite(LED_SEIN_KANAN, blinkState ? HIGH : LOW);
    }
  }
}