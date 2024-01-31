#!/usr/bin/python3
# Shebang line

import sys
import os
import time
import re

from opcua import Client
from opcua import ua

from queue import Queue

if os.name == 'nt':
    # Try to get ANSI color support for Windows
    from ctypes import windll
    kernel32 = windll.kernel32
    kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)

# --------------------------------------------------------------------------------------------------

# ANSI escape codes for color 
RED =                   "\033[31m"
GREEN =                 "\033[32m"
YELLOW =                "\033[33m"
BLUE =                  "\033[34m"
MAGENTA =               "\033[35m"
CYAN =                  "\033[36m"
RESET =                 "\033[0m"

# Internal defines
TREE_END =              "└── "
TREE_MIDDLE =           "├── "
TREE_MAX_LAYERS =       20

DIR_RES =               "res/"
DIR_BUILD =             "build/"

# --------------------------------------------------------------------------------------------------

# SubHandler
class SubHandler(object):
    
    # Subscription Handler. To receive events from server for a subscription
    # data_change and event methods are called directly from receiving thread.
    # Do not do expensive, slow or network operation there. Create another 
    # thread if you need to do such a thing

    def datachange_notification(self, node, val, data):
        print(f"New value from {node}: {val}")

    def event_notification(self, event):
        print("New event", event)

# --------------------------------------------------------------------------------------------------

