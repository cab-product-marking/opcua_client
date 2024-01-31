# OPC UA client test documentation of functionality.

## Python Linux 

| Task | Check or comment |
|--------------------|----------------------|
| ./client.py                                                                                                                               | io |
| ./client.py -h                                                                                                                            | io |
| ./client.py --help                                                                                                                        | io |
| ./client.py -url opc.tcp://192.168.200.70:4840                                                                                            | io |
| ./client.py --url=opc.tcp://192.168.200.70:4840                                                                                           | io |
| ./client.py -url opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                            | io |
| ./client.py --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                           | io |
| ./client.py -I 192.168.200.70 -P 4840                                                                                                     | io |
| ./client.py --ip=192.168.200.70 --port=4840                                                                                               | io |
| ./client.py -u opcuser -p opcpass -m user                                                                                                 | io |
| ./client.py --user=opcuser --password=opcpass --mode=user                                                                                 | io |
| ./client.py -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user                                                                       | io |
| ./client.py --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user                                                 | io |
| ./client.py -j print:JScript:f                                                                                                            | io |
| ./client.py --job=print:JScript:f                                                                                                         | io |
| ./client.py -j jobs.txt                                                                                                                   | io |
| ./client.py --job=jobs.txt                                                                                                                | io |
| ./client.py -j mitem_add:ID_CLOCK_TIME:4                                                                                                  | Shows the possibility - override terminal output |
| ./client.py --job=mitem_add:ID_CLOCK_TIME:4                                                                                               | Shows the possibility - override terminal output |
| ./client.py -j node_read:ID_HEAT_LEVEL:4                                                                                                  | io |
| ./client.py --job=node_read:ID_HEAT_LEVEL:4                                                                                               | io |
| ./client.py -j node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                       | io |
| ./client.py --job=node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                    | io |
| ./client.py -j browse:ID_RIBBON:4                                                                                                         | io |
| ./client.py --job=browse:ID_RIBBON:4                                                                                                      | io |
| ./client.py -j print:JScript:label.txt:cablogo.png                                                                                        | io |
| ./client.py --job=print:JScript:label.txt:cablogo.png                                                                                     | io |
| ./client.py -j print:JScript:label.txt                                                                                                    | io |
| ./client.py --job=print:JScript:label.txt                                                                                                 | io |
| ./client.py -j print:JScript:replace.txt -j print:JScript:PrintNow                                                                        | io |
| ./client.py --job=print:JScript:replace.txt --job=print:JScript:PrintNow                                                                  | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow                                             | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow                                       | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow          | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow    | io |

## Python Windows

| Task | Check or comment |
|--------------------|----------------------|
| ./client.py                                                                                                                               | io |
| ./client.py -h                                                                                                                            | io |
| ./client.py --help                                                                                                                        | io |
| ./client.py -url opc.tcp://192.168.200.70:4840                                                                                            | io |
| ./client.py --url=opc.tcp://192.168.200.70:4840                                                                                           | io |
| ./client.py -url opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                            | io |
| ./client.py --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                           | io |
| ./client.py -I 192.168.200.70 -P 4840                                                                                                     | io |
| ./client.py --ip=192.168.200.70 --port=4840                                                                                               | io |
| ./client.py -u opcuser -p opcpass -m user                                                                                                 | io |
| ./client.py --user=opcuser --password=opcpass --mode=user                                                                                 | io |
| ./client.py -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user                                                                       | io |
| ./client.py --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user                                                 | io |
| ./client.py -j print:JScript:f                                                                                                            | io |
| ./client.py --job=print:JScript:f                                                                                                         | io |
| ./client.py -j jobs.txt                                                                                                                   | io |
| ./client.py --job=jobs.txt                                                                                                                | io |
| ./client.py -j mitem_add:ID_CLOCK_TIME:4                                                                                                  | Shows the possibility - override terminal output |
| ./client.py --job=mitem_add:ID_CLOCK_TIME:4                                                                                               | Shows the possibility - override terminal output |
| ./client.py -j node_read:ID_HEAT_LEVEL:4                                                                                                  | io |
| ./client.py --job=node_read:ID_HEAT_LEVEL:4                                                                                               | io |
| ./client.py -j node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                       | io |
| ./client.py --job=node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                    | io |
| ./client.py -j browse:ID_RIBBON:4                                                                                                         | io |
| ./client.py --job=browse:ID_RIBBON:4                                                                                                      | io |
| ./client.py -j print:JScript:label.txt:cablogo.png                                                                                        | io |
| ./client.py --job=print:JScript:label.txt:cablogo.png                                                                                     | io |
| ./client.py -j print:JScript:label.txt                                                                                                    | io |
| ./client.py --job=print:JScript:label.txt                                                                                                 | io |
| ./client.py -j print:JScript:replace.txt -j print:JScript:PrintNow                                                                        | io |
| ./client.py --job=print:JScript:replace.txt --job=print:JScript:PrintNow                                                                  | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow                                             | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow                                       | io |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow          | io |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow    | io |

## C++ Linux

