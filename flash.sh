python ~/Share/esptool/esptool.py --baud 115200 --port /dev/ttyUSB0 write_flash 0x00000 bin/eagle.flash.bin 0x10000 bin/eagle.irom0text.bin
