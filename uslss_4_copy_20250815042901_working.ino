/*
ESP32 SH110x OLED Animation Controller with Enhanced Animations
Connections: VCC->3.3V, GND->GND, SCL->GPIO22, SDA->GPIO21
Required Libraries:
- U8g2lib (install via Library Manager)
- ArduinoJson (install via Library Manager)
- WiFi (built-in)
- WebServer (built-in)
*/

#include <WiFi.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <math.h>

// OLED Display Setup (SH1106 128x64)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// WiFi Credentials
const char* ssid = "ESP";
const char* password = "sreehari";

// Web Server on port 80
WebServer server(80);

// Animation Variables
String currentAnimation = "bouncing_ball";
int animationSpeed = 50;
int brightness = 80;
unsigned long lastAnimationTime = 0;

// Animation States
struct AnimationState {
  // Bouncing Ball
  int ballX = 64, ballY = 32;
  int ballVelX = 2, ballVelY = 1;
  
  // Matrix Rain
  int matrixCols[16];
  int matrixSpeed[16];
  
  // Wave Pattern
  float wavePhase = 0;
  
  // Spiral
  int spiralAngle = 0;
  
  // Fireworks
  struct Particle {
    float x, y, vx, vy;
    bool active;
  } particles[15];
  bool fireworksInit = false;
  
  // Snake
  int snakeX[50], snakeY[50];
  int snakeLen = 5;
  int snakeDir = 0; // 0=right, 1=down, 2=left, 3=up
  int foodX = 100, foodY = 40;
  unsigned long snakeTime = 0;
  
  // Clock
  unsigned long clockStart = 0;
  
  // Spectrum
  int spectrumBars[16];
  
  // Aquarium Animation
  int fishX = 40, fishY = 30;
  int fishDir = 1; // 1 = right, -1 = left
  struct AquariumFood {
    int x, y;
    bool active;
  } aquariumFood[5];
  struct Bubble {
    int x, y;
  } bubbles[4];
  int plantOffset = 0;
  
  // Dino Game Animation
  int dinoX = 10, dinoY = 48;
  bool isJumping = false;
  int jumpVelocity = 0;
  int cactusX = 128;
  int cactusY = 44;
  int dinoScore = 0;
  bool gameOver = false;
  unsigned long lastJump = 0;
  
  // RoboEyes Animation
  int eyeSize = 20;
  int leftEyeX = 30, rightEyeX = 90;
  int eyeY = 32;
  int pupilOffsetX = 0, pupilOffsetY = 0;
  bool blinking = false;
  unsigned long blinkTime = 0;
  int mood = 0; // 0=normal, 1=happy, 2=angry
  unsigned long moodChangeTime = 0;
} animState;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 OLED Animation Controller Starting...");
  
  // Initialize OLED
  u8g2.begin();
  u8g2.setContrast(brightness * 2.55);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 15, "ESP32 OLED");
  u8g2.drawStr(0, 30, "Animation Controller");
  u8g2.drawStr(0, 45, "Starting WiFi...");
  u8g2.sendBuffer();
  
  // Initialize animation states
  initializeAnimations();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    // Show IP Address on OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 15, "WiFi Connected!");
    String ipStr = WiFi.localIP().toString();
    u8g2.drawStr(0, 30, "IP Address:");
    u8g2.drawStr(0, 45, ipStr.c_str());
    u8g2.drawStr(0, 60, "Ready for control!");
    u8g2.sendBuffer();
    
    Serial.println();
    Serial.println("WiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 20, "WiFi Failed!");
    u8g2.drawStr(0, 40, "Check credentials");
    u8g2.sendBuffer();
    Serial.println("WiFi connection failed!");
  }
  
  // Setup web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/animation", HTTP_POST, handleAnimationChange);
  server.on("/api/settings", HTTP_POST, handleSettingsChange);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/styles.css", HTTP_GET, handleCSS);
  
  server.begin();
  Serial.println("HTTP server started");
  
  delay(3000); // Show IP for 3 seconds
  animState.clockStart = millis();
}