| Task | Check or comment |
|--------------------|----------------------|
| ./client.py                                                                                                                               | comment |
| ./client.py -h                                                                                                                            | comment |
| ./client.py --help                                                                                                                        | comment |
| ./client.py -url opc.tcp://192.168.200.70:4840                                                                                            | comment |
| ./client.py --url=opc.tcp://192.168.200.70:4840                                                                                           | comment |
| ./client.py -url opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                            | comment |
| ./client.py --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                           | comment |
| ./client.py -I 192.168.200.70 -P 4840                                                                                                     | comment |
| ./client.py --ip=192.168.200.70 --port=4840                                                                                               | comment |
| ./client.py -u opcuser -p opcpass -m user                                                                                                 | comment |
| ./client.py --user=opcuser --password=opcpass --mode=user                                                                                 | comment |
| ./client.py -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user                                                                       | comment |
| ./client.py --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user                                                 | comment |
| ./client.py -j print:JScript:f                                                                                                            | comment |
| ./client.py --job=print:JScript:f                                                                                                         | comment |
| ./client.py -j jobs.txt                                                                                                                   | comment |
| ./client.py --job=jobs.txt                                                                                                                | comment |
| ./client.py -j mitem_add:ID_CLOCK_TIME:4                                                                                                  | comment |
| ./client.py --job=mitem_add:ID_CLOCK_TIME:4                                                                                               | comment |
| ./client.py -j node_read:ID_HEAT_LEVEL:4                                                                                                  | comment |
| ./client.py --job=node_read:ID_HEAT_LEVEL:4                                                                                               | comment |
| ./client.py -j node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                       | comment |
| ./client.py --job=node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                    | comment |
| ./client.py -j browse:ID_RIBBON:4                                                                                                         | comment |
| ./client.py --job=browse:ID_RIBBON:4                                                                                                      | comment |
| ./client.py -j print:JScript:label.txt:cablogo.png                                                                                        | comment |
| ./client.py --job=print:JScript:label.txt:cablogo.png                                                                                     | comment |
| ./client.py -j print:JScript:label.txt                                                                                                    | comment |
| ./client.py --job=print:JScript:label.txt                                                                                                 | comment |
| ./client.py -j print:JScript:replace.txt -j print:JScript:PrintNow                                                                        | comment |
| ./client.py --job=print:JScript:replace.txt --job=print:JScript:PrintNow                                                                  | comment |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow                                             | comment |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow                                       | comment |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow          | comment |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow    | comment |

## C++ Windows

| Task | Check or comment |
|--------------------|----------------------|
| ./client.py                                                                                                                               | comment |
| ./client.py -h                                                                                                                            | comment |
| ./client.py --help                                                                                                                        | comment |
| ./client.py -url opc.tcp://192.168.200.70:4840                                                                                            | comment |
| ./client.py --url=opc.tcp://192.168.200.70:4840                                                                                           | comment |
| ./client.py -url opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                            | comment |
| ./client.py --url=opc.tcp://opcuser:opcpass@192.168.200.70:4840                                                                           | comment |
| ./client.py -I 192.168.200.70 -P 4840                                                                                                     | comment |
| ./client.py --ip=192.168.200.70 --port=4840                                                                                               | comment |
| ./client.py -u opcuser -p opcpass -m user                                                                                                 | comment |
| ./client.py --user=opcuser --password=opcpass --mode=user                                                                                 | comment |
| ./client.py -I 192.168.200.70 -P 4840 -u opcuser -p opcpass -m user                                                                       | comment |
| ./client.py --ip=192.168.200.70 --port=4840 --user=opcuser --password=opcpass --mode=user                                                 | comment |
| ./client.py -j print:JScript:f                                                                                                            | comment |
| ./client.py --job=print:JScript:f                                                                                                         | comment |
| ./client.py -j jobs.txt                                                                                                                   | comment |
| ./client.py --job=jobs.txt                                                                                                                | comment |
| ./client.py -j mitem_add:ID_CLOCK_TIME:4                                                                                                  | comment |
| ./client.py --job=mitem_add:ID_CLOCK_TIME:4                                                                                               | comment |
| ./client.py -j node_read:ID_HEAT_LEVEL:4                                                                                                  | comment |
| ./client.py --job=node_read:ID_HEAT_LEVEL:4                                                                                               | comment |
| ./client.py -j node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                       | comment |
| ./client.py --job=node_write:ID_HEAT_LEVEL:4:int32_t:6                                                                                    | comment |
| ./client.py -j browse:ID_RIBBON:4                                                                                                         | comment |
| ./client.py --job=browse:ID_RIBBON:4                                                                                                      | comment |
| ./client.py -j print:JScript:label.txt:cablogo.png                                                                                        | comment |
| ./client.py --job=print:JScript:label.txt:cablogo.png                                                                                     | comment |
| ./client.py -j print:JScript:label.txt                                                                                                    | comment |
| ./client.py --job=print:JScript:label.txt                                                                                                 | comment |
| ./client.py -j print:JScript:replace.txt -j print:JScript:PrintNow                                                                        | comment |
| ./client.py --job=print:JScript:replace.txt --job=print:JScript:PrintNow                                                                  | comment |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client" -j print:JScript:PrintNow                                             | comment |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client" --job=print:JScript:PrintNow                                       | comment |
| ./client.py -j "replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" -j print:JScript:PrintNow          | comment |
| ./client.py --job="replace:REPLACE:replace REPLACE with OPC UA Client:CONTENTS:more contents in CONTENTS" --job=print:JScript:PrintNow    | comment |

<!-- Eof -->