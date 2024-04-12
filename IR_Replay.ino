#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>

// Uncomment line below your if you're using StickCPlus 1.1
// #define STICK_C_PLUS

#if defined(STICK_C_PLUS)
  #include <M5StickCPlus.h>
#else
  #include <M5StickCPlus2.h>
#endif

#define MAX_RAWBUF_SIZE 100
#define IR_RECV_PIN 33 // Receiver Pin (default for M5Stack official IR module is 33)
#define IR_LED_PIN 26 // Sender Pin (default for M5Stack official IR module is 32)

IRrecv irrecv(IR_RECV_PIN);
IRsend irsend(IR_LED_PIN);

enum States {
  RECEIVER_LISTENING,
  RECEIVER_RECEIVED,
  SENDER_WAITING,
  SENDER_SENDED
};

struct State {
  States currentState;
};

struct TextScreen {
  String title;
  String message;
};

decode_results results;
uint16_t* rawbuf_copy;

int rawDataLen;

State state = {RECEIVER_LISTENING};
TextScreen textScreen;

void setup() {
  rawbuf_copy = new uint16_t[MAX_RAWBUF_SIZE];

  Serial.begin(115200);
  M5.begin();
  M5.Lcd.setRotation(3);
  irrecv.enableIRIn();
  irsend.begin();

  drawScreen();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    if (state.currentState == SENDER_WAITING) sendSignal();
  }

  if (M5.BtnB.wasPressed()) {
    if (state.currentState == RECEIVER_LISTENING) {
      state.currentState = SENDER_WAITING;
    } else {
      state.currentState = RECEIVER_LISTENING;
    }
    drawScreen();
  }

  if (state.currentState == RECEIVER_LISTENING) receiveSignal();
}

uint16_t hexToRGB888(String corHex) {
  long corInt = strtol(corHex.c_str(), NULL, 16);

  int red = (corInt >> 16) & 0xFF;
  int green = (corInt >> 8) & 0xFF;
  int blue = corInt & 0xFF;

  int red8bit = map(red, 0, 255, 0, 31);
  int green8bit = map(green, 0, 255, 0, 63);
  int blue8bit = map(blue, 0, 255, 0, 31);

  return (red8bit << 11) | (green8bit << 5) | blue8bit;
}

void drawScreen() {
  uint16_t color = hexToRGB888("4A4A4A");

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  int rectX = 0;
  int rectY = 0;
  int rectWidth = M5.Lcd.width(); 
  int rectHeight = 40;

  M5.Lcd.fillRect(rectX, rectY, rectWidth, rectHeight, color);

  int textX;

  switch (state.currentState) {
    case RECEIVER_LISTENING:
      textScreen.title = "Receiver Mode";
      textScreen.message = "Waiting IR Signal";
      textX = 42;
      break;
    case RECEIVER_RECEIVED:
      textScreen.title = "Receiver Mode";
      textScreen.message = "IR Received";
      state.currentState = RECEIVER_LISTENING;
      textX = 42;
      break;
    case SENDER_WAITING:
      textScreen.title = "Sender Mode";
      textScreen.message = "Waiting";
      textX = 54;
      break;
    case SENDER_SENDED:
      textScreen.title = "Sender Mode";
      textScreen.message = "IR Sended";
      state.currentState = SENDER_WAITING;
      textX = 54;
      break;
  }

  M5.Lcd.setTextDatum(ML_DATUM);
  M5.Lcd.drawString(textScreen.title, textX, 20);

  M5.Lcd.setTextDatum(MC_DATUM);
  int16_t x = M5.Lcd.width() / 2;
  int16_t y = 70;

  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.drawString(textScreen.message, x, y);
  
  M5.Lcd.setTextColor(0xB4B4B4);
  M5.Lcd.drawString("Press B to switch", x, 110);
}

// Method to receive the IR Signal
void receiveSignal() {
  if (irrecv.decode(&results)) {
    memset(rawbuf_copy, 0, MAX_RAWBUF_SIZE * sizeof(uint16_t));
    rawDataLen = results.rawlen;
    
    // Print in serial monitor the raw and hex data
    Serial.print("Raw Data ");
    Serial.print(rawDataLen);
    Serial.print(" : ");
    for (int i = 1; i < results.rawlen; i++) {
      Serial.print(results.rawbuf[i], DEC);
      if (i < results.rawlen - 1) Serial.print(", ");
      rawbuf_copy[i - 1] = results.rawbuf[i] * 2;
    }
    Serial.println();
    Serial.print("Hex: ");
    Serial.print(results.value, HEX);
    Serial.println();

    state.currentState = RECEIVER_RECEIVED;
    drawScreen();
    delay(1000);
    drawScreen();
    irrecv.resume();  // Waiting for the next IR Code
  }
}

// Method to send/replay the IR Signal (raw)
void sendSignal() {
  state.currentState = SENDER_SENDED;
  drawScreen();
  irsend.sendRaw(rawbuf_copy, rawDataLen, 38); // 38Khz
  delay(1000);
  drawScreen();
}