void loop() {
  server.handleClient();
  
  if (millis() - lastAnimationTime > animationSpeed) {
    runAnimation();
    lastAnimationTime = millis();
  }
}

void initializeAnimations() {
  // Initialize matrix rain
  for (int i = 0; i < 16; i++) {
    animState.matrixCols[i] = random(-10, 0);
    animState.matrixSpeed[i] = random(1, 4);
  }
  
  // Initialize snake
  for (int i = 0; i < animState.snakeLen; i++) {
    animState.snakeX[i] = 30 - i * 4;
    animState.snakeY[i] = 32;
  }
  
  // Initialize spectrum
  for (int i = 0; i < 16; i++) {
    animState.spectrumBars[i] = random(5, 40);
  }
  
  // Initialize aquarium
  for (int i = 0; i < 4; i++) {
    animState.bubbles[i].x = random(0, 128);
    animState.bubbles[i].y = random(64);
  }
  for (int i = 0; i < 5; i++) {
    animState.aquariumFood[i].active = false;
  }
  
  // Initialize dino game
  animState.dinoX = 10;
  animState.dinoY = 48;
  animState.cactusX = 128;
  animState.dinoScore = 0;
  animState.gameOver = false;
  
  // Initialize robo eyes
  animState.leftEyeX = 30;
  animState.rightEyeX = 90;
  animState.eyeY = 32;
  animState.blinkTime = millis();
  animState.moodChangeTime = millis();
}

void runAnimation() {
  u8g2.clearBuffer();
  
  if (currentAnimation == "bouncing_ball") {
    drawBouncingBall();
  } else if (currentAnimation == "matrix_rain") {
    drawMatrixRain();
  } else if (currentAnimation == "wave_pattern") {
    drawWavePattern();
  } else if (currentAnimation == "spiral") {
    drawSpiral();
  } else if (currentAnimation == "fireworks") {
    drawFireworks();
  } else if (currentAnimation == "snake") {
    drawSnake();
  } else if (currentAnimation == "clock") {
    drawClock();
  } else if (currentAnimation == "spectrum") {
    drawSpectrum();
  } else if (currentAnimation == "aquarium") {
    drawAquarium();
  } else if (currentAnimation == "dino_game") {
    drawDinoGame();
  } else if (currentAnimation == "robo_eyes") {
    drawRoboEyes();
  }
  
  u8g2.sendBuffer();
}

void drawBouncingBall() {
  // Update ball position
  animState.ballX += animState.ballVelX;
  animState.ballY += animState.ballVelY;
  
  // Bounce off walls
  if (animState.ballX <= 3 || animState.ballX >= 125) animState.ballVelX *= -1;
  if (animState.ballY <= 3 || animState.ballY >= 61) animState.ballVelY *= -1;
  
  // Constrain to screen
  animState.ballX = constrain(animState.ballX, 3, 125);
  animState.ballY = constrain(animState.ballY, 3, 61);
  
  // Draw ball with trail
  u8g2.drawCircle(animState.ballX, animState.ballY, 3);
  u8g2.drawDisc(animState.ballX, animState.ballY, 2);
  
  // Draw label
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Bouncing Ball");
}

void drawMatrixRain() {
  u8g2.setFont(u8g2_font_4x6_tr);
  
  for (int x = 0; x < 16; x++) {
    animState.matrixCols[x] += animState.matrixSpeed[x];
    
    if (animState.matrixCols[x] > 70) {
      animState.matrixCols[x] = random(-20, -5);
      animState.matrixSpeed[x] = random(1, 4);
    }
    
    // Draw falling characters
    for (int y = 0; y < animState.matrixCols[x] && y < 64; y += 6) {
      if (y >= 0 && y < 60) {
        char c = random(33, 126); // Random ASCII
        u8g2.drawGlyph(x * 8, y + 6, c);
      }
    }
  }
}

void drawWavePattern() {
  // Draw multiple sine waves
  for (int x = 0; x < 128; x++) {
    int y1 = 20 + sin((x + animState.wavePhase) * 0.1) * 15;
    int y2 = 44 + cos((x + animState.wavePhase * 0.7) * 0.08) * 12;
    
    u8g2.drawPixel(x, y1);
    u8g2.drawPixel(x, y2);
    
    // Add some sparkles
    if (random(100) < 5) {
      u8g2.drawPixel(x, random(10, 54));
    }
  }
  animState.wavePhase += 3;
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Wave Pattern");
}

