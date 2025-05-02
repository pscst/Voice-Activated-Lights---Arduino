#include <FastLED.h>
#include <SoftwareSerial.h>

// === LED Setup ===
#define LED_PIN     6
#define NUM_LEDS    64
#define WIDTH       8
#define HEIGHT      8
#define BRIGHTNESS  50

CRGB leds[NUM_LEDS];

// === Bluetooth Setup ===
SoftwareSerial BT(10, 11); // RX, TX
String currentPattern = "off"; // Default pattern

// === Serpentine Mapping ===
uint16_t XY(uint8_t x, uint8_t y) {
  if (y % 2 == 0) {
    return y * WIDTH + x;
  } else {
    return y * WIDTH + (WIDTH - 1 - x);
  }
}

void setPixel(int x, int y, CRGB color) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    leds[XY(x, y)] = color;
  }
}

// === Heart Bitmap ===
const byte heartShape[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);
  FastLED.clear();
  FastLED.show();

pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }

  BT.begin(9600);
  Serial.begin(9600);
  delay(1000);
  Serial.println("Ready to receive pattern commands...");
}

// === Patterns ===

void pulseWaveform() {
  static unsigned long lastBeat = 0;
  static int beatPhase = 0;
  static bool inPulse = false;
  const int pulseDuration = 250;
  const int beatInterval = 1000;
  const int pauseDuration = 300;
  unsigned long currentTime = millis();

  if (!inPulse && currentTime - lastBeat > (beatPhase == 1 ? pauseDuration : beatInterval)) {
    inPulse = true;
    lastBeat = currentTime;
  }

  if (inPulse) {
    float progress = min(1.0, (float)(currentTime - lastBeat) / pulseDuration);
    if (progress >= 1.0) {
      inPulse = false;
      beatPhase = (beatPhase + 1) % 3;
    } else {
      FastLED.clear();
      uint8_t brightness = (progress < 0.5)
        ? 255 * (progress * 2.0)
        : 255 * (2.0 - progress * 2.0);
      for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
          if (bitRead(heartShape[y], 7 - x)) {
            setPixel(x, y, CRGB(brightness, 0, 0));
          }
        }
      }
    }
  } else {
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        if (bitRead(heartShape[y], 7 - x)) {
          setPixel(x, y, CRGB(20, 0, 0));
        }
      }
    }
  }

  FastLED.show();
}


// Fire (red/orange flicker)
void firePattern() {
  static byte heat[8][8]; 
  const uint8_t cooling = 55;   
  const uint8_t sparking = 120; 


  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      heat[x][y] = qsub8(heat[x][y], random8(0, ((cooling * 10) / 8) + 2));
    }
  }

 
  for (int x = 0; x < 8; x++) {
    for (int y = 7; y >= 2; y--) {
      heat[x][y] = (heat[x][y - 1] + heat[x][y - 2] + heat[x][y - 2]) / 3;
    }
  }


  for (int x = 0; x < 8; x++) {
    if (random8() < sparking) {
      heat[x][0] = qadd8(heat[x][0], random8(160, 255));
    }
  }


  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      CRGB color = HeatColor(heat[x][y]); 
      setPixel(x, 7 - y, color); 
    }
  }

  FastLED.show();
  delay(40);
}


// Water wave pattern
void waterWave() {
  static uint8_t t = 0;
  static uint8_t sparkleTimer = 0;

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {

      // More dynamic waves: combine x and y waves
      uint8_t wave = sin8(x * 18 + t) + sin8(y * 16 - t);
      uint8_t brightness = wave / 2; // Blend waves
      
      CHSV color = CHSV(160 + sin8(t + y * 8) / 10, 255, brightness);

      // Random sparkle effect
      if (random8() < 5) {  
        color = CHSV(160, 100, 255); 
      }

      setPixel(x, y, color);
    }
  }

  FastLED.show();
  delay(30);
  t += 3;  
}



// Air shimmer
void airPattern() {
  static uint8_t phase = 0;

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {

   
      uint8_t wave = sin8(x * 8 + phase) + sin8(y * 8 - phase);
      uint8_t brightness = wave / 2; 
      
     
      uint8_t hueShift = sin8(phase + y * 5) / 8; 
      CHSV color = CHSV(135 + hueShift, 50, brightness);

      setPixel(x, y, color);
    }
  }

  FastLED.show();
  phase += 3; 
  delay(30);
}



