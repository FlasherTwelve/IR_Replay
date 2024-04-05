
# IR_Replay

Firmware for Replay IR codes using an M5Stack Mini Infrared Emitter & Receiver Unit or another emitter/transmitter. Using another emitter/transmitter requires changes to the pins in the code.

## How to use

Using the firmware is very simple. After capturing the IR you want to replicate, press the B button on your Stick, this way, you will switch to sender mode. From sender mode, you can send the captured IR from button A, you can send the IR as many times as you want, there is no need to capture the code again. To change the mode again, simply press button B again.

## Using custom emitter/transmitter

In the main .ino file, change the following constants:


```
#define IR_RECV_PIN 33 // Receiver Pin
#define IR_LED_PIN 32 // Sender Pin
```

If you're using the Mini Infrared Emitter & Receiver Unit from M5Stack, you can use ports 33 (Receiver) and 32 (Sender). In case you are using a transmitter like the one in the image below, you can set the transmitter pin to 26.

![IR Transmitter](https://iili.io/JNNO7TX.md.png)

## Contribution

Any contribution is welcome! The firmware is simple, but it can become something bigger.