void drawSpiral() {
  int centerX = 64, centerY = 32;
  
  for (int i = 0; i < 60; i++) {
    float angle = (animState.spiralAngle + i * 8) * PI / 180.0;
    float radius = i * 0.8;
    
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius * 0.6;
    
    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
      u8g2.drawPixel(x, y);
      
      // Add some glow effect
      if (i % 3 == 0) {
        u8g2.drawPixel(x+1, y);
        u8g2.drawPixel(x, y+1);
      }
    }
  }
  animState.spiralAngle += 4;
  if (animState.spiralAngle >= 360) animState.spiralAngle = 0;
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Spiral");
}

void drawFireworks() {
  if (!animState.fireworksInit) {
    // Initialize particles
    for (int i = 0; i < 15; i++) {
      animState.particles[i].x = 64;
      animState.particles[i].y = 32;
      animState.particles[i].vx = random(-30, 30) / 10.0;
      animState.particles[i].vy = random(-30, 30) / 10.0;
      animState.particles[i].active = true;
    }
    animState.fireworksInit = true;
  }
  
  // Update and draw particles
  for (int i = 0; i < 15; i++) {
    if (animState.particles[i].active) {
      animState.particles[i].x += animState.particles[i].vx;
      animState.particles[i].y += animState.particles[i].vy;
      animState.particles[i].vy += 0.1; // Gravity
      
      // Reset if out of bounds
      if (animState.particles[i].x < 0 || animState.particles[i].x > 128 || 
          animState.particles[i].y > 64) {
        animState.particles[i].x = 64;
        animState.particles[i].y = 32;
        animState.particles[i].vx = random(-30, 30) / 10.0;
        animState.particles[i].vy = random(-30, 30) / 10.0;
      }
      
      u8g2.drawPixel((int)animState.particles[i].x, (int)animState.particles[i].y);
      
      // Trail effect
      u8g2.drawPixel((int)animState.particles[i].x - animState.particles[i].vx, 
                     (int)animState.particles[i].y - animState.particles[i].vy);
    }
  }
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Fireworks");
}

void drawSnake() {
  // Simple snake game simulation
  if (millis() - animState.snakeTime > 200) {
    // Move snake head
    int newX = animState.snakeX[0];
    int newY = animState.snakeY[0];
    
    switch (animState.snakeDir) {
      case 0: newX += 4; break; // Right
      case 1: newY += 4; break; // Down
      case 2: newX -= 4; break; // Left
      case 3: newY -= 4; break; // Up
    }
    
    // Wrap around screen
    if (newX >= 128) newX = 0;
    if (newX < 0) newX = 124;
    if (newY >= 64) newY = 0;
    if (newY < 0) newY = 60;
    
    // Change direction randomly
    if (random(100) < 10) {
      animState.snakeDir = random(4);
    }
    
    // Move body
    for (int i = animState.snakeLen - 1; i > 0; i--) {
      animState.snakeX[i] = animState.snakeX[i-1];
      animState.snakeY[i] = animState.snakeY[i-1];
    }
    
    animState.snakeX[0] = newX;
    animState.snakeY[0] = newY;
    
    animState.snakeTime = millis();
  }
  
  // Draw snake
  for (int i = 0; i < animState.snakeLen; i++) {
    if (i == 0) {
      u8g2.drawBox(animState.snakeX[i], animState.snakeY[i], 3, 3); // Head
    } else {
      u8g2.drawPixel(animState.snakeX[i], animState.snakeY[i]); // Body
    }
  }
  
  // Draw food
  u8g2.drawPixel(animState.foodX, animState.foodY);
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Snake Game");
}

