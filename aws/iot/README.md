The following lab assumes you have the Arduino IDE installed, and can install the necessary libraries and board.
Tested with an MKR-1000.

Let's start with the [base_wifi.ino](https://github.com/robertpyke/Tutorials/blob/master/aws/iot/base_wifi.ino) and
[arduino_secrets.h](https://github.com/robertpyke/Tutorials/blob/master/aws/iot/arduino_secrets.h) for our initial sketch.

The following is a brief explanation of each part of the initial base_wifi.ino:

Libraries/constants:

```
#include "arduino_secrets.h"
#include <SPI.h>
#include <WiFi101.h>
```

The arduino_secrets.h file contains our constants, 


