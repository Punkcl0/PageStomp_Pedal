//*************************//
// ESP32-C3
//
// The ESP32 acts as a Bluetooth HID keyboard
// and sends keystrokes to the connected device.
//
// First version:
// - Start BLE
// - Wait for a connection
// - Send data if a button is pressed
//*************************//

#include <HijelHID_BLEKeyboard.h>
#include <Button2.h>
#include <Adafruit_NeoPixel.h>

// Bluetooth
HijelHID_BLEKeyboard keyboard("PageStomp Pedal", "Gaston Tech", 100);

//Leds
constexpr uint8_t PIXEL_PIN = 7;
constexpr uint8_t PIXEL_COUNT = 4;

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//Buttons
constexpr uint8_t LEFT_BUTTON_PIN = 5;
constexpr uint8_t RIGHT_BUTTON_PIN = 6;

Button2 leftBt;    //Left button object
Button2 rightBt;   //Right button object

//Configuration

constexpr unsigned int LONG_CLICK_DELAY = 1500; //Time in ms for detect a long clic from a short clic

struct ConfigBouton {
  uint8_t appuiCourt;
  uint8_t appuiLong;
};

ConfigBouton boutonGauche = {
  KEY_LEFT,
  KEY_HOME
};

ConfigBouton boutonDroit = {
  KEY_RIGHT,
  KEY_END
};

//******************//
// Leds
//******************//

void welcome() {

  for(int a=0; a<4; a++) {  // Repeat 4 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)

      uint32_t color = strip.Color(127, 50, 220);
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(100);  // Pause for a moment
      
    }
  }
  strip.clear();
  strip.show();
}

void simplePress(int firstLed, int wait = 500) {
  strip.clear();         //   Set all pixels in RAM to 0 (off)
  for(int i = firstLed; i< firstLed+2; i += 1) {
        uint32_t color = strip.Color(80, 80, 80);
        strip.setPixelColor(i, color); 
      }
  strip.show(); // Update strip with new contents
  delay(wait);  // Pause for a moment
  
  strip.clear();
  strip.show();
}

void longPress(int firstLed, int wait = 500) {
  strip.clear();         //   Set all pixels in RAM to 0 (off)
  for(int i = firstLed; i< firstLed+2; i += 1) {
        uint32_t color = strip.Color(10, 10, 80);
        strip.setPixelColor(i, color); 
      }
  strip.show(); // Update strip with new contents
  delay(wait);  // Pause for a moment

  strip.clear();
  strip.show();
}

void haloDisplay() {

  static uint8_t brightness = 10;
  static int8_t direction = 1;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate < 20) {
    return;
  }

  lastUpdate = millis();

  brightness += direction;

  if (brightness >= 70) {
    brightness = 70;
    direction = -1;
  }

  if (brightness <= 10) {
    brightness = 10;
    direction = 1;
  }

  // Left button - soft purple
  uint32_t leftColor = strip.Color(
    brightness / 2,
    0,
    brightness
  );

  // Right button - pale green
  uint32_t rightColor = strip.Color(
    brightness / 3,
    brightness,
    brightness / 3
  );

  strip.setPixelColor(0, leftColor);
  strip.setPixelColor(1, leftColor);

  strip.setPixelColor(2, rightColor);
  strip.setPixelColor(3, rightColor);

  strip.show();
}


//******************//
// Helpers
//******************//

void leftClick(Button2& btn) {
  if (keyboard.isPaired()) {
    keyboard.tap(boutonGauche.appuiCourt);
    //Serial.println("Left click"); //Uncomment for debug
    simplePress(0,250);
  }
}

void rightClick(Button2& btn) {
  if (keyboard.isPaired()) {
    keyboard.tap(boutonDroit.appuiCourt);
    //Serial.println("Right click"); //Uncomment for debug
    simplePress(2,250);
  }
}
void leftLongClick(Button2& btn) {
  if (keyboard.isPaired()) {
    keyboard.tap(boutonGauche.appuiLong);
    //Serial.println("Left Long click"); //Uncomment for debug
    longPress(0);
  }
}
void rightLongClick(Button2& btn) {
  if (keyboard.isPaired()) {
    keyboard.tap(boutonDroit.appuiLong );
    //Serial.println("Right long click"); ////Uncomment for debug
    longPress(2);
  }
}

//******************//
// Setup
//******************//

void setup() {
  //Serial.begin(115200); //Uncomment for debug

  leftBt.begin(LEFT_BUTTON_PIN );
  rightBt.begin(RIGHT_BUTTON_PIN);

  leftBt.setClickHandler(leftClick);
  rightBt.setClickHandler(rightClick);

  leftBt.setLongClickHandler(leftLongClick);
  rightBt.setLongClickHandler(rightLongClick);

  leftBt.setLongClickTime(LONG_CLICK_DELAY);
  rightBt.setLongClickTime(LONG_CLICK_DELAY);

  //Serial.println("Starting BLE keyboard..."); //Uncomment for debug
  keyboard.begin();

  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)

  welcome();
  delay(10);

  strip.show();  // Initialize all pixels to 'off'
}

//******************//
// Loop
//******************//

void loop() {

  leftBt.loop();
  rightBt.loop();

  haloDisplay(); //Lights on

}