# Contains the individual opc ua workflow
class Job:
    # Init
    def __init__(self, 
                 argument,
                 job_number = None,
                 parsed_list = None):
        self.argument = argument
        self.job_number = job_number
        self.parsed_list = parsed_list if parsed_list is not None else []
        
    # Parse for different jobs 
    def parse(self):
        self.parsed_list = self.argument.split(':')
        self.print_job()
        # Parse
        if self.parsed_list[0] == "mitem_add":
            self.add_mon_item()
        elif self.parsed_list[0] == "node_read":
            self.read_node()
        elif self.parsed_list[0] == "node_write":
            self.write_node()
        elif self.parsed_list[0] == "browse":
            self.browse()
        elif self.parsed_list[0] == "print":
            self.print_label()
        elif self.parsed_list[0] == "replace":
            self.replace_label()
        else:
            print(f"Entered job type is invalid: {self.parsed_list[0]}")
        return
    
    # Print single job 
    def print_job(self):
        print(f"job {self.job_number}: [ {self.argument} ] = {self.parsed_list}")

    # Add monitored item incl. subscription
    def add_mon_item(self):
        # 1. Node id
        # 2. Namespace
        print(BLUE)
        try:
            if len(self.parsed_list) < 3:
                raise ValueError("Parsed list does not contain enough elements.")
            # Extract/Create node id
            namespace_index = int(self.parsed_list[2])
            identifier = self.parsed_list[1]
            if identifier.isdigit():
                identifier = int(identifier)
            # Get node
            node = printer.get_node(ua.NodeId(identifier, namespace_index))
            print(node)
            handler = SubHandler()
            # 100 ms polling intervall
            sub = printer.create_subscription(100, handler)
            handle = sub.subscribe_data_change(node)
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

    # Read node 
    def read_node(self):
        # 1. Node id
        # 2. Namespace
        print(BLUE)
        try:
            if len(self.parsed_list) < 3:
                raise ValueError("Parsed list does not contain enough elements.")
            # Extract/Create node id
            namespace_index = int(self.parsed_list[2])
            identifier = self.parsed_list[1]
            if identifier.isdigit():
                identifier = int(identifier)
            # Read node
            var = printer.get_node(ua.NodeId(identifier, namespace_index))
            print(var)
            value = var.get_data_value()
            print("Data: ", value)
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

    # Write node     
    def write_node(self):
        # 1. Node id
        # 2. Namespace 
        # 3. - X. Value/values
        print(BLUE)
        try:    
            if len(self.parsed_list) < 4:
                raise ValueError("Parsed list does not contain enough elements.")
            # Extract/Create node id
            value = self.parsed_list[4]
            namespace_index = int(self.parsed_list[2])
            identifier = self.parsed_list[1]
            if identifier.isdigit():
                identifier = int(identifier)
            # Write node
            var = printer.get_node(ua.NodeId(identifier, namespace_index))
            print(var)
            variant_type = var.get_data_type_as_variant_type()
            # Konvert value to data type 
            if variant_type == ua.VariantType.Int16 or \
                variant_type == ua.VariantType.Int32 or \
                variant_type == ua.VariantType.Int64 or \
                variant_type == ua.VariantType.UInt16 or \
                variant_type == ua.VariantType.UInt32 or \
                variant_type == ua.VariantType.UInt64:
                convert = int(value)
            elif variant_type == ua.VariantType.Float:
                convert = float(value)
            elif variant_type == ua.VariantType.String:
                convert = str(value)
            # More data types 
            # elif variant_type == ua.VariantType.SByte:
            # elif variant_type == ua.VariantType.Byte:
            # elif variant_type == ua.VariantType.ByteString:
            # elif variant_type == ua.VariantType.Boolean:
            # elif variant_type == ua.VariantType.DateTime:
            var.set_value(convert, variant_type)
            test = var.get_data_value()
            print("Data: ", test)
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

    # Browse printer nodes     
    def browse(self):
        # Empty
        # 1. Node id
        # 2. Namespace 
        print(BLUE)
        try:    
            if len(self.parsed_list) < 1 or len(self.parsed_list) == 2:
                raise ValueError("Parsed list does not contain enough elements.")
            elif len(self.parsed_list) == 1:
                # Browse from root node 
                root = printer.get_root_node()
            elif len(self.parsed_list) == 3:
                # Browse from entered node
                namespace_index = int(self.parsed_list[2])
                identifier = self.parsed_list[1]
                if identifier.isdigit():
                    identifier = int(identifier)
                root = printer.get_node(ua.NodeId(identifier, namespace_index))
            try:
                # Is filepath?
                if not os.path.exists(DIR_BUILD):
                    os.makedirs(DIR_BUILD)
                # Clear file
                with open(f"{DIR_BUILD}browse.txt", 'w') as file:
                    pass
            except Exception as inner_e:
                print(f"Error: {inner_e}")
            # Print in console 
            print(f"{root} - [ {MAGENTA}VALUE | TYPE | NODECLASS{BLUE} ]")
            # Write in file 
            with open(f"{DIR_BUILD}browse.txt", 'a') as file:  # 'a' append mode
                file.write(f"{root} - [ VALUE | TYPE | NODECLASS ]" + '\n')
            # Nodes browser
            fill = [False] * TREE_MAX_LAYERS
            self.browser(root, 0, fill)
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

    # Browser    
    def browser(self, root, layer, fill):
        fill[layer] = True
        children = root.get_children()
        for i, child in enumerate(children):
            # Filter for last element
            if i == len(children) - 1:
                fill[layer] = False
                self.print_data(child, layer, fill, TREE_END)
                self.browser(child, layer + 1, fill)
                continue
            # Normal node element
            else:
                self.print_data(child, layer, fill, TREE_MIDDLE)
                self.browser(child, layer + 1, fill)
                continue
    
    # Generate browsed output
    def print_data(self, child, layer, fill, präfix):
        string = ""
        for i in range(layer):
            # Fillup empty space
            if fill[i] == True:
                string = string + "│   "
            else:
                string = string + "    "
        # Depends on node type
        try:
            node_data = child.get_data_value()
            value = node_data.Value.Value
            type = node_data.Value.VariantType
            node_class = child.get_node_class()
            console_line = f"{BLUE}{string}{präfix}{child}  [ {MAGENTA}{value} | {type} | {str(node_class)}{BLUE} ]{RESET}"
            file_line = f"{string}{präfix}{child}  [ {value} | {type} | {str(node_class)} ]"
        except:
            console_line = f"{BLUE}{string}{präfix}{child}{RESET}"
            file_line = f"{string}{präfix}{child}"
        # Print in console
        print(console_line)
        # Write in file with 'a' append mode
        if os.name == 'nt':
            with open(f"{DIR_BUILD}browse.txt", 'a', encoding='utf-8') as file: 
                file.write(file_line + '\n')
        elif os.name == 'posix':
            with open(f"{DIR_BUILD}browse.txt", 'a') as file:  
                file.write(file_line + '\n')
    
    # Print label from file or string
    def print_label(self):
        # 1. Syntax language JScript/zpl
        # 2. Syntax or filename 
        print(BLUE)
        try:    
            if len(self.parsed_list) < 3:
                raise ValueError("Parsed list does not contain enough elements.")
            elif len(self.parsed_list) == 3:
                # Work with input contents
                contents = self.parsed_list[2]
                # Is textfile?
                if os.path.isfile(f"{DIR_RES}{contents}"):
                    print(f"Found label {DIR_RES}{contents} in filesystem.")
                    handle = open(f"{DIR_RES}{contents}", "rb")
                    lbl = handle.read()
                    handle.close()
                    contents = lbl  
                # Only print
                elif contents == "PrintNow":
                    # Print current label in buffer
                    print("Print current label in printer buffer.")
                    methods = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","2:MethodSet"])
                    print_current_label = methods.get_child("3:PrintCurrentLabel")
                    methods.call_method(print_current_label, ua.Variant(int(1), ua.VariantType.UInt32))
                    print(RESET)
                    return
                else:
                    # Use entered contents
                    print("Use entered contents.")
                    contents = contents.encode()
                contents += b"\n"  
            elif len(self.parsed_list) == 4:
                # Load argument four cablogo.png in separate function
                self.load_file(self.parsed_list[3])
                # Work with input contents
                contents = self.parsed_list[2]
                # Is textfile?
                if os.path.isfile(f"{DIR_RES}{contents}"):
                    print(f"Found label {DIR_RES}{contents} in filesystem.")
                    handle = open(f"{DIR_RES}{contents}", "rb")
                    lbl = handle.read()
                    handle.close()
                    contents = lbl  
            # JScript or zpl?
            if self.parsed_list[1] == "zpl":
                language = 1
            else:
                language = 0
            try:
                if os.path.isfile(f"{DIR_RES}{contents}"):
                    # Check loading state
                    var = printer.get_node(ua.NodeId(int(10086), int(3)))
                    data_value = var.get_data_value()
                    start_value = data_value.Value.Value
                    # Call method
                    methods = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","2:MethodSet"])
                    print_data = methods.get_child("3:PrintData")
                    methods.call_method(print_data, 
                                        ua.Variant(int(language), ua.VariantType.Int32), 
                                        ua.Variant(contents, ua.VariantType.ByteString))
                    # Polling for loading state
                    while True:
                        var = printer.get_node(ua.NodeId(int(10086), int(3)))
                        data_value = var.get_data_value()
                        end_value = data_value.Value.Value
                        if(int(end_value) > int(start_value)):
                            break
                elif contents == b'f\n':
                    # Send feed
                    print(f"Load found contents: {contents}")
                    methods = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","2:MethodSet"])
                    print_data = methods.get_child("3:PrintData")
                    methods.call_method(print_data, 
                                        ua.Variant(int(language), ua.VariantType.Int32), 
                                        ua.Variant(contents, ua.VariantType.ByteString))
                else:
                    # Send contents 
                    print(f"Load found contents: {contents[:50]}")
                    methods = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","2:MethodSet"])
                    print_data = methods.get_child("3:PrintData")
                    methods.call_method(print_data,
                                        ua.Variant(int(language), ua.VariantType.Int32),
                                        ua.Variant(contents, ua.VariantType.ByteString))
            except Exception as inner_e:
                print(f"Error: {inner_e}")
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

    def load_file(self, file):
        try:
            with open(f"{DIR_RES}{file}", "rb") as handle:
                print(f"Found data {DIR_RES}{file} in filesystem.")
                contents = handle.read()
                # print(f"Type of contents: {type(contents)}")
                # Load file
                methods = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","2:MethodSet"])
                load_data = methods.get_child("3:FileUpload")
                try:
                    methods.call_method(load_data,
                                        ua.Variant(int(1), ua.VariantType.Int32),
                                        ua.Variant(str(file), ua.VariantType.String),
                                        ua.Variant(contents, ua.VariantType.ByteString))
                    # Wait because its not possible to check upload status
                    time.sleep(0.5)
                except Exception as inner_e:
                    print(f"Error, {inner_e}")
        except Exception as e:
            print(f"Error, {e}")

    # Replace contents field
    def replace_label(self):
        # 1. Syntax language JScript/zpl
        # 2. Task, filename or arguments - contentsfiled:replacewith endless with seperator ':'
        print(BLUE)
        try:    
            if len(self.parsed_list) < 3 or len(self.parsed_list) % 2 != 1:
                raise ValueError("Parsed list does not contain enough or invalid number of elements.")
            # Goto current label
            current_label = printer.get_objects_node().get_child(["2:DeviceSet","3:Printer","3:Interpreter","3:Current Label"])
            # Replace contents
            i = 1
            while i < len(self.parsed_list):
                print(f"Replace item {str(self.parsed_list[i])}.")
                # Use parsed arguments to replace contents in childs
                field = str(self.parsed_list[i])
                contents = current_label.get_child(["3:Content Fields", f"3:{field}"])
                i += 1
                contents.set_value(str(self.parsed_list[i]))
                i += 1
        except Exception as e:
            print(f"Error: {e}")
        print(RESET)