void drawClock() {
  unsigned long currentTime = (millis() - animState.clockStart) / 1000;
  int hours = (currentTime / 3600) % 24;
  int minutes = (currentTime / 60) % 60;
  int seconds = currentTime % 60;
  
  u8g2.setFont(u8g2_font_logisoso16_tr);
  
  // Format time string
  String timeStr = "";
  if (hours < 10) timeStr += "0";
  timeStr += String(hours) + ":";
  if (minutes < 10) timeStr += "0";
  timeStr += String(minutes) + ":";
  if (seconds < 10) timeStr += "0";
  timeStr += String(seconds);
  
  // Center the time
  int strWidth = u8g2.getStrWidth(timeStr.c_str());
  u8g2.drawStr((128 - strWidth) / 2, 35, timeStr.c_str());
  
  // Draw decorative elements
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(20, 8, "Digital Clock");
  
  // Blinking separator
  if ((millis() / 500) % 2) {
    u8g2.drawStr(45, 55, "ESP32 OLED");
  }
}

void drawSpectrum() {
  // Simulate audio spectrum
  for (int i = 0; i < 16; i++) {
    // Update bar heights with some randomness
    animState.spectrumBars[i] += random(-3, 4);
    animState.spectrumBars[i] = constrain(animState.spectrumBars[i], 2, 50);
    
    // Draw bars
    int barHeight = animState.spectrumBars[i];
    int x = i * 8;
    
    for (int y = 64 - barHeight; y < 64; y += 2) {
      u8g2.drawPixel(x, y);
      u8g2.drawPixel(x + 1, y);
      u8g2.drawPixel(x + 2, y);
    }
    
    // Peak indicators
    u8g2.drawPixel(x, 64 - barHeight);
    u8g2.drawPixel(x + 1, 64 - barHeight);
    u8g2.drawPixel(x + 2, 64 - barHeight);
  }
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Spectrum Analyzer");
}

void drawAquarium() {
  // Draw plants
  for (int i = 0; i < 4; i++) {
    int px = i * 30 + 5;
    for (int j = 0; j < 20; j++) {
      int py = 63 - j;
      if ((j + animState.plantOffset) % 4 < 2) {
        u8g2.drawPixel(px, py);
      } else {
        u8g2.drawPixel(px + 1, py);
      }
    }
  }
  animState.plantOffset = (animState.plantOffset + 1) % 4;
  
  // Draw bubbles
  for (int i = 0; i < 4; i++) {
    u8g2.drawCircle(animState.bubbles[i].x, animState.bubbles[i].y, 1);
    animState.bubbles[i].y--;
    if (animState.bubbles[i].y < 0) {
      animState.bubbles[i].y = 63;
      animState.bubbles[i].x = random(0, 128);
    }
  }
  
  // Fish movement
  animState.fishX += animState.fishDir;
  if (animState.fishX > 124) animState.fishDir = -1;
  if (animState.fishX < 4) animState.fishDir = 1;
  
  // Random food drops
  if (random(200) < 2) {
    for (int i = 0; i < 5; i++) {
      if (!animState.aquariumFood[i].active) {
        animState.aquariumFood[i].active = true;
        animState.aquariumFood[i].x = random(10, 118);
        animState.aquariumFood[i].y = 0;
        break;
      }
    }
  }
  
  // Update food
  for (int i = 0; i < 5; i++) {
    if (animState.aquariumFood[i].active) {
      animState.aquariumFood[i].y++;
      u8g2.drawBox(animState.aquariumFood[i].x, animState.aquariumFood[i].y, 2, 2);
      
      // Fish eats food
      if (abs(animState.fishX - animState.aquariumFood[i].x) < 5 && 
          abs(animState.fishY - animState.aquariumFood[i].y) < 5) {
        animState.aquariumFood[i].active = false;
      }
      if (animState.aquariumFood[i].y > 63) animState.aquariumFood[i].active = false;
    }
  }
  
  // Draw fish
  u8g2.drawCircle(animState.fishX, animState.fishY, 4); // body
  if (animState.fishDir == 1) {
    // Tail to the left
    u8g2.drawTriangle(animState.fishX - 4, animState.fishY, 
                      animState.fishX - 8, animState.fishY - 3, 
                      animState.fishX - 8, animState.fishY + 3);
    u8g2.drawPixel(animState.fishX + 2, animState.fishY - 2); // eye
  } else {
    // Tail to the right
    u8g2.drawTriangle(animState.fishX + 4, animState.fishY, 
                      animState.fishX + 8, animState.fishY - 3, 
                      animState.fishX + 8, animState.fishY + 3);
    u8g2.drawPixel(animState.fishX - 2, animState.fishY - 2); // eye
  }
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Aquarium");
}

