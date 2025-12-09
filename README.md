# 🤖 ESP32 Robot Controller with MQTT

Project robot berbasis ESP32 yang dapat dikontrol melalui web interface menggunakan protokol MQTT. Robot ini dilengkapi dengan sistem LED sein dan kontrol motor dual driver L298N.

## ✨ Fitur

- 🚗 **Kontrol Gerakan**: Maju, mundur, belok kiri, dan belok kanan
- 💡 **LED Sein**: LED indikator kiri dan kanan dengan efek kedip
- 🌐 **Web Controller**: Interface web responsif untuk kontrol robot
- 📡 **MQTT Protocol**: Komunikasi real-time menggunakan MQTT broker
- 🎮 **Touch Control**: Mendukung kontrol touch untuk perangkat mobile
- 🔄 **State Management**: Sistem manajemen state untuk gerakan yang smooth

## 🛠️ Hardware yang Dibutuhkan

- ESP32 Development Board
- Motor Driver L298N
- 2x DC Motor
- 2x LED (untuk sein kiri dan kanan)
- Power Supply (untuk motor)
- Kabel jumper
- Chassis robot (opsional)

## 📌 Pin Configuration

### Motor Driver (L298N)
- **Motor Kiri IN1**: GPIO 18
- **Motor Kiri IN2**: GPIO 19
- **Motor Kanan IN3**: GPIO 23
- **Motor Kanan IN4**: GPIO 5

### LED Sein
- **LED Sein Kiri**: GPIO 33
- **LED Sein Kanan**: GPIO 32

## 🔧 Setup dan Instalasi

### 1. Prerequisites
- [PlatformIO IDE](https://platformio.org/) atau [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html)
- MQTT Broker (bisa menggunakan broker public atau private)

### 2. Clone Repository
```bash
git clone https://github.com/Donidiatmika/robot_controller.git
cd robot_controller
```

### 3. Konfigurasi WiFi dan MQTT
Edit file `src/main.cpp` dan sesuaikan dengan setting Anda:

```cpp
// WIFI SETTINGS
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT SETTINGS
const char* mqtt_server = "YOUR_MQTT_BROKER_IP";
const int mqtt_port = 1883;
const char* mqtt_topic = "motor_robot";
```

### 4. Upload ke ESP32
```bash
# Build project
pio run

# Upload ke ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

## 🎮 Cara Menggunakan

### 1. Web Controller
1. Buka file `html-robot-controller/controller.html` di browser
2. Masukkan alamat MQTT broker Anda
3. Klik "Connect" untuk menghubungkan ke broker
4. Gunakan tombol kontrol untuk menggerakkan robot

### 2. MQTT Commands
Publish pesan ke topic `motor_robot`:

| Command | Fungsi |
|---------|--------|
| `maju` | Robot bergerak maju |
| `mundur` | Robot bergerak mundur |
| `berhenti` | Robot berhenti |
| `kanan_press` | Belok kanan (saat ditekan) |
| `kanan_release` | Kembali ke mode sebelumnya |
| `kiri_press` | Belok kiri (saat ditekan) |
| `kiri_release` | Kembali ke mode sebelumnya |
| `sein_kiri` | Toggle LED sein kiri |
| `sein_kanan` | Toggle LED sein kanan |
| `sein_off` | Matikan semua LED sein |

## 📡 MQTT Topic Structure

**Subscribe Topic**: `motor_robot`
- ESP32 mendengarkan perintah dari topic ini

## 🔌 Wiring Diagram

```
ESP32          L298N
GPIO 18  ----> IN1 (Motor Kiri)
GPIO 19  ----> IN2 (Motor Kiri)
GPIO 23  ----> IN3 (Motor Kanan)
GPIO 5   ----> IN4 (Motor Kanan)

ESP32          LED
GPIO 33  ----> LED Sein Kiri (+)
GPIO 32  ----> LED Sein Kanan (+)
GND      ----> LED (-) melalui resistor
```

## 📚 Dependencies

Project ini menggunakan library berikut:
- `PubSubClient@^2.8` - MQTT client library
- `WiFi` - Built-in ESP32 WiFi library
- `Arduino Framework` - Core framework

## 🚀 Pengembangan Lebih Lanjut

Ide untuk pengembangan:
- [ ] Tambahkan sensor ultrasonik untuk obstacle avoidance
- [ ] Implementasi PWM untuk kontrol kecepatan motor
- [ ] Tambahkan kamera untuk video streaming
- [ ] Integrasi dengan aplikasi mobile native
- [ ] Tambahkan mode autonomous

## 🐛 Troubleshooting

### Robot tidak bergerak
- Cek koneksi motor driver L298N
- Pastikan power supply motor cukup
- Verifikasi pin GPIO yang digunakan

### Tidak bisa connect ke MQTT
- Pastikan ESP32 terhubung ke WiFi
- Cek alamat MQTT broker sudah benar
- Verifikasi port MQTT (default: 1883)

### LED sein tidak kedip
- Cek koneksi LED dan resistor
- Pastikan GPIO 32 dan 33 tidak digunakan untuk keperluan lain

## 📄 License

Project ini dibuat untuk keperluan pembelajaran dan pengembangan.

## 👨‍💻 Author

**Doni Diatmika**
- GitHub: [@Donidiatmika](https://github.com/Donidiatmika)

## 🙏 Acknowledgments

- PlatformIO Team
- Arduino ESP32 Community
- MQTT.js Library Contributors

---

⭐ Jika project ini bermanfaat, jangan lupa berikan star!