# --------------------------------------------------------------------------------------------------

# Processes the input arguments and generates the connection url
class Parser:
    # Init
    def __init__(self, 
                 argv,
                 printer_ip = "192.168.200.70", 
                 printer_port = "4840", 
                 printer_password = "opcpass", 
                 printer_user = "opcuser", 
                 printer_mode = "default", 
                 printer_url = "default", 
                 job_list = None):
        self.arguments = argv
        self.printer_ip = printer_ip
        self.printer_port = printer_port
        self.printer_password = printer_password
        self.printer_user = printer_user
        self.printer_mode = printer_mode
        self.printer_url = printer_url
        self.job_list = job_list if job_list is not None else []

    # Print usage of client
    def print_usage(self):
        with open(f'{DIR_RES}usage.txt', 'r') as file:
            for line in file:
                print(line, end='')

    # Build the client url
    def build_url(self):
        if self.printer_url != "default":
            print("The client uses your entered url.")
        elif self.printer_mode == "user":
            # Secure url
            print("The client uses the secure url.",
                  "If you don't entered all arguments, the default arguments will be used.")
            self.printer_url =  ("opc.tcp://" + 
                                self.printer_user +
                                ":" +
                                self.printer_password + 
                                "@" +
                                self.printer_ip + 
                                ":" +
                                self.printer_port)
        else: 
            # Anonymus url
            print("The client uses the anonymus url.",
                  "If you don't entered all arguments, the default arguments will be used.")
            self.printer_url =  ("opc.tcp://" + 
                                self.printer_ip + 
                                ":" +
                                self.printer_port)
        print(self.printer_url)

    def extractor(self, text):
        match = re.search(r'"([^"]*)"', text)
        if match:
            return match.group(1)
        else:
            print("argument syntax error. try to handle.")
            return text

    # Separate the input arguments  
    def parse(self):  
        # If there are input arguments    
        if len(self.arguments) > 1:
            # Parse 
            i = 1
            while i < len(self.arguments):
                if self.arguments[i] == "-h" or self.arguments[i] == "--help":
                    self.print_usage()
                    sys.exit(0)
                    return self.printer_url, self.job_list
                elif self.arguments[i] == "-url":
                    i += 1
                    self.printer_url = self.arguments[i]
                elif self.arguments[i][:6] == "--url=":
                    self.printer_url = self.arguments[i][6:]
                elif self.arguments[i] == "-u":
                    i += 1
                    self.printer_user = self.arguments[i]
                elif self.arguments[i][:11] == "--username=":
                    self.printer_user = self.arguments[i][11:]
                elif self.arguments[i] == "-p":
                    i += 1
                    self.printer_password = self.arguments[i]
                elif self.arguments[i][:11] == "--password=":
                    self.printer_password = self.arguments[i][11:]
                elif self.arguments[i] == "-P":
                    i += 1
                    self.printer_port = self.arguments[i]
                elif self.arguments[i][:7] == "--port=":
                    self.printer_port = self.arguments[i][7:]
                elif self.arguments[i] == "-I":
                    i += 1
                    self.printer_ip = self.arguments[i]
                elif self.arguments[i][:5] == "--ip=":
                    self.printer_ip = self.arguments[i][5:]
                elif self.arguments[i] == "-m":
                    i += 1
                    self.printer_mode = self.arguments[i]
                elif self.arguments[i][:7] == "--mode=":
                    self.printer_mode = self.arguments[i][7:]
                # parse for jobs 
                elif self.arguments[i] == "-j":
                    i += 1
                    self.job_list.append(self.arguments[i])
                elif self.arguments[i][:6] == "--job=":
                    self.job_list.append(self.arguments[i][6:])
                i += 1
        # Nothing to do 
        else:
            print("Arguments empty. Nothing to do...")
        self.build_url()
        return self.printer_url, self.job_list

