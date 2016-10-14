import serial
import paho.mqtt.publish as publish
import time

class Communicator(object):

    def __init__(self, host):
        self.serial = serial.Serial(port = "/dev/ttyACM0", baudrate=9600)
        self.serial.close()
        self.serial.open()
        self.host = host

    def loop(self):
        while True:
            line = self.serial.readline()
            publish.single("lasersaurtemp", line, hostname=self.host)
            time.sleep(10)

    def stop(self):
        self.serial.close()

if __name__ == "__main__":
    instance = Communicator("hostname")
    instance.loop()
    instance.stop()
