#include <Keyboard.h>

#define LED_PIN 2

long timeout = 10 * 60 * ((long) 1000); /*10 minutes*/

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  
  bool reisub = false;
  
  Serial.begin(9600);
  while(!Serial) {
    if(millis() > timeout) {
      reisub = true;
      delay(100);
      break;
    }
  }
  
  Serial.print("Reboot prevented. ");
  Serial.print((timeout - millis()) / 1000);
  Serial.println(" seconds were left.");
  
  Serial.end();

  if(!reisub) { // deactivate
    while(true) {
      digitalWrite(LED_PIN, HIGH);
      delay(1000);
      digitalWrite(LED_PIN, LOW);
      delay(1000);
    }
  }

  
  digitalWrite(LED_PIN, LOW);

  delay(5000);
  
  // Start the Keyboard library
  Keyboard.begin();

  // Give the system some time to recognize the keyboard
  delay(5000);

  // Hold down Alt + Print Screen (SysRq)
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.press(KEY_PRINT_SCREEN);
  delay(1000);

  // Send the REISUB sequence with brief delays
  sendKeyWithModifiers('r');
  sendKeyWithModifiers('e');
  sendKeyWithModifiers('i');
  sendKeyWithModifiers('s');
  sendKeyWithModifiers('u');
  sendKeyWithModifiers('b');

  // Release all keys
  Keyboard.releaseAll();

  // End the Keyboard library
  Keyboard.end();
  
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

void sendKeyWithModifiers(char key) {
  // Send the key while holding Alt and Print Screen
  Keyboard.press(key);
  delay(100);
  Keyboard.release(key);
  delay(900);  // 1 second delay between each key press
}