// Arrow animation (scrolling right)
const byte rightArrow[8] = {
  B00011000,
  B00011100,
  B11111110,
  B11111111,
  B11111110,
  B00011100,
  B00011000,
  B00000000
};

void arrowWaveRight() {
  static int scrollX = 0;
  FastLED.clear();

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int arrowX = (x + scrollX) % 8;
      if (arrowX < 0) arrowX += 8;
      if (bitRead(rightArrow[y], arrowX)) {
        setPixel(x, y, CRGB::Aqua);
      }
    }
  }

  FastLED.show();
  delay(100);
  scrollX = (scrollX + 1) % 8;
}

const byte leftArrow[8] = {
  B00011000,
  B00111000, 
  B01111111, 
  B11111111, 
  B01111111, 
  B00111000, 
  B00011000, 
  B00000000
};

void arrowWaveLeft() {
  static int scrollX = 0;
  FastLED.clear();

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int arrowX = (x - scrollX) % 8;
      if (arrowX < 0) arrowX += 8;
      if (bitRead(leftArrow[y], arrowX)) {
        setPixel(x, y, CRGB::Aqua);
      }
    }
  }

  FastLED.show();
  delay(100);
  scrollX = (scrollX + 1) % 8; // Scroll left
}


// Up arrow pattern
const byte upArrow[8] = {
  B00011000,
  B00111100,
  B01111110,
  B11111111,
  B00111100,
  B00111100,
  B00111100,
  B00000000
};

// Down arrow pattern
const byte downArrow[8] = {
  B00000000,
  B00111100,
  B00111100,
  B00111100,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

void scrollUpArrows() {
  static int scrollY = 0;
  FastLED.clear();
  
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int arrowY = (y + scrollY) % 8;  // Moving up
      if (arrowY < 0) arrowY += 8;
      if (bitRead(upArrow[arrowY], 7-x)) {  
        setPixel(x, y, CRGB::Aqua);
      }
    }
  }
  
  FastLED.show();
  delay(100);
  scrollY = (scrollY + 1) % 8;  
  
}

void scrollDown() {
  static int scrollY = 0;
  FastLED.clear();
  
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      int arrowY = (y - scrollY) % 8;  // Moving down
      if (arrowY < 0) arrowY += 8;
      if (bitRead(downArrow[arrowY], 7-x)) {  
        setPixel(x, y, CRGB::Aqua);
      }
    }
  }
  
  FastLED.show();
   delay(100);
  scrollY = (scrollY + 1) % 8; 
 
}


const byte lightBulb[8] = {
  B00111100,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00011000,
  B00011000,
  B00011000
};

void lightBulbGlow() {
  static uint8_t brightness = 100;     

  CRGB baseColor = CRGB::White;        

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      if (bitRead(lightBulb[y], 7 - x)) {
        CRGB pixel = baseColor;
        pixel.nscale8(brightness);    
        setPixel(x, y, pixel);
      } else {
        setPixel(x, y, CRGB::Black);  
      }
    }
  }

  FastLED.show();
  delay(30); 
}


void rainbowPattern() {
  static uint8_t hue = 0;

  // Define the center of the rainbow arc
  int centerX = 3;  // Middle left
  int centerY = 7;  // Bottom center 

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {

      // Calculate distance from center to create curved effect
      int dx = x - centerX;
      int dy = y - centerY;
      float distance = sqrt(dx * dx + dy * dy); 

      // Map distance to hue
      leds[XY(x, y)] = CHSV(hue + distance * 20, 255, 255);
    }
  }

  hue += 2;  // Shift rainbow over time
  FastLED.show();
  delay(50);
}


void fireworksPattern() {
  static uint8_t launchY = 7;
  static uint8_t phase = 0; 
  static uint8_t burstColorHue;
  static uint8_t explosionRadius = 0;
  static uint8_t centerX, centerY;

  FastLED.clear();

  if (phase == 0) {
    // Launch phase
    leds[XY(4, launchY)] = CHSV(0, 0, 255);
    if (launchY > 3) {
      launchY--;
    } else {
      phase = 1;
      burstColorHue = random8();
      centerX = 4;
      centerY = launchY;
    }
  }

  else if (phase == 1) {
    // Explosion phase
    for (int i = 0; i < 12; i++) {
      float angle = i * (PI * 2.0 / 12);
      int x = round(centerX + cos(angle) * explosionRadius);
      int y = round(centerY + sin(angle) * explosionRadius);

      if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        leds[XY(x, y)] = CHSV(burstColorHue + i * 10, 255, 255);
      }
    }
    explosionRadius++;
    if (explosionRadius > 3) {
      phase = 2;
    }
  }

  else if (phase == 2) {
    // Fade out
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(40);
    }
    static uint8_t fadeCount = 0;
    fadeCount++;
    if (fadeCount > 5) {
      // Reset for next firework
      fadeCount = 0;
      phase = 0;
      launchY = 7;
      explosionRadius = 0;
    }
  }

  FastLED.show();
  delay(80);
}




