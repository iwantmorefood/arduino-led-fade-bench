# Arduino LED Fade Bench

Set an LED's PWM duty cycle or run a repeating fade from the Arduino Serial Monitor. This small sketch is intended for bench checks and introductory PWM experiments on the Arduino Uno R3 and classic Nano.

## Quick start

1. Open [LedFadeBench.ino](LedFadeBench/LedFadeBench.ino) in the Arduino IDE. Keep it in the `LedFadeBench` folder so the folder and sketch names match.
2. With the board unpowered, connect **D9 → 330 Ω resistor → LED anode**, and connect the LED cathode to **GND**. This circuit is for a standard indicator LED powered by the board's pin.
3. Connect the board over USB, select the board and port in the IDE, and upload the sketch. The LED starts off.
4. Open Serial Monitor at **115200 baud** and select **Newline** or **Both NL & CR**. Send `help` to list commands.

## Commands

| Command | Result |
| --- | --- |
| `level 128` | Set a fixed PWM value from 0 to 255. A value of 128 is approximately 50% duty cycle. |
| `fade 2000` | Fade from off to full and back in 2 seconds. The full cycle can be 200–60,000 ms. |
| `stop` | Stop the fade and hold the current output level. |
| `status` | Print the mode, current PWM level, and configured fade period. |
| `help` | List the available commands and ranges. |

Try `level 0`, `level 128`, and `level 255` to compare fixed outputs. Then send `fade 2000`, followed by `stop` while the LED is changing. The LED should hold its brightness until you change the mode again.

The sketch computes its triangular fade from elapsed `millis()` ticks, so it keeps reading serial input while the LED fades. PWM duty cycle changes linearly, while perceived brightness depends on the LED and the observer.

For a bug report, include your board model, Arduino core version, the command you sent, and the Serial Monitor output.

## License

[MIT](LICENSE)
