import tkinter as tk
from tkinter import ttk
import time
import datetime
import serial
import requests
from requests.exceptions import HTTPError
from digi.xbee.devices import *

textWindow = None
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

def stop_scan():
    xnet.stop_discovery_process()

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
        
        for F in (StartPage, PageOne, PageTwo,PageThree,PageFour):
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

        button1 = ttk.Button(self, text="List Network", command= lambda: controller.show_frame(PageOne))
        button1.pack(fill=tk.X, pady=4)
        button2 = ttk.Button(self, text="Remove Devices", command= lambda: controller.show_frame(PageTwo))
        button2.pack(fill=tk.X, pady=4)
        button3 = ttk.Button(self, text="Add Devices", command= lambda: controller.show_frame(PageThree))
        button3.pack(fill=tk.X, pady=4)
        button4 = ttk.Button(self, text="Scan for Devices", command= lambda: controller.show_frame(PageFour))
        button4.pack(fill=tk.X, pady=4)
        button5 = ttk.Button(self, text="Exit", command= lambda: controller.quit())
        button5.pack(fill=tk.X, pady=4)

class PageOne(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = ttk.Label(self, text="Display Network Menu")
        label.pack()

        button1 = ttk.Button(self, text="Back to Main Menu", command= lambda: controller.show_frame(StartPage))
        button1.pack()
        button3 = ttk.Button(self, text="Remove Devices", command= lambda: controller.show_frame(PageTwo))
        button3.pack()
        button4 = ttk.Button(self, text="Add Devices", command= lambda: controller.show_frame(PageThree))
        button4.pack()
        button5 = ttk.Button(self, text="Scan Network", command= lambda: controller.show_frame(PageFour))
        button5.pack()
        button6 = ttk.Button(self, text="Refresh text", command= lambda: update(textWindow))
        button6.pack()
        textWindow = tk.Text(self, height=5, width=30)
        textWindow.pack(fill=tk.X, pady=50)

class PageTwo(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        text = tk.StringVar()
        label = tk.Label(self, text="Remove Device Menu")
        label.pack()

        button1 = ttk.Button(self, text="Back to Main Menu", command= lambda: controller.show_frame(StartPage))
        button1.pack()
        button3 = ttk.Button(self, text="Show Device List", command= lambda: controller.show_frame(PageOne))
        button3.pack()
        button4 = ttk.Button(self, text="Add Devices", command= lambda: controller.show_frame(PageThree))
        button4.pack()
        button2 = ttk.Button(self, text="Scan Network", command= lambda: controller.show_frame(PageFour))
        button2.pack()


        label1 = ttk.Label(self, text="Enter the device identifier to remove below")
        label1.pack()
        entry = ttk.Entry(self)
        entry.pack()
        label2 = ttk.Label(self, textvariable=text)
        label2.pack()
        button5 = ttk.Button(self, text="Enter", command= lambda:textField_remove(entry,text))
        button5.pack()

def textField_remove(e, text):
    string = e.get()
    remote = xnet.get_device_by_node_id(string)
    if remote == None:
        text.set("Error! That node does not exist in the network")
    else:
        base.send_data(remote,b'0')
        xnet.remove_device(remote)
        text.set("Success! Node %s was removed from the network" % string)


class PageThree(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        text = tk.StringVar()
        label = tk.Label(self, text="Add Device Menu")
        label.pack()

        button1 = ttk.Button(self, text="Back to Main Menu", command= lambda: controller.show_frame(StartPage))
        button1.pack()
        button3 = ttk.Button(self, text="Show Device List", command= lambda: controller.show_frame(PageOne))
        button3.pack()
        button4 = ttk.Button(self, text="Remove Devices", command= lambda: controller.show_frame(PageTwo))
        button4.pack()
        button2 = ttk.Button(self, text="Scan Network", command= lambda: controller.show_frame(PageFour))
        button2.pack()
        label1 = ttk.Label(self, text="Enter the device identifier to add below")
        label1.pack()
        entry = ttk.Entry(self)
        entry.pack()
        label2 = ttk.Label(self, textvariable=text)
        label2.pack()
        button5 = ttk.Button(self, text="Enter", command= lambda:textField_add(entry,text))
        button5.pack()
def textField_add(e,text):
    string = e.get()
    remote = xnet.discover_device(string)
    if remote == None:
        text.set("Error! This node does not exist!")
    else:
        base.send_data(remote,b'1')
        text.set("Success! Node was added to network")


        

class PageFour(tk.Frame):
    def __init__(self, parent, controller):
        tk.Frame.__init__(self,parent)
        label = tk.Label(self, text="Scan Network")
        label.pack()
        
        button5 = ttk.Button(self, text="List Network", command= lambda: controller.show_frame(PageOne))
        button5.pack()
        button6 = ttk.Button(self, text="Remove Devices", command= lambda: controller.show_frame(PageTwo))
        button6.pack()
        button7 = ttk.Button(self, text="Add Devices", command= lambda: controller.show_frame(PageThree))
        button7.pack()

        button1 = ttk.Button(self, text="Start Scan", command= lambda: scan_network(xnet))
        button1.pack()
        button2 = ttk.Button(self, text="Stop Scan", command= lambda: stop_scan(xnet))
        button2.pack()
        button3 = ttk.Button(self, text="Refresh text", command= lambda: update(textWindow))
        button3.pack()
        button4 = ttk.Button(self, text="Back to Main Menu", command= lambda:controller.show_frame(StartPage))
        button4.pack()
        global textWindow
        textWindow = tk.Text(self, height=5, width=30)
        scroll = ttk.Scrollbar(self)
        scroll.pack(side=tk.RIGHT, fill=tk.Y)
        textWindow.pack(side=tk.LEFT, fill=tk.Y)
        scroll.config(command=textWindow.yview)
        textWindow.config(yscrollcommand=scroll.set)

def update(textWin):
    textWin.delete('1.0', tk.END)
    textWin.insert(tk.END, network_string(xnet))


def server_send():
    if xnet.has_devices():
        currentDT = datetime.datetime.now()
        xbee_message = base.read_data()
        remote = xbee_message.remote_device
        node = xnet.get_device_by_64(remote.get_64bit_addr())
        data = xbee_message.data
       # url = "http://raspberrypi.local:5000/update/{}".format(("%s"%node) + str(currentDT))
       # try:
       #     response = requests.get(url)
       #     response.raise_for_status()
       # except HTTPError as http_err:
       #     print(f'HTTP error occured: {http_err}')
       # except Exception as err:
       #     print(f'Other error occured: {err}')
   #l root.after(5000,server_send)

try:
    root = mainMenu()
    root.geometry("420x640")
    #root.after(1000, server_send)
    root.mainloop()
except KeyboardInterrupt:
    print('')
    base.close()
    root.quit()
    exit()