# --------------------------------------------------------------------------------------------------

# Import jobs 
def import_jobs(job_list):
    # Queue of jobs
    jobs = Queue()
    # Default if empty
    if len(job_list) == 0:
        jobs.put("node_read:ID_HEAT_LEVEL:4")
    else:
        # If job is filename, import jobs frome file and insert in list jobs[]
        for job in job_list:
            try:
                if os.path.isfile(f"{DIR_RES}{job}"):
                    print(f"Found file {job} in filesystem.")
                    with open(f"{DIR_RES}{job}", 'r') as file:
                        for line in file:
                            # Seperate at '#' - dont read comments
                            line = line.split('#')[0].strip()
                            if line:
                                # Add
                                jobs.put(line)
                else:
                    # Add
                    jobs.put(job)
            except Exception as e:
                print(f"Error: {e}")
        # Default if e.g. textfile jobs.txt is empty
        if jobs.empty():
            jobs.put("node_read:ID_HEAT_LEVEL:4")
    return jobs

# Connect printer
def connect_printer(printer_url):
    i = 10
    run = 0
    while True:    
        try:
            # Connenct
            printer.connect()
            print(f"(Re-)connecting printer...  with: {printer_url}")
            time.sleep(1)
            if check_server_state() == True:
                break
        except Exception as e:
            print(f"Error: {e}")
            # Reconnecting
            run += 1
            if run == 3:
                print("Printer is not available.")
                sys.exit(0)
            else:
                print("Retry to connect in few seconds.")
                time.sleep(i * run)  

