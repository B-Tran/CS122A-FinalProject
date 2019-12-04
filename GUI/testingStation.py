import tkinter as tk
from tkinter.font import Font
from tkinter import ttk
import time
import datetime
import serial
import re
import requests
from requests.exceptions import HTTPError
from digi.xbee.devices import *


class xbeeDevice():
    def __init__(self,name):
        self.name = name
        deviceList.append(self)
        deviceNames.append(name)

textWindow = None
deviceList = []
deviceNames = []
listCount = 0
base = XBeeDevice("/dev/serial0", 9600)

base.open()

xnet = base.get_network()
xnet.set_discovery_timeout(10) #10 seconds before a timeout occurs
xnet.set_discovery_options({DiscoveryOptions.APPEND_DD, DiscoveryOptions.DISCOVER_MYSELF})

xnet.clear()

def network_string(network):
    if network.has_devices():
        return('\n'.join(map(str,network.get_devices())))
    else:
        return("None")

def callback_device_discovered(remote):
    print("Device discovered: %s" % remote)

def callback_discovery_finished(status):
    if status == NetworkDiscoveryStatus.SUCCESS:
        print("Discovery process finished successfully.")
    else:
        print("There was an error discovering devices %s" % status.description)

def print_network(network):
    if network.has_devices():
        print("%s" % '\n   '.join(map(str, network.get_devices())))
    else:
        print("None")

def scan_network(network):
    base.send_data_broadcast(b'1')
    network.start_discovery_process()
    print("Beginning the network search")
    while xnet.is_discovery_running():
        time.sleep(0.1)
    xmap = map(str,network.get_devices())
    for i in xmap:
        i = re.sub('[A-Z0-9]+\ \-\ ', '', i)
        if i != '':
            xbeeDevice(str(i))

def stop_scan():
    xnet.stop_discovery_process()

def update(textWin):
    textWin.delete('1.0', tk.END)
    textWin.insert(tk.END, network_string(xnet))


xnet.add_device_discovered_callback(callback_device_discovered)
xnet.add_discovery_process_finished_callback(callback_discovery_finished)

