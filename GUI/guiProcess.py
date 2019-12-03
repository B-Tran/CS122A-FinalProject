import socket
import time

global clientsocket
global address

class guiClient():
    def __init__(self):
        print("Beginning connection process")
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((socket.gethostname(), 1234))


    def handshake(self):
        global clientsocket
        global address
        self.s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s1.bind((socket.gethostname(), 1235))
        self.s1.listen(5)
        while True:
            clientsocket, address = self.s1.accept()
            print(f"Connection from xbeeCommunicator has been established!")
            break
        msg = self.s.recv(1024)
        print(msg)
        while True:
            if self.s.recv(1024) == bytes("connected", "utf-8"):
                break;
        clientsocket.send(bytes("yes", "utf-8"))

    
    def sendData(self,byteMsg):
        clientsocket.send(bytes(byteMsg, "utf-8"))
    
    def closeClient(self):
        self.s.close()
        self.s1.close()




if __name__ == '__main__':
    try:
        client = guiClient()
        client.handshake()
        while True:
            client.sendData("")

    except KeyboardInterrupt:
        client.closeClient()