# Check opc ua server status
def check_server_state():
    try:
        printer.get_objects_node().get_child(["0:Server","0:ServerStatus","0:State"])
        return True
    except:
        print("Server is not ready for communication.") 
        return False 

# --------------------------------------------------------------------------------------------------

# Main
if __name__ == "__main__":\

    parser = Parser(sys.argv)
    printer_url, job_list = parser.parse()
       
    printer = Client(printer_url)
    try:
        # Connect and init
        jobs = import_jobs(job_list)
        connect_printer(printer_url)
        try:
            while True:
                # Workspace 
                if jobs.empty():
                    # Waiting animation
                    if int(time.time() * 1000) % 2 == 0:
                        print("Polling...nothing to do ¯\\_(ツ)_/¯. CTRL+C to end. \\", end="\r")
                    else:
                        print("Polling...nothing to do ¯\\_(ツ)_/¯. CTRL+C to end. /", end="\r")
                index = 0
                while not jobs.empty():
                    try:
                        # FIFO polling jobs
                        object = Job(jobs.get(), index)
                        object.parse()
                    except IndexError as e:
                        print(f"Error work with job {index}: {e}")
                        continue
                    index += 1
                time.sleep(0.2)
        # Keyboard interrupt CTRL+C
        except KeyboardInterrupt:
            print("\nControlled client shutdown.")
            # Disconnect
            printer.disconnect()
    except:
        print("Fatal error!")

    finally:
        # Nothing to do 
        print("Disconnect client, bye!")

# Eof