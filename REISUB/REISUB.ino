#include <Keyboard.h>
#include "LowPower.h"

#define LED_PIN 2

long timeout = 10 * 60 * ((long) 1000); // 10 minutes

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  bool reisub = false;
  
  // === wait for serial connection or timeout ===
  Serial.begin(9600);
  timeout += millis();
  while(!Serial) {
    if(millis() > timeout) {
      reisub = true;
      break;
    }

    blinkOnce(); // also causes delay
  }

  
  // === serial connection established ===
  if(!reisub) {
    Serial.print("Reboot prevented. ");
    Serial.print((timeout - millis()) / 1000);
    Serial.println(" seconds were left.");
    
    Serial.end();

    // Give system time to detect serial disconnected
    delay(5000);

    digitalWrite(LED_PIN, LOW);

    // go to sleep, don't let anyone wake you
    // NOTE: Arduino won't be recognized as USB device until unplugged and reconnected
    while(true)  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  Serial.end();

  
  // === issue REISUB key sequence ===
  digitalWrite(LED_PIN, HIGH);

  // Give system time to detect serial disconnected
  delay(5000);

  issueREISUB();
}

// Connect as keyboard, send REISUB key sequence, disconnect
void issueREISUB() {
  // Connect as keyboard
  Keyboard.begin();

  // Give system time to recognize the keyboard
  delay(5000);

  // Hold down Alt + Print Screen (SysRq)
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_PRINT_SCREEN);
  delay(1000);

  // Send REISUB sequence with brief delays
  typeKey('r');
  typeKey('e');
  typeKey('i');
  typeKey('s');
  typeKey('u');
  typeKey('b');

  // Release all keys
  Keyboard.releaseAll();

  // Disconnect as keyboard
  Keyboard.end();
}

// Press and release key without releasing keys that are currently pressed
void typeKey(char key) {
  Keyboard.press(key);
  delay(100);
  Keyboard.release(key);
  delay(900);  // => 1 second delay between each key press
}

// Turn LED on and off once, with delay
void blinkOnce() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
