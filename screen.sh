#!/bin/bash

tty=/dev/ttyACM0

cd "$(dirname "$0")"

touch screen.log
echo -n "[$(date "+%Y-%m-%d %H:%M:%S")] " >> screen.log

screen -dmS bootUnstuckDongle -L -Logfile screen.log $tty 9600