void drawDinoGame() {
  if (!animState.gameOver) {
    // Auto jump every few seconds
    if (millis() - animState.lastJump > 2000 && !animState.isJumping) {
      animState.isJumping = true;
      animState.jumpVelocity = -8;
      animState.lastJump = millis();
    }
    
    // Jump physics
    if (animState.isJumping) {
      animState.dinoY += animState.jumpVelocity;
      animState.jumpVelocity += 1; // gravity
      if (animState.dinoY >= 48) {
        animState.dinoY = 48;
        animState.isJumping = false;
      }
    }
    
    // Move cactus
    animState.cactusX -= 3;
    if (animState.cactusX + 6 < 0) {
      animState.cactusX = 128 + random(20, 60);
      animState.dinoScore++;
    }
    
    // Collision detection
    if (animState.dinoX + 16 > animState.cactusX && 
        animState.dinoX < animState.cactusX + 6 &&
        animState.dinoY + 16 > animState.cactusY) {
      animState.gameOver = true;
    }
  } else {
    // Reset game after some time
    if (millis() % 5000 < 100) {
      animState.dinoY = 48;
      animState.cactusX = 128;
      animState.dinoScore = 0;
      animState.gameOver = false;
    }
  }
  
  // Draw ground line
  u8g2.drawLine(0, 64, 128, 64);
  
  // Draw dino (simple rectangle)
  u8g2.drawBox(animState.dinoX, animState.dinoY, 8, 8);
  
  // Draw cactus
  u8g2.drawBox(animState.cactusX, animState.cactusY, 6, 20);
  
  // Draw score
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Dino Game");
  String scoreStr = "Score: " + String(animState.dinoScore);
  u8g2.drawStr(80, 8, scoreStr.c_str());
  
  if (animState.gameOver) {
    u8g2.drawStr(40, 20, "GAME OVER");
  }
}

void drawRoboEyes() {
  // Change mood periodically
  if (millis() - animState.moodChangeTime > 3000) {
    animState.mood = random(3);
    animState.moodChangeTime = millis();
  }
  
  // Blinking
  if (millis() - animState.blinkTime > 3000) {
    animState.blinking = true;
    animState.blinkTime = millis();
  }
  if (animState.blinking && millis() - animState.blinkTime > 200) {
    animState.blinking = false;
  }
  
  // Pupil movement
  animState.pupilOffsetX = sin(millis() * 0.001) * 5;
  animState.pupilOffsetY = cos(millis() * 0.0015) * 3;
  
  int eyeHeight = animState.blinking ? 5 : animState.eyeSize;
  
  // Draw left eye
  u8g2.drawCircle(animState.leftEyeX, animState.eyeY, animState.eyeSize);
  if (!animState.blinking) {
    // Pupil
    u8g2.drawDisc(animState.leftEyeX + animState.pupilOffsetX, 
                  animState.eyeY + animState.pupilOffsetY, 5);
  }
  
  // Draw right eye
  u8g2.drawCircle(animState.rightEyeX, animState.eyeY, animState.eyeSize);
  if (!animState.blinking) {
    // Pupil
    u8g2.drawDisc(animState.rightEyeX + animState.pupilOffsetX, 
                  animState.eyeY + animState.pupilOffsetY, 5);
  }
  
  // Draw eyebrows based on mood
  if (animState.mood == 1) { // Happy
    u8g2.drawLine(animState.leftEyeX - 15, animState.eyeY - 25, 
                  animState.leftEyeX + 15, animState.eyeY - 30);
    u8g2.drawLine(animState.rightEyeX - 15, animState.eyeY - 30, 
                  animState.rightEyeX + 15, animState.eyeY - 25);
  } else if (animState.mood == 2) { // Angry
    u8g2.drawLine(animState.leftEyeX - 15, animState.eyeY - 30, 
                  animState.leftEyeX + 15, animState.eyeY - 25);
    u8g2.drawLine(animState.rightEyeX - 15, animState.eyeY - 25, 
                  animState.rightEyeX + 15, animState.eyeY - 30);
  }
  
  u8g2.setFont(u8g2_font_micro_tr);
  u8g2.drawStr(0, 8, "Robo Eyes");
  
  // Show mood
  String moodStr = "Mood: ";
  if (animState.mood == 0) moodStr += "Normal";
  else if (animState.mood == 1) moodStr += "Happy";
  else moodStr += "Angry";
  u8g2.drawStr(70, 8, moodStr.c_str());
}