class mainMenu(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        tk.Tk.wm_title(self, "ABT Home System")
        container = tk.Frame(self)
        
        container.pack(side="top", fill="both", expand = True)
        container.grid_rowconfigure(0, weight=1)
        container.grid_columnconfigure(0,weight=1)

        self.frames = {}
        
        for F in (StartPage, PageOne, PageTwo,PageThree,PageFour, PageFive):
            frame = F(container, self)
            self.frames[F] = frame
            frame.grid(row=0,column=0,sticky="nsew")
        
        self.show_frame(StartPage)
        
    def show_frame(self, cont):
        frame = self.frames[cont]
        frame.tkraise()

class StartPage(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self, parent)
        label = tk.Label(self, text="Main Menu")
        label.pack()

        button1 = tk.Button(self, text="List Network", height = 3, command= lambda: controller.show_frame(PageOne))
        button1.pack(fill=tk.X, pady=4)
        button2 = tk.Button(self, text="Remove Devices", height = 3, command= lambda: controller.show_frame(PageTwo))
        button2.pack(fill=tk.X, pady=4)
        button3 = tk.Button(self, text="Add Devices", height = 3, command= lambda: controller.show_frame(PageThree))
        button3.pack(fill=tk.X, pady=4)

        button4 = tk.Button(self, text="Select Device(s)", height = 3, command=lambda: controller.show_frame(PageFive))
        button4.pack(fill=tk.X, pady=4)

        button5 = tk.Button(self, text="Scan for Devices", height = 3, command= lambda: controller.show_frame(PageFour))
        button5.pack(fill=tk.X, pady=4)
        button6 = tk.Button(self, text="Exit", height = 3, command= lambda: controller.quit())
        button6.pack(fill=tk.X, pady=4)

class PageOne(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = ttk.Label(self, text="Display Network Menu")
        label.pack()

        button6 = tk.Button(self, text="Refresh text", height = 1, width = 47, command= lambda: update(textWindow))
        button6.pack()
        textWindow = tk.Text(self, height= 4, width=50)
        textWindow.pack(anchor='center')
        button1 = tk.Button(self, text="Back to Main Menu", height = 2, command= lambda: controller.show_frame(StartPage))
        button1.pack(fill = tk.X, pady = 5)
        button3 = tk.Button(self, text="Remove Devices", height = 2, command= lambda: controller.show_frame(PageTwo))
        button3.pack(fill = tk.X, pady= 5)
        button4 = tk.Button(self, text="Add Devices", height = 2, command= lambda: controller.show_frame(PageThree))
        button4.pack(fill = tk.X, pady = 5)
        button5 = tk.Button(self, text="Scan Network", height = 2, command= lambda: controller.show_frame(PageFour))
        button5.pack(fill = tk.X, pady = 5)
        button6 = tk.Button(self, text="Select Device", height = 2, command= lambda: controller.show_frame(PageFive))
        button6.pack(fill = tk.X, pady = 5)

class PageTwo(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        text = tk.StringVar()
        label = tk.Label(self, text="Remove Device Menu")
        label.pack()

        label1 = ttk.Label(self, text="Enter the device identifier to remove below")
        label1.pack(pady = 10)
        entry = ttk.Entry(self, width = 30)
        entry.pack(pady = 10)
        label2 = ttk.Label(self, textvariable=text)
        label2.pack(pady = 5)
        button5 = tk.Button(self, text="Enter", height = 2, width = 30, command= lambda:textField_remove(entry,text))
        button5.pack(pady = 10)

        button1 = tk.Button(self, text="Back to Main Menu", height = 2, command= lambda: controller.show_frame(StartPage))
        button1.pack(fill = tk.X,pady = 2)
        button3 = tk.Button(self, text="Show Device List", height = 2,command= lambda: controller.show_frame(PageOne))
        button3.pack(fill = tk.X, pady = 2)
        button4 = tk.Button(self, text="Add Devices", height = 2, command= lambda: controller.show_frame(PageThree))
        button4.pack(fill = tk.X, pady = 2)
        button2 = tk.Button(self, text="Scan Network", height = 2, command= lambda: controller.show_frame(PageFour))
        button2.pack(fill = tk.X, pady = 2)
        
        button6 = tk.Button(self, text="Select Device", height = 2, command= lambda: controller.show_frame(PageFive))
        button6.pack(fill = tk.X, pady = 2)

        def textField_remove(e, text):
            string = e.get()
            remote = xnet.get_device_by_node_id(string)
            if remote == None:
                text.set("Error! That node does not exist in the network")
            else:
                base.send_data(remote,b'0')
                xnet.remove_device(remote)
                text.set("Success! Node %s was removed from the network" % string)
                if deviceNames[0] == string:
                    deviceList.pop(0)
                    deviceNames.pop(0)
                else:
                    for i in range(1, len(deviceList)):
                        if deviceNames[i] == string:
                            deviceList.pop(i)
                            deviceNames.pop(i)
                print(deviceNames)



class PageThree(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        text = tk.StringVar()
        label = tk.Label(self, text="Add Device Menu")
        label.pack()

        label1 = ttk.Label(self, text="Enter the device identifier to add below")
        label1.pack(pady = 10)
        entry = ttk.Entry(self, width = 30)
        entry.pack(pady = 10)
        label2 = ttk.Label(self, textvariable=text)
        label2.pack(pady = 5)
        button5 = tk.Button(self, text="Enter", height = 2, width = 30, command= lambda:textField_add(entry,text))
        button5.pack(pady = 10)

        button1 = tk.Button(self, text="Back to Main Menu", height = 2, command= lambda: controller.show_frame(StartPage))
        button1.pack(fill = tk.X, pady = 2)
        button3 = tk.Button(self, text="Show Device List", height = 2, command= lambda: controller.show_frame(PageOne))
        button3.pack(fill = tk.X, pady = 2)
        button4 = tk.Button(self, text="Remove Devices", height = 2, command= lambda: controller.show_frame(PageTwo))
        button4.pack(fill = tk.X, pady = 2)
        button2 = tk.Button(self, text="Scan Network", height = 2, command= lambda: controller.show_frame(PageFour))
        button2.pack(fill = tk.X, pady = 2)

        button6 = tk.Button(self, text="Select Device", height = 2, command= lambda: controller.show_frame(PageFive))
        button6.pack(fill = tk.X, pady = 2
                )
        def textField_add(e,text):
            string = e.get()
            remote = xnet.discover_device(string)
            if remote == None:
                text.set("Error! This node does not exist!")
            else:
                base.send_data(remote,b'1')
                text.set("Success! Node was added to network")
                xbeeDevice(string)


        

class PageFour(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = tk.Label(self, text="Scan Network")
        label.pack()
        
        button1 = tk.Button(self, text="Start Scan", height = 1, width = 47, command= lambda: scan_network(xnet))
        button1.pack(pady = 4)
        button3 = tk.Button(self, text="Refresh text", height = 1, width = 47, command= lambda: update(textWindow))
        button3.pack(pady = 4)
        global textWindow
        textWindow = tk.Text(self, height=5, width=50)
        textWindow.pack(anchor = 'center', pady = 4)

        button4 = tk.Button(self, text="Back to Main Menu", height = 2, command= lambda:controller.show_frame(StartPage))
        button4.pack(fill = tk.X, pady = 2)
        button5 = tk.Button(self, text="List Network", height = 2, command= lambda: controller.show_frame(PageOne))
        button5.pack(fill = tk.X, pady = 2)
        button6 = tk.Button(self, text="Remove Devices", height = 2, command= lambda: controller.show_frame(PageTwo))
        button6.pack(fill = tk.X, pady = 2)
        button7 = tk.Button(self, text="Add Devices", height = 2, command= lambda: controller.show_frame(PageThree))
        button7.pack(fill = tk.X, pady = 2)
        button8 = tk.Button(self, text="Select Device", height = 2, command= lambda: controller.show_frame(PageFive))
        button8.pack(fill = tk.X, pady = 2)

class PageFive(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = tk.Label(self, text="Device Select")
        label.pack()
        curDevice = tk.StringVar()
        curDevice.set("empty")

        deviceMenu = tk.OptionMenu(self, curDevice, 'empty')
        deviceMenu.config(width = 10)
        deviceMenu.pack(pady = 10)
        def callback_update(self):
            if len(deviceList) == 0: 
                deviceMenu.children["menu"].delete(0,"end")
                deviceMenu.children["menu"].add_command(label="empty", command = None)
                curDevice.set("empty")
            else:
                deviceNames = []
                deviceMenu.children["menu"].delete(0,"end")
                for node in deviceList:
                    deviceNames.append(node.name)
                    deviceMenu.children["menu"].add_command(label=node.name, command = lambda device=node.name: curDevice.set(device))
                curDevice.set(deviceNames[0])
        
        
        deviceMenu.bind("<Enter>", callback_update)

        option = tk.IntVar()
        lightOn = tk.Radiobutton(self, text = "lights on", variable = option, value = 1, command = lambda: interpretOption())
        lightOff = tk.Radiobutton(self, text = "lights off", variable = option, value = 2, command = lambda: interpretOption())
        motion = tk.Radiobutton(self, text = "motion sense", variable = option, value = 3, command = lambda: interpretOption())

        lightOn.pack(anchor="center", pady = 4)
        lightOff.pack(anchor="center", pady = 4)
        motion.pack(anchor="center", pady = 4)
        
        label1 = ttk.Label(self, text = "Brightness Level")
        label1.pack(pady = 10)

        sBox = tk.Spinbox(self, from_ = 0, to = 15, width = 6, font = Font(family = 'Helvetica', size = 36),state = 'disabled')
        sBox.pack(pady = 10)

        button3 = tk.Button(self, text="Apply Changes", height = 1, width = 30, command = lambda: applyOptions())
        button3.pack(pady = 5 )
        textField = tk.StringVar()
        label2 = ttk.Label(self, textvariable=textField)
        label2.pack()
        button4 = tk.Button(self, text="Back to Main Menu", height = 2, command= lambda:controller.show_frame(StartPage))
        button4.pack(fill = tk.X, pady = 5)

        def interpretOption():
            if len(deviceList) != 0:
                sBox['state'] = 'normal'
            else:
                sBox['state'] = 'disabled'

        def applyOptions():
            if len(deviceList) == 0:
                textField.set("Error there are no devices in the list")

            choice = option.get()
            val = int(sBox.get())
            if val > 15:
                textField.set("Error! Please select a value from 0 to 15 for brightness")
                pass
            elif choice == 1:
                node = curDevice.get()
                textField.set("Setting {}'s lights to on".format(node))    
                remote = xnet.get_device_by_node_id(node)
                base.send_data(remote, b'2')
                time.sleep(.5)
                base.send_data(remote, bytes([val]))
            elif choice == 2:
                node = curDevice.get()
                textField.set("Setting {}'s lights to off".format(node))
                remote = xnet.get_device_by_node_id(node)
                base.send_data(remote, b'3')
            elif choice == 3:
                node = curDevice.get()
                textField.set("Setting {}'s lights to trigger on motion detected".format(node))
                remote = xnet.get_device_by_node_id(node)
                base.send_data(remote, b'4')
                time.sleep(.5)
                base.send_data(remote, bytes([val]))


def server_send():
    if len(deviceList) != 0:
        if xnet.has_devices():
            currentDT = datetime.datetime.now()
            xbee_message = base.read_data()
            if xbee_message != None:
                remote = xbee_message.remote_device
                node = xnet.get_device_by_64(remote.get_64bit_addr())
                data = xbee_message.data
                url = "http://RaspZeroWN.local:5000/update/{}".format(("%s"%node) + str(currentDT))
                try:
                    response = requests.get(url)
                    response.raise_for_status()
                except HTTPError as http_err:
                    print(f'HTTP error occured: {http_err}')
                except Exception as err:
                    print(f'Other error occured: {err}')
    root.after(1000,server_send)

try:
    root = mainMenu()
    root.geometry("680x460")
    root.after(1000, server_send)
    root.mainloop()
except KeyboardInterrupt:
    base.send_data_broadcast(b'0')
    base.close()
    xnet.clear()
    root.quit()
    exit()
