// Sun-direction tracker using two LDRs (left/right) with a divider between them.
// LDR on A0 = LEFT, LDR on A1 = RIGHT.
// Each LDR: 5V -> LDR -> junction -> 10k -> GND, junction to the analog pin.
//
// v3: a fixed-in scale for the right sensor (set once, no recalibrating every power-up)
//     and an ABSOLUTE minimum difference, so small gaps in dim light don't count as "sun".
//
// Serial commands:
//   c = calibrate (put BOTH LDRs in the SAME bright, even light first)
//   z = reset scale to DEFAULT_RIGHT_SCALE

// After you run 'c' once, it prints a value. Paste that number here and re-upload.
const float DEFAULT_RIGHT_SCALE = 1.0;

const int LEFT_PIN  = A0;
const int RIGHT_PIN = A1;

const bool  SWAP_SIDES = false;  // true if LEFT/RIGHT are backwards on your hardware
const bool  INVERT     = false;  // true if readings DROP when it gets brighter
const float ALPHA      = 0.2;    // smoothing: lower = smoother but slower
const float GAIN       = 3.0;    // amplifies the left/right difference on the bar
const int   DEADZONE   = 10;     // within +/- this on the bar counts as centered
const int   MIN_DIFF   = 25;     // LEFT and RIGHT(adj) must differ by at least this many counts
const int   MIN_LIGHT  = 50;     // average (after scaling) below this = too dark to tell

const unsigned long SAMPLE_MS = 20;
const unsigned long PRINT_MS  = 250;

float left = 0, right = 0;  // smoothed raw readings, 0 (dark) to 1023 (bright)
float rightScale = DEFAULT_RIGHT_SCALE;
unsigned long lastSample = 0, lastPrint = 0;

int readLeft()  { int v = analogRead(SWAP_SIDES ? RIGHT_PIN : LEFT_PIN);  return INVERT ? 1023 - v : v; }
int readRight() { int v = analogRead(SWAP_SIDES ? LEFT_PIN  : RIGHT_PIN); return INVERT ? 1023 - v : v; }

void calibrate() {
  Serial.println("Calibrating: keep both LDRs in the SAME bright, even light...");
  delay(1500);
  long l = 0, r = 0;
  const int N = 100;
  for (int i = 0; i < N; i++) {
    l += readLeft();
    r += readRight();
    delay(5);
  }
  if (r < 100 || l < 100) {
    Serial.println("Too dark to calibrate. Use brighter, even light and send 'c' again.");
    return;
  }
  rightScale = (float)l / (float)r;
  Serial.print("Right scale set to ");
  Serial.println(rightScale, 3);
  Serial.print("To make it permanent, set DEFAULT_RIGHT_SCALE = ");
  Serial.print(rightScale, 3);
  Serial.println(" in the code and re-upload.");
}

void setup() {
  Serial.begin(9600);
  left  = readLeft();
  right = readRight();
  Serial.println("Readings: 0 = dark, 1023 = very bright.");
  Serial.print("Right scale: ");
  Serial.println(rightScale, 3);
  Serial.println("Send 'c' with both LDRs in the same light to calibrate. 'z' resets.");
}

void loop() {
  unsigned long now = millis();

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'c') calibrate();
    if (c == 'z') { rightScale = DEFAULT_RIGHT_SCALE; Serial.println("Scale reset"); }
  }

  if (now - lastSample >= SAMPLE_MS) {
    lastSample = now;
    left  += ALPHA * (readLeft()  - left);
    right += ALPHA * (readRight() - right);
  }

  if (now - lastPrint >= PRINT_MS) {
    lastPrint = now;

    float r = constrain(right * rightScale, 0, 1023);  // right, matched to left
    float diff = left - r;
    float total = left + r;

    Serial.print("LEFT ");
    Serial.print((int)left);
    Serial.print("  RIGHT(raw) ");
    Serial.print((int)right);
    Serial.print("  RIGHT(adj) ");
    Serial.print((int)r);
    Serial.print("  diff ");
    Serial.print((int)diff);
    Serial.print("   ");

    if (total / 2.0 < MIN_LIGHT) {
      Serial.println("too dark to tell");
      return;
    }

    // -100 = all light on RIGHT sensor, 0 = equal, +100 = all light on LEFT sensor
    int pos = (int)constrain(GAIN * 100.0 * diff / total, -100, 100);
    bool centered = (abs(diff) < MIN_DIFF) || (abs(pos) <= DEADZONE);
    if (centered) pos = 0;

    // 21-slot bar: slot 0 = far LEFT, slot 10 = center, slot 20 = far RIGHT
    int slot = constrain(10 - pos / 10, 0, 20);
    Serial.print("LEFT [");
    for (int i = 0; i <= 20; i++) {
      if (i == slot) Serial.print("#");
      else if (i == 10) Serial.print("|");
      else Serial.print("-");
    }
    Serial.print("] RIGHT   ");

    if (centered)      Serial.println("sun CENTERED");
    else if (pos > 0)  Serial.println("sun on LEFT");
    else               Serial.println("sun on RIGHT");
  }
}