// Web Server Handlers
void handleRoot() {
  String html = "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\">\n"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                "    <title>ESP32 OLED Controller</title>\n"
                "    <link rel=\"stylesheet\" href=\"/styles.css\">\n"
                "</head>\n"
                "<body>\n"
                "    <div class=\"container\">\n"
                "        <h1>🖥️ ESP32 OLED Animation Controller</h1>\n"
                "        \n"
                "        <div class=\"section\">\n"
                "            <h2>🎨 Select Animation</h2>\n"
                "            <div class=\"animation-grid\">\n"
                "                <button onclick=\"setAnimation('bouncing_ball')\">🏀 Bouncing Ball</button>\n"
                "                <button onclick=\"setAnimation('matrix_rain')\">💚 Matrix Rain</button>\n"
                "                <button onclick=\"setAnimation('wave_pattern')\">🌊 Wave Pattern</button>\n"
                "                <button onclick=\"setAnimation('spiral')\">🌀 Spiral</button>\n"
                "                <button onclick=\"setAnimation('fireworks')\">🎆 Fireworks</button>\n"
                "                <button onclick=\"setAnimation('snake')\">🐍 Snake Game</button>\n"
                "                <button onclick=\"setAnimation('clock')\">🕐 Digital Clock</button>\n"
                "                <button onclick=\"setAnimation('spectrum')\">📊 Spectrum</button>\n"
                "                <button onclick=\"setAnimation('aquarium')\">🐠 Aquarium</button>\n"
                "                <button onclick=\"setAnimation('dino_game')\">🦕 Dino Game</button>\n"
                "                <button onclick=\"setAnimation('robo_eyes')\">🤖 Robo Eyes</button>\n"
                "            </div>\n"
                "        </div>\n"
                "\n"
                "        <div class=\"section\">\n"
                "            <h2>⚙️ Settings</h2>\n"
                "            <div class=\"control-group\">\n"
                "                <label for=\"speed\">Animation Speed:</label>\n"
                "                <input type=\"range\" id=\"speed\" min=\"10\" max=\"200\" value=\"50\" onchange=\"updateSpeed()\">\n"
                "                <span id=\"speedValue\">50</span> ms\n"
                "            </div>\n"
                "            <div class=\"control-group\">\n"
                "                <label for=\"brightness\">Brightness:</label>\n"
                "                <input type=\"range\" id=\"brightness\" min=\"0\" max=\"100\" value=\"80\" onchange=\"updateBrightness()\">\n"
                "                <span id=\"brightnessValue\">80</span>%\n"
                "            </div>\n"
                "        </div>\n"
                "\n"
                "        <div class=\"section\">\n"
                "            <h2>📊 Status</h2>\n"
                "            <div class=\"status-info\">\n"
                "                <p><strong>Current Animation:</strong> <span id=\"currentAnim\">bouncing_ball</span></p>\n"
                "                <p><strong>Speed:</strong> <span id=\"currentSpeed\">50</span> ms</p>\n"
                "                <p><strong>Brightness:</strong> <span id=\"currentBrightness\">80</span>%</p>\n"
                "                <p><strong>WiFi Status:</strong> Connected</p>\n"
                "            </div>\n"
                "        </div>\n"
                "    </div>\n"
                "\n"
                "    <script>\n"
                "        function setAnimation(animation) {\n"
                "            fetch('/api/animation', {\n"
                "                method: 'POST',\n"
                "                headers: {'Content-Type': 'application/json'},\n"
                "                body: JSON.stringify({animation: animation})\n"
                "            })\n"
                "            .then(response => {\n"
                "                if(response.ok) {\n"
                "                    document.getElementById('currentAnim').textContent = animation;\n"
                "                }\n"
                "            });\n"
                "        }\n"
                "\n"
                "        function updateSpeed() {\n"
                "            const speed = document.getElementById('speed').value;\n"
                "            document.getElementById('speedValue').textContent = speed;\n"
                "            fetch('/api/settings', {\n"
                "                method: 'POST',\n"
                "                headers: {'Content-Type': 'application/json'},\n"
                "                body: JSON.stringify({speed: parseInt(speed)})\n"
                "            });\n"
                "        }\n"
                "\n"
                "        function updateBrightness() {\n"
                "            const brightness = document.getElementById('brightness').value;\n"
                "            document.getElementById('brightnessValue').textContent = brightness;\n"
                "            fetch('/api/settings', {\n"
                "                method: 'POST',\n"
                "                headers: {'Content-Type': 'application/json'},\n"
                "                body: JSON.stringify({brightness: parseInt(brightness)})\n"
                "            });\n"
                "        }\n"
                "\n"
                "        function updateStatus() {\n"
                "            fetch('/api/status')\n"
                "            .then(response => response.json())\n"
                "            .then(data => {\n"
                "                document.getElementById('currentAnim').textContent = data.currentAnimation;\n"
                "                document.getElementById('currentSpeed').textContent = data.animationSpeed;\n"
                "                document.getElementById('currentBrightness').textContent = data.brightness;\n"
                "            })\n"
                "            .catch(error => console.log('Status update failed'));\n"
                "        }\n"
                "\n"
                "        // Update status every 2 seconds\n"
                "        setInterval(updateStatus, 2000);\n"
                "        \n"
                "        // Initial status update\n"
                "        updateStatus();\n"
                "    </script>\n"
                "</body>\n"
                "</html>";
  
  server.send(200, "text/html", html);
}

