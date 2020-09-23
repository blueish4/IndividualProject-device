# Room noise monitor - Device

This is a section of my final year project which aims to create a system to monitor ambient
noise in a given environment. A summary of the project can be found at https://blog.blueish.me/posts/dissertation/

## Setup

This project used platform.io as a framework to compile code to an ESP32. The specific board
used is the Heltec WiFi LoRa v1 board, but it shouldn't be too difficult to configure for
any other esp32 board. It can connect to both WPA2-PSK and WPA2-Enterprise networks, and
details should be supplied in [`lib/transmit/config.h`](./lib/transmit/config.h).

It also uses Google's MQTT service, which should be configured as per the documentation.
[`lib/transmit/ciotc_config.h`](./lib/transmit/ciotc_config.h) should contain the device's
private key and the current X.509 certificate for the google service in your area
(instructions are provided above the config options).

## Methodology

Once connected to wifi, the board samples enough audio samples, performs an FFT on them, then
repeats the process a set defined number of times. A coarse octave grouping of the bins is
then uploaded to Google in a packed binary format (double,double, ...uint16).

## Further expansion

I was unable to get the LoRa radio on the board working consistently and reliably, but it
seems an easy option to expand the project.
