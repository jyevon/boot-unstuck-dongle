# Linux boot-unstuck dongle with Arduino
This repository contains Arduino code for a gadget that shouldn't be needed in the first place: A rubber ducky that tries to safely reboot a Linux system using the [REISUB key sequence](https://wiki.archlinux.org/title/Keyboard_shortcuts#Rebooting) if startup didn't finish within a timeout.

__Please note:__ A reboot of the system is not detected unless the dongle temporarily looses its power. Therefore, you should prefer power cycling the system when you don't want to miss the dongle's function.

## Requirements
- Linux system that tends to get stuck during loading the Kernel or starting the OS
- Arduino board (or equivalent) that is [supported by the Keyboard.h library](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/#_compatible_hardware), e.g. Arduino Micro or Pro Micro
  - If your board doesn't have a built-in, controllable LED: Add an LED with adequate resistor
- USB cable to connect the two

## Set up the linux system
In order to cancel the reboot timeout, the PC needs to open a serial connection to the Arduino. That signals a sucessful startup.

You can determine the tty name of the Arduino and use, e.g., crontab and screen to open the serial connection automatically on startup:

0. __Do not connect the Arduino yet.__

1. Determine the tty that corresponds to your Arduino:
    - List the available ttys on the system in file: `ls -l /dev/tty* > ttys-before.txt`
    - Connect the Arduino.
    - Compare with ttys on the system now: `ls -l /dev/tty* | diff ttys-before.txt -`
    - Note the newly added tty. This is typically `/dev/ttyACM0` or `/dev/ttyUSB0`.
    - You might want to delete the file created: `rm ttys-before.txt`

2. Test serial connection with that tty:
    - Open serial port with screen: `sudo screen /dev/ttyACM0 9600`
    - You should now see the Arduino's response:
`Reboot prevented. X seconds were left.`
    - Exit the screen session: Press <kbd>Ctrl</kbd> + <kbd>A</kbd> followed by <kbd>K</kbd> and <kbd>Y</kbd>.

3. Add entry to root crontab:
    - Open root's crontab: `sudo crontab -e`
    - Add a line `@reboot` for `screen -dmS bootUnstuckDongle /dev/ttyACM0 9600` and save the file.  
    Example:
```cron
# Edit this file to introduce tasks to be run by cron.
#
# [...]
#
# m h  dom mon dow   command
@reboot              screen -dmS bootUnstuckDongle /dev/ttyACM0 9600
```

You might use minicom or picocom as an alternative to screen for the serial connection. Instead of crontab you could, e.g., create a systemd service or add a script to `/etc/init.d/`. If you come up with an alternative setup, consider sharing it in a pull request so that instructions can be added here.

## Dongle states
| No. | LED indicator         | Dongle state                                           | Following state |
|:---:| --------------------- | ------------------------------------------------------ |:---------------:|
| 1   | > 1 s on              | waiting for serial connection or timeout               | 2 or 4          |
| 2   | > 1 s off             | issuing REISUB key sequence                            | 3               |
| 3   | blink 2 times         | REISUB issued                                          | 1               |
| 4   | blink until power off | serial connection established (once), reboot cancelled | -               |
