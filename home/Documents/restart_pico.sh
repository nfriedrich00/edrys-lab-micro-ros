avrdude -v -p atmega328p -c arduino -P /dev/arduino_uno -b 57600 -D -U flash:w:/home/appuser/Documents/restart_pico.hex:i &
AVRDUDE_PID=$!

sleep 1

kill $AVRDUDE_PID

