import time
import serial
from digi.xbee.devices import *
#note: pi to atmega tx to rx, rx to tx
# pi to xbee tx to tx, rx to rx

#ser = serial.Serial(port='/dev/serial0', baudrate = 9600)

device = XBeeDevice("/dev/serial0", 9600)

device.open()

xnet = device.get_network()
xnet.set_discovery_timeout(10) #10 secs
xnet.set_discovery_options({DiscoveryOptions.APPEND_DD, DiscoveryOptions.DISCOVER_MYSELF})

def callback_device_discovered(remote):
    print("Device discovered: %s" % remote)

def callback_discovery_finished(status):
    if status == NetworkDiscoveryStatus.SUCCESS:
        print("Discovery process finished successfully.")
    else:
        print("There was an error discovering devices %s" % status.description)

xnet.add_device_discovered_callback(callback_device_discovered)
xnet.add_discovery_process_finished_callback(callback_discovery_finished)

xnet.start_discovery_process()
print("Beginning the network search")
while xnet.is_discovery_running():
    time.sleep(0.1)

remote = xnet.get_device_by_node_id("Bedroom")
xnet.remove_device(remote)

if __name__ == '__main__':
    try:
        print("We did it now he is a loop of the available devices")
        while True:
            if xnet.has_devices():
                print("%s" % '\n   '.join(map(str, xnet.get_devices())))
            else:
                print("None")
            time.sleep(1)
    except KeyboardInterrupt:
        print('')
        device.close()
        exit()