void spiralPattern() {
  static uint8_t angle = 0;
  FastLED.clear();

  for (uint8_t i = 0; i < 8; i++) {


    float x = 3.5 + (cos8(angle + i * 32) - 128) / 32.0;
    float y = 3.5 + (sin8(angle + i * 32) - 128) / 32.0;


    uint8_t xi = constrain(round(x), 0, 7);
    uint8_t yi = constrain(round(y), 0, 7);

    leds[XY(xi, yi)] = CHSV(angle + i * 10, 255, 255);
  }

  angle += 5;
  FastLED.show();
  delay(50);
}

void digitalRainPattern() {
  static uint8_t rain[8];  

  FastLED.clear();


  for (int col = 0; col < 8; col++) {

    if (random8() < 24) {  
      rain[col] = 0; 
    } else {
      rain[col]++;  
    }


    for (int row = 0; row < 8; row++) {
      if (row < rain[col]) {
        leds[XY(col, row)] = CHSV(85, 255, 255);  
      } else {
        leds[XY(col, row)] = CRGB::Black;  
      }
    }
  }


  FastLED.show();
  delay(50); 
}

// Rain pattern
void rainPattern() {
  static uint8_t raindropTimer = 0;


  if (raindropTimer++ > 5) { 
    int x = random(0, 8);     
    leds[XY(x, 0)] = CRGB::Blue; 
    raindropTimer = 0;
  }


  for (int y = 7; y > 0; y--) {
    for (int x = 0; x < 8; x++) {
      leds[XY(x, y)] = leds[XY(x, y - 1)];
    }
  }


  for (int x = 0; x < 8; x++) {
    leds[XY(x, 0)] = CRGB::Black;
  }

  FastLED.show();
  delay(100); 
}


void waveAnimation() {
  static uint8_t wavePosition = 0;
  
  FastLED.clear();

  for (int x = 0; x < 8; x++) {

   
    int y = 4 + sin8(x * 32 + wavePosition) / 32 - 4;
    if (y >= 0 && y < 8) {
      leds[XY(x, y)] = CHSV(wavePosition + x * 16, 255, 255);
    }
  }

  FastLED.show();
  wavePosition += 4; 
  delay(30);
}



void partyMode() {
  static uint8_t hue = 0;
  static unsigned long lastFlash = 0;
  unsigned long now = millis();

  if (now - lastFlash > random(50, 150)) { 
    for (int i = 0; i < NUM_LEDS; i++) {
      if (random8() < 50) { 
        leds[i] = CHSV(hue + random8(50), 255, 255);
      } else {
        leds[i] = CRGB::Black;
      }
    }
    hue += 16; 
    FastLED.show();
    lastFlash = now;
  }
}

void relaxMode() {
  static uint8_t hue = 160; 
  fill_solid(leds, NUM_LEDS, CHSV(hue, 200, 150)); 
  hue++; 
  FastLED.show();
  delay(60); 
}


