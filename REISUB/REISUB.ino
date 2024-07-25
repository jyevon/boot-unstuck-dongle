#include <Keyboard.h>

#define LED_PIN 2

long timeout = 10 * 60 * ((long) 1000); // 10 minutes

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  bool reisub = false;
  
  // === wait for serial connection or timeout ===
  digitalWrite(LED_PIN, HIGH);
  
  Serial.begin(9600);
  while(!Serial) {
    if(millis() > timeout) {
      reisub = true;
      break;
    }

    delay(100);
  }

  
  // === serial connection established ===
  if(!reisub) {
    Serial.print("Reboot prevented. ");
    Serial.print((timeout - millis()) / 1000);
    Serial.println(" seconds were left.");
    
    Serial.end();

    // become inactive
    while(true)  blinkOnce();
  }

  
  // === issue REISUB key sequence ===
  digitalWrite(LED_PIN, LOW);

  // Give system time to detect serial disconnected
  delay(5000);

  issueREISUB();

  
  // === REISUB issued ===
  blinkOnce();
  blinkOnce();
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
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