void handleCSS() {
  String css = "body {\n"
               "    font-family: Arial, sans-serif;\n"
               "    margin: 0;\n"
               "    padding: 20px;\n"
               "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
               "    color: white;\n"
               "    min-height: 100vh;\n"
               "}\n"
               "\n"
               ".container {\n"
               "    max-width: 800px;\n"
               "    margin: 0 auto;\n"
               "    background: rgba(255, 255, 255, 0.1);\n"
               "    padding: 30px;\n"
               "    border-radius: 15px;\n"
               "    backdrop-filter: blur(10px);\n"
               "    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);\n"
               "}\n"
               "\n"
               "h1 {\n"
               "    text-align: center;\n"
               "    margin-bottom: 30px;\n"
               "    color: #ffffff;\n"
               "    text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.5);\n"
               "    font-size: 2.5em;\n"
               "}\n"
               "\n"
               "h2 {\n"
               "    color: #f0f8ff;\n"
               "    border-bottom: 2px solid rgba(255, 255, 255, 0.3);\n"
               "    padding-bottom: 10px;\n"
               "    margin-top: 30px;\n"
               "}\n"
               "\n"
               ".section {\n"
               "    margin-bottom: 30px;\n"
               "    padding: 20px;\n"
               "    background: rgba(255, 255, 255, 0.05);\n"
               "    border-radius: 10px;\n"
               "    border: 1px solid rgba(255, 255, 255, 0.1);\n"
               "}\n"
               "\n"
               ".animation-grid {\n"
               "    display: grid;\n"
               "    grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));\n"
               "    gap: 15px;\n"
               "    margin-top: 20px;\n"
               "}\n"
               "\n"
               "button {\n"
               "    background: linear-gradient(45deg, #ff6b6b, #ee5a24);\n"
               "    color: white;\n"
               "    border: none;\n"
               "    padding: 15px 20px;\n"
               "    border-radius: 8px;\n"
               "    cursor: pointer;\n"
               "    font-size: 16px;\n"
               "    font-weight: bold;\n"
               "    transition: all 0.3s ease;\n"
               "    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);\n"
               "}\n"
               "\n"
               "button:hover {\n"
               "    transform: translateY(-2px);\n"
               "    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.3);\n"
               "    background: linear-gradient(45deg, #ff5252, #d63031);\n"
               "}\n"
               "\n"
               ".control-group {\n"
               "    margin: 20px 0;\n"
               "    display: flex;\n"
               "    align-items: center;\n"
               "    gap: 15px;\n"
               "    flex-wrap: wrap;\n"
               "}\n"
               "\n"
               "label {\n"
               "    font-weight: bold;\n"
               "    min-width: 140px;\n"
               "    color: #f0f8ff;\n"
               "}\n"
               "\n"
               "input[type=\"range\"] {\n"
               "    flex: 1;\n"
               "    min-width: 150px;\n"
               "    height: 8px;\n"
               "    background: rgba(255, 255, 255, 0.2);\n"
               "    border-radius: 4px;\n"
               "    outline: none;\n"
               "}\n"
               "\n"
               "input[type=\"range\"]::-webkit-slider-thumb {\n"
               "    appearance: none;\n"
               "    width: 20px;\n"
               "    height: 20px;\n"
               "    background: linear-gradient(45deg, #ff6b6b, #ee5a24);\n"
               "    border-radius: 50%;\n"
               "    cursor: pointer;\n"
               "    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.3);\n"
               "}\n"
               "\n"
               ".status-info {\n"
               "    background: rgba(255, 255, 255, 0.05);\n"
               "    padding: 20px;\n"
               "    border-radius: 8px;\n"
               "    border-left: 4px solid #00d2d3;\n"
               "}\n"
               "\n"
               ".status-info p {\n"
               "    margin: 10px 0;\n"
               "    font-size: 16px;\n"
               "}\n"
               "\n"
               ".status-info strong {\n"
               "    color: #00d2d3;\n"
               "}\n"
               "\n"
               "@media (max-width: 600px) {\n"
               "    .container {\n"
               "        margin: 10px;\n"
               "        padding: 20px;\n"
               "    }\n"
               "    \n"
               "    .animation-grid {\n"
               "        grid-template-columns: 1fr;\n"
               "    }\n"
               "    \n"
               "    .control-group {\n"
               "        flex-direction: column;\n"
               "        align-items: stretch;\n"
               "    }\n"
               "    \n"
               "    label {\n"
               "        min-width: auto;\n"
               "    }\n"
               "    \n"
               "    h1 {\n"
               "        font-size: 2em;\n"
               "    }\n"
               "}";
  
  server.send(200, "text/css", css);
}

