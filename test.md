# OPC UA client test documentation of functionality.

## Python Linux 

| Task | Check or comment |
|--------------------|----------------------|
| ./client.py | io |
| ./client.py -h   | io |
| ./client.py --help | io |
| ./client.py -url opc.tcp://192.168.200.70:4840 | io |
| ./client.py --url=opc.tcp://192.168.200.70:4840 | io |
| ./client.py -url opc.tcp://opcuser:opcpass@192.168.200.70:4840 | io |
| ./client.py --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840 | io |
| ./client.py -I 192.168.200.70 -P 4840 | io |
| ./client.py --ip=192.168.200.70 --port=4840 | io |
| ./client.py -u opcuser -p opcpass -m user | io |
| ./client.py --user=opcuser --password=opcpass --mode=user | io |
| ./client.py -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user | io |
| ./client.py --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user  | io |
| ./client.py -j print:JScript:f | io |
| ./client.py --job=print:JScript:f | io |
| ./client.py -j jobs.txt | io |
| ./client.py --job=jobs.txt | io |
| ./client.py -j mitem_add:ID_CLOCK_TIME:4 | Shows the possibility - override terminal output  |
| ./client.py --job=mitem_add:ID_CLOCK_TIME:4 | Shows the possibility - override terminal output |
| ./client.py -j node_read:ID_HEAT_LEVEL:4 | io |
| ./client.py --job=node_read:ID_HEAT_LEVEL:4 | io |
| ./client.py -j node_write:ID_HEAT_LEVEL:4:int32_t:6 | io |
| ./client.py --job=node_write:ID_HEAT_LEVEL:4:int32_t:6 | io |
| ./client.py -j browse:ID_RIBBON:4 | io |
| ./client.py --job=browse:ID_RIBBON:4 | io |
| ./client.py -j print:JScript:label.txt:cablogo.png | io |
| ./client.py --job=print:JScript:label.txt:cablogo.png | io |
| ./client.py -j print:JScript:label.txt | io |
| ./client.py --job=print:JScript:label.txt | io |
| ./client.py -j print:JScript:replace.txt -j print:JScript:PrintNow | io |
| ./client.py --job=print:JScript:replace.txt --job=print:JScript:PrintNow | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow | io |

## Python Windows

| Task | Check or comment |
|--------------------|----------------------|
| Without arguments | thisisacomment |
| -h   | thisisacomment |
| --help | thisisacomment |
| -url opc.tcp://192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://192.168.200.70:4840 | thisisacomment |
| -url opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| -I 192.168.200.70 -P 4840 | thisisacomment |
| --ip=192.168.200.70 --port=4840 | thisisacomment |
| -u opcuser -p opcpass -m user | thisisacomment |
| --user=opcuser --password=opcpass --mode=user | thisisacomment |
| -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user | thisisacomment |
| --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user  | thisisacomment |
| -j print:JScript:f | thisisacomment |
| --job=print:JScript:f | thisisacomment |
| -j jobs.txt | thisisacomment |
| --job=jobs.txt | thisisacomment |
| -j mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| --job=mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| -j node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| --job=node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| -j node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| --job=node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| -j browse:ID_RIBBON:4 | thisisacomment |
| --job=browse:ID_RIBBON:4 | thisisacomment |
| -j print:JScript:label.txt | thisisacomment |
| --job=print:JScript:label.txt | thisisacomment |
| -j print:JScript:label.txt:cablogo.png | thisisacomment |
| --job=print:JScript:label.txt:cablogo.png | thisisacomment |
| -j print:JScript:replace.txt -j print:JScript:PrintNow | thisisacomment |
| --job=print:JScript:replace.txt --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow | thisisacomment |

## C++ Linux

| Task | Check or comment |
|--------------------|----------------------|
| Without arguments | thisisacomment |
| -h   | thisisacomment |
| --help | thisisacomment |
| -url opc.tcp://192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://192.168.200.70:4840 | thisisacomment |
| -url opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| -I 192.168.200.70 -P 4840 | thisisacomment |
| --ip=192.168.200.70 --port=4840 | thisisacomment |
| -u opcuser -p opcpass -m user | thisisacomment |
| --user=opcuser --password=opcpass --mode=user | thisisacomment |
| -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user | thisisacomment |
| --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user  | thisisacomment |
| -j print:JScript:f | thisisacomment |
| --job=print:JScript:f | thisisacomment |
| -j jobs.txt | thisisacomment |
| --job=jobs.txt | thisisacomment |
| -j mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| --job=mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| -j node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| --job=node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| -j node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| --job=node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| -j browse:ID_RIBBON:4 | thisisacomment |
| --job=browse:ID_RIBBON:4 | thisisacomment |
| -j print:JScript:label.txt | thisisacomment |
| --job=print:JScript:label.txt | thisisacomment |
| -j print:JScript:label.txt:cablogo.png | thisisacomment |
| --job=print:JScript:label.txt:cablogo.png | thisisacomment |
| -j print:JScript:replace.txt -j print:JScript:PrintNow | thisisacomment |
| --job=print:JScript:replace.txt --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow | thisisacomment |

## C++ Windows

| Task | Check or comment |
|--------------------|----------------------|
| Without arguments | thisisacomment |
| -h   | thisisacomment |
| --help | thisisacomment |
| -url opc.tcp://192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://192.168.200.70:4840 | thisisacomment |
| -url opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840 | thisisacomment |
| -I 192.168.200.70 -P 4840 | thisisacomment |
| --ip=192.168.200.70 --port=4840 | thisisacomment |
| -u opcuser -p opcpass -m user | thisisacomment |
| --user=opcuser --password=opcpass --mode=user | thisisacomment |
| -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user | thisisacomment |
| --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user  | thisisacomment |
| -j print:JScript:f | thisisacomment |
| --job=print:JScript:f | thisisacomment |
| -j jobs.txt | thisisacomment |
| --job=jobs.txt | thisisacomment |
| -j mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| --job=mitem_add:ID_CLOCK_TIME:4 | thisisacomment |
| -j node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| --job=node_read:ID_HEAT_LEVEL:4 | thisisacomment |
| -j node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| --job=node_write:ID_HEAT_LEVEL:4:int32_t:6 | thisisacomment |
| -j browse:ID_RIBBON:4 | thisisacomment |
| --job=browse:ID_RIBBON:4 | thisisacomment |
| -j print:JScript:label.txt | thisisacomment |
| --job=print:JScript:label.txt | thisisacomment |
| -j print:JScript:label.txt:cablogo.png | thisisacomment |
| --job=print:JScript:label.txt:cablogo.png | thisisacomment |
| -j print:JScript:replace.txt -j print:JScript:PrintNow | thisisacomment |
| --job=print:JScript:replace.txt --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow | thisisacomment |
| -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow | thisisacomment |
| --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow | thisisacomment |

<!-- Eof -->