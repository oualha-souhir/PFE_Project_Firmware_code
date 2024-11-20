ESP IDF firmware deployed on ESP Wrover Kit for EV Charging Station Management.

The firmware integrates RFID-based access control, IoT Hub communication, WiFi connectivity, sensor monitoring, and LCD-based user interface to streamline EV charging station operations.

**Features**
- WiFi Connectivity
- Azure IoT Hub Integration:
Communicates with the Azure IoT Hub for real-time monitoring and data exchange.
- RFID Authentication:
Uses an RC522 RFID module for user authentication.
- LCD Display
- FreeRTOS Tasks:
Implements tasks for sending/receiving data and controlling operations concurrently.

**Prerequisites**

- Hardware:
  ESP Wrover Kit (ESP32)
  RC522 RFID module
  Sensors (current: ACS712, voltage: zmpt101b)
  LCD Display
  Reliable internet access
- Software:
  ESP-IDF (v5.2.2)
  Microsoft Azure Account
  Azure IoT Hub credentials
  Azure Function
  Azure Stream Analytics
  Web Application( https://github.com/oualha-souhir/PFE_Project_Web_App )
  

**Environment Setup:**
Install ESP-IDF and configure the development environment.
Set up Azure IoT Hub and generate a device connection string.