void drawHappyFaceAnimation() {
  static bool blink = false;
  static unsigned long lastBlinkTime = 0;
  unsigned long now = millis();

  if (now - lastBlinkTime > 500) {
    blink = !blink;
    lastBlinkTime = now;
  }

  const byte happyOpenEyes[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100
  };

  const byte happyBlinkEyes[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10011001,
    B01000010,
    B00111100
  };

  const byte* face = blink ? happyBlinkEyes : happyOpenEyes;

  for (int y = 0; y < 8; y++) {
    byte row = face[y];
    for (int x = 0; x < 8; x++) {
      if (bitRead(row, 7 - x)) {
        leds[XY(x, y)] = CRGB::Yellow;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  FastLED.show();
}

void drawSadFaceAnimation() {
  static uint8_t tearPos = 0;
  static unsigned long lastTearTime = 0;
  unsigned long now = millis();

  if (now - lastTearTime > 300) { 
    tearPos++;
    if (tearPos > 7) tearPos = 3; 
    lastTearTime = now;
  }

  const byte sadFaceBase[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100
  };

  for (int y = 0; y < 8; y++) {
    byte row = sadFaceBase[y];
    for (int x = 0; x < 8; x++) {
      if (bitRead(row, 7 - x)) {
        leds[XY(x, y)] = CRGB::Blue;
      } else {
        leds[XY(x, y)] = CRGB::Black;
      }
    }
  }

  // Draw a "tear" drop on side of the face
  leds[XY(2, tearPos)] = CRGB::Aqua; 

  FastLED.show();
}




// Define 'HI' 
const byte message_HI[7][12] = {
  {0,0,1,1,1,0,  0,1,0,0,0,1},
  {0,0,0,1,0,0,  0,1,0,0,0,1},
  {0,0,0,1,0,0,  0,1,0,0,0,1},
  {0,0,0,1,0,0,  0,1,1,1,1,1},
  {0,0,0,1,0,0,  0,1,0,0,0,1},
  {0,0,0,1,0,0,  0,1,0,0,0,1},
  {0,0,1,1,1,0,  0,1,0,0,0,1}
};


void scrollHI() {
  static int8_t scrollX = WIDTH; 

  FastLED.clear();

  for (uint8_t y = 0; y < 7; y++) { 
    for (uint8_t x = 0; x < WIDTH; x++) {
      int8_t msgX = x - scrollX;
      if (msgX >= 0 && msgX < 12) {
        if (message_HI[y][msgX]) {
          setPixel(x, y, CRGB::White);
        }
      }
    }
  }

  FastLED.show();
  delay(100);

  scrollX--;

  if (scrollX < -12) { 
    scrollX = WIDTH;   
  }
}


void redLight() {
  static int pos = 0;
  static int dir = 1; 

  FastLED.clear();


  setPixel(pos, 3, CRGB::Red);
  setPixel(pos, 4, CRGB::Red);


  if (pos > 0) {
    setPixel(pos - 1, 3, CRGB::DarkRed);
    setPixel(pos - 1, 4, CRGB::DarkRed);
  }
  if (pos < WIDTH - 1) {
    setPixel(pos + 1, 3, CRGB::DarkRed);
    setPixel(pos + 1, 4, CRGB::DarkRed);
  }

  FastLED.show();
  delay(50);

  pos += dir;
  if (pos <= 0 || pos >= WIDTH - 1) {
    dir = -dir; 
  }
}

void greenLight() {
  static int pos = 0;
  static int dir = 1; 

  FastLED.clear();


setPixel(pos, 3, CRGB::Green); 
setPixel(pos, 4, CRGB::Green);  

// Optional: Fading tail effect
if (pos > 0) {
  setPixel(pos - 1, 3, CRGB::DarkGreen);  
  setPixel(pos - 1, 4, CRGB::DarkGreen);  
}
if (pos < WIDTH - 1) {
  setPixel(pos + 1, 3, CRGB::DarkGreen);  
  setPixel(pos + 1, 4, CRGB::DarkGreen); 
}


  FastLED.show();
  delay(50);

  pos += dir;
  if (pos <= 0 || pos >= WIDTH - 1) {
    dir = -dir; 
  }
}


// PAC-MAN open mouth
const byte pacman_open[8] = {
  B00111100,
  B01111110,
  B11111000,
  B11110000,
  B11111000,
  B01111110,
  B00111100,
  B00000000
};

// PAC-MAN closed mouth 
const byte pacman_closed[8] = {
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00000000
};

// Ghost bitmap stays the same
const byte ghost[8] = {
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11011011,
  B11111111,
  B11111111
};

void pacmanChompAnimation() {
  static int scrollX = 8;
  static bool mouthOpen = true; // Toggle mouth

  FastLED.clear();


  const byte* pacmanFrame = mouthOpen ? pacman_open : pacman_closed;

  // Draw Pac-Man
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (bitRead(pacmanFrame[y], 7 - x)) {
        int drawX = x - scrollX - 10;
        if (drawX >= 0 && drawX < 8) {
           setPixel(drawX, y, CRGB(128, 128, 0)); // Dim warm gold
        }
      }
    }
  }

  // Draw Ghost
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (bitRead(ghost[y], 7 - x)) {
        int drawX = x - scrollX ; // Ghost a little behind
        if (drawX >= 0 && drawX < 8) {
          setPixel(drawX, y, CRGB::Blue);
        }
      }
    }
  }

  FastLED.show();
  delay(100);

  scrollX--;
  if (scrollX < -16) {
    scrollX = 8; 
  }


  mouthOpen = !mouthOpen;
}

