# Linux boot-unstuck dongle with Arduino
This repository contains Arduino code for a gadget that shouldn't be needed in the first place: A rubber ducky that tries to safely reboot a Linux system using the [REISUB key sequence](https://wiki.archlinux.org/title/Keyboard_shortcuts#Rebooting) if startup didn't finish within a timeout.

__Please note:__ A reboot of the system is not detected unless the dongle temporarily looses its power. Therefore, you should prefer power cycling the system when you don't want to miss the dongle's function.

## Requirements
- Linux system that tends to get stuck during loading the Kernel or starting the OS
- Arduino board (or equivalent) that is [supported by the Keyboard.h](https://www.arduino.cc/reference/en/language/functions/usb/keyboard/#_compatible_hardware) and [rocketscream/Low-Power](https://github.com/rocketscream/Low-Power) libraries, e.g. Arduino Micro or Pro Micro
  - If your board doesn't have a built-in, controllable LED: Add an LED with adequate resistor
  - You might need to adapt `LED_PIN` to your board.
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
`Reboot prevented. X seconds were left. REISUB had been issued Y times since power on.`
    - Exit the screen session: Press <kbd>Ctrl</kbd> + <kbd>A</kbd> followed by <kbd>K</kbd> and <kbd>Y</kbd>.

3. Prepare [`screen.sh`](screen.sh) on the system:
    - Create and change to a directory where it can remain: `mkdir boot-unstuck-dongle && cd boot-unstuck-dongle`
    - Clone the repository: `git clone https://github.com/jyevon/boot-unstuck-dongle.git .`
    - Or download the single file instead: `wget https://github.com/jyevon/boot-unstuck-dongle/raw/main/screen.sh`
    - Open `screen.sh` with a text editor and change `tty=/dev/ttyACM0` to the tty determined above: `nano screen.sh`

4. Add entry to root crontab:
    - Open root's crontab: `sudo crontab -e`
    - Add a line `@reboot` for `bash /path/to/boot-unstuck-dongle/screen.sh` and save the file.  
    Example:
```cron
# Edit this file to introduce tasks to be run by cron.
#
# [...]
#
# m h  dom mon dow   command
@reboot              bash /home/user/boot-unstuck-dongle/screen.sh
```

`screen.sh` will create a log file within its parent directory. Within this log, you can e.g. look up how many REISUBs it took to successfully boot the system.

You might use minicom or picocom as an alternative to screen for the serial connection. Instead of crontab you could, e.g., create a systemd service or add a script to `/etc/init.d/`. If you come up with an alternative setup, consider sharing it in a pull request so that instructions can be added here.

## Dongle states
| No. | LED indicator     | Dongle state                                           | Following state |
|:---:| ------------------| ------------------------------------------------------ |:---------------:|
| 1   | blink (1s period) | waiting for serial connection or timeout               | 2 or 3          |
| 2   | > 0,5 s on        | issuing REISUB key sequence                            | 1               |
| 3   | > 0,5 s off       | serial connection established (once), reboot cancelled | -               |
