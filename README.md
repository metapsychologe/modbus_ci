# modbus_ci
Command line interface for modbus using libmodbus

you can for example read registeres with the line

``` bash
modbus_read_register -h 192.168.0.1 -p 602 -s 40000 -n 125
```
to read 125 holding registers starting from address 40000 from 192.168.0.1:602

current supported flags:
``` bash
-p --port host port, default is 502
-h --host server host name defaults to localhost
-u --unit_id used unit id to use, defaults to 0xff as suggested by ModbusTCP
-s --address first modbus register address to read from
-n number of registers to read
-v verbose mode
-x --hex hexadecimal output
-H --help print this help message
```