void wakeUpSystem() {
  Serial.println("Waking up!");
  
  // Quick white flash
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(100);
  
  FastLED.clear();
  FastLED.show();
  

  currentPattern = "hello aries"; 
}



// === Loop & Command Handling ===
String cleanString(String input) {
  String output = "";
  for (unsigned int i = 0; i < input.length(); i++) {
    char c = input.charAt(i);
    if (isPrintable(c)) {   
      output += c;
    }
  }
  return output;
}

void handleCommand(String cmd) {
   BT.println("Received command: " + cmd);
  
  
  if (cmd == "wake up") {
    wakeUpSystem();
  }else if (cmd == "turn off light") {
    currentPattern = "turn off light";
    FastLED.clear();
    FastLED.show();
    BT.println("Turning off light.");
  }
  else if (cmd == "make a fire" || cmd == "demonstrate water flow" || cmd == "demonstrate air" ||
           cmd == "signal right" || cmd == "signal left" || cmd == "upward arrow" || cmd == "downward arrow" ||
           cmd == "show a heart" || cmd == "show a rainbow" || cmd == "show fireworks" || cmd == "show spiral" || cmd == "matrix" ||
           cmd == "raindrop" || cmd == "waves" || cmd == "red light" || cmd == "green light" || cmd == "turn on party lights" || cmd == "turn on relax mode" ||
           cmd == "show a happy face" || cmd == "show a sad face" || cmd == "hello aries" || cmd == "show pac-man") {
    currentPattern = cmd;
    BT.println("Switching to pattern: " + cmd);
  }
  else if (cmd == "turn on light bulb") {
    currentPattern = "turn on light bulb";
    BT.println("Turning on light bulb.");
  }
  else {
    Serial.println("Unknown command: " + cmd);
    BT.println("Unknown command: " + cmd);
  }
}


void flushBT() {
  while (BT.available()) {
    BT.read();
  }
}


void loop() {
if (BT.available() > 0) {
  String cmd = BT.readStringUntil('\n');
  cmd.trim();
  cmd = cleanString(cmd);
  cmd.toLowerCase();
  
  if (cmd.length() >= 3) { 
    Serial.println("Received command: " + cmd);
    BT.println("Received command: " + cmd);
    delay(10); // small delay to let BT send properly
    handleCommand(cmd);
  } else {
    flushBT(); // clear garbage if too short
  }
}



  // Pattern selector
  if (currentPattern == "make a fire") firePattern();
  else if (currentPattern == "demonstrate water flow") waterWave();
  else if (currentPattern == "demonstrate air") airPattern();
  else if (currentPattern == "signal right") arrowWaveRight();
  else if (currentPattern == "signal left") arrowWaveLeft();
  else if (currentPattern == "upward arrow") scrollUpArrows();
  else if (currentPattern == "downward arrow") scrollDown();
  else if (currentPattern == "show a heart") pulseWaveform();
  else if (currentPattern == "show a rainbow") rainbowPattern();
  else if (currentPattern == "show fireworks") fireworksPattern();
  else if (currentPattern == "show spiral") spiralPattern();
  else if (currentPattern == "matrix") digitalRainPattern();
  else if (currentPattern == "turn on light bulb") lightBulbGlow();
  else if (currentPattern == "raindrop") rainPattern();
  else if (currentPattern == "waves") waveAnimation();
  else if (currentPattern == "turn on party lights") partyMode();
else if (currentPattern == "turn on relax mode") relaxMode();
else if (currentPattern == "show a happy face") drawHappyFaceAnimation();
else if (currentPattern == "show a sad face") drawSadFaceAnimation();
else if (currentPattern == "hello aries") scrollHI();
else if(currentPattern == "red light") redLight();
else if(currentPattern == "green light") greenLight();
else if(currentPattern == "show pac-man") pacmanChompAnimation();

  
  delay(10);
}

