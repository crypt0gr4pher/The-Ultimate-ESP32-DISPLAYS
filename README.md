The Ultimate ESP32 DISPLAYS
Basic Details
Team Name: DeadPixels
Team Members
Team Lead: kurias tenson - TocH


Member 2: sreehari nandanan - TocH


Project Description
An ESP32-powered dual-display system that combines mesmerizing OLED animations with brutal tech roasts displayed on an LCD, all controlled via a web interface. Because who doesn't need personalized tech insults every 60 seconds?
The Problem (that doesn't exist)
People are getting too confident about their coding skills and need regular reality checks. Additionally, there's a severe shortage of animated entertainment combined with savage tech humor in embedded systems. How will developers maintain their healthy levels of self-doubt without constant roasting?
The Solution (that nobody asked for)
Introducing RoastBot - a sophisticated ESP32 system that displays 13 different mesmerizing animations on an OLED screen while simultaneously delivering brutal tech roasts on an LCD display. It features real-time NTP synchronization for accurate timestamps of your digital humiliation, web-based animation control, and a carefully curated collection of programmer-targeted insults that rotate every minute.
Technical Details
Technologies/Components Used
For Hardware:
ESP32 microcontroller


SH1106 128x64 OLED Display


16x4 I2C LCD Display (0x27 address)


Breadboard and jumper wires


Power supply (5V/3.3V)


For Software:
Languages: C++ (Arduino IDE), HTML, CSS, JavaScript


Frameworks: Arduino Core for ESP32, Web Server


Libraries:


U8g2lib (OLED graphics)


ArduinoJson (API communication)


LiquidCrystal_I2C (LCD control)


WiFi (network connectivity)


WebServer (HTTP server)


Tools: Arduino IDE, Web browser for control interface


Implementation
Installation
bash
# Install required libraries in Arduino IDE:
# 1. U8g2lib by oliver
# 2. ArduinoJson by Benoit Blanchon
# 3. LiquidCrystal I2C by Frank de Brabander

# Hardware connections:
# OLED: VCC->3.3V, GND->GND, SCL->GPIO22, SDA->GPIO21
# LCD: VCC->5V, GND->GND, SCL->GPIO22, SDA->GPIO21 (shared I2C)

Run
bash
# 1. Upload the code to ESP32
# 2. Connect to WiFi network "ESP" with password "sreehari"
# 3. Access web interface via ESP32's IP address
# 4. Control animations and settings through browser

Project Documentation
Screenshots (Add at least 3)
<img width="1000" height="1000" alt="OLED Display Animation 1" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/WhatsApp%20Image%202025-08-15%20at%2005.52.16_e364b519.jpg" />

<img width="1000" height="1000" alt="OLED Display Animation 2" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/WhatsApp%20Image%202025-08-15%20at%2005.52.16_73ea90ed.jpg" />

<img width="1000" height="1000" alt="OLED Display Animation 3" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/WhatsApp%20Image%202025-08-15%20at%2005.52.15_1577f9fa.jpg" />

Diagrams
<img width="1000" height="1000" alt="System Diagram" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/generated-image.png" />

Schematic & Circuit
<img width="1000" height="1000" alt="Circuit Schematic" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/image.png" />


Build Photos
<!-- Build Photos -->
<img width="1000" height="1000" alt="Build Setup 1" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0022.jpg" />
<img width="1000" height="1000" alt="Build Setup 2" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0021.jpg" />
<img width="1000" height="1000" alt="Circuit Closeup 1" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0020.jpg" />
<img width="1000" height="1000" alt="Circuit Closeup 2" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0019.jpg" />
<img width="1000" height="1000" alt="Display Working 1" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0018.jpg" />
<img width="1000" height="1000" alt="Display Working 2" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0017.jpg" />
<img width="1000" height="1000" alt="Full Assembly 1" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0016.jpg" />
<img width="1000" height="1000" alt="Full Assembly 2" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0015.jpg" />
<img width="1000" height="1000" alt="Component Layout" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/IMG-20250815-WA0014.jpg" />
<img width="1000" height="1000" alt="ESP32 Dual Display Setup" src="https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/WhatsApp%20Image%202025-08-15%20at%2007.25.28_fe6285f2.jpg" />

Project Demo
Video
https://github.com/crypt0gr4pher/The-Ultimate-ESP32-DISPLAYS/blob/main/WhatsApp%20Video%202025-08-15%20at%2006.53.58_73bcf05e.mp4

Additional Demos
Live web interface demonstration at [ESP32 IP Address]


Animation showcase video showing all effects


Roast compilation displaying the brutal tech humor collection


Team Contributions
Sreehari nandanan: ESP32 programming, OLED animation development, web server implementation, and system integration
Kurias tenson: LCD display programming, roast content creation, NTP time synchronization, and user interface design