void handleAnimationChange() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);
    
    if (doc.containsKey("animation")) {
      currentAnimation = doc["animation"].as<String>();
      Serial.println("Animation changed to: " + currentAnimation);
      
      // Reset animation state for new animation
      if (currentAnimation == "fireworks") {
        animState.fireworksInit = false;
      }
      
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"Invalid request\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleSettingsChange() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);
    
    bool changed = false;
    
    if (doc.containsKey("speed")) {
      animationSpeed = doc["speed"];
      changed = true;
      Serial.println("Speed changed to: " + String(animationSpeed));
    }
    
    if (doc.containsKey("brightness")) {
      brightness = doc["brightness"];
      u8g2.setContrast(brightness * 2.55);
      changed = true;
      Serial.println("Brightness changed to: " + String(brightness));
    }
    
    if (changed) {
      server.send(200, "application/json", "{\"status\":\"success\"}");
    } else {
      server.send(400, "application/json", "{\"error\":\"No valid parameters\"}");
    }
  } else {
    server.send(400, "application/json", "{\"error\":\"No data received\"}");
  }
}

void handleStatus() {
  DynamicJsonDocument doc(1024);
  doc["currentAnimation"] = currentAnimation;
  doc["animationSpeed"] = animationSpeed;
  doc["brightness"] = brightness;
  doc["wifiStatus"] = WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected";
  doc["ipAddress"] = WiFi.localIP().toString();
  doc["uptime"] = millis();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}
