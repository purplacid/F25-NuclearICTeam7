// --- RECOMMENDED MODIFIED TRANSMITTER CODE (for a separate Arduino board) ---
// This board acts as the hidden "nuclear radiation source."

const int TRANSMIT_PIN = 7; // Connect TX-118S K1 to Arduino Digital Pin 7

void setup() {
  pinMode(TRANSMIT_PIN, OUTPUT);
  digitalWrite(TRANSMIT_PIN, HIGH); // K1 is HIGH by default (no transmission)
  Serial.begin(9600);
  Serial.println("433MHz Transmitter Ready!");
  Serial.println("Continuously sending 'radiation' signal...");
}

void loop() {
  // Simulate pressing and releasing the K1 button to send a distinct pulse
  digitalWrite(TRANSMIT_PIN, LOW);   // "Press" the button (pull K1 low)
  delay(50);                        // Hold it for 100ms (more reliable for encoder)
  digitalWrite(TRANSMIT_PIN, HIGH);  // "Release" the button (pull K1 high)
  delay(200);                        // Wait 400ms before "pressing" again. Total pulse cycle: 500ms.
                                     // This means 2 pulses per second (1000ms / 500ms).
}