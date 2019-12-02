import socket
import time


global clientsocket
global address

def interpretCommand(cmd):
    if cmd == bytes("exit","utf-8"):
        print("Thank you for using ABT Home System")
        quit()
    elif cmd == bytes("remove", "utf-8"):
        print("Remove Selected")
    elif cmd == bytes("add", "utf-8"):
        print("Add selected")
    elif cmd == bytes("list", "utf-8"):
        print("List selected")
    elif cmd == bytes("scan", "utf-8"):
        print("Scan selected")
    else:
        print("Invaild cmd")


class xbeeCommunicator():
    def __init__(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind((socket.gethostname(), 1234))
        self.s.listen(5)



    def handshake(self):
        print ("Awaiting connection from client")
        while True:
            clientsocket, address = self.s.accept()
            print(f"Connection from {address} has been established!")
            break

        time.sleep(10)
        clientsocket.send(bytes("connected", "utf-8"))
        self.s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s1.connect((socket.gethostname(), 1235))
        time.sleep(5)
        clientsocket.send(bytes("connected", "utf-8"))
        while True:
            if self.s1.recv(8) == bytes("yes", "utf-8"):
                print("Confirmation from client has been receieved")
                break

        print("Handshake was successfully executed")


    def receiveData(self):
        msg = self.s1.recv(1024)
        if msg != b'': 
            print(msg)
            interpretCommand(msg)



    def closeCommunicator(self):
        self.s.close()
        self.s1.close()

if __name__ == '__main__':
    try:
        host = xbeeCommunicator()
        host.handshake()
        while True:
            host.receiveData()

    except KeyboardInterrupt:
        host.closeCommunicator()



