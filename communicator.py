import serial
import paho.mqtt.publish as publish
import time

class Communicator(object):

    def __init__(self, host):
        self.serial = serial.Serial(port = "/dev/ttyACM0", baudrate=9600)
        self.serial.close()
        self.serial.open()
        self.serial.flushInput()
        self.host = host

    def loop(self):
        while True:
            line = self.serial.readline()
            line = str(time.clock()) + ',' + line
            publish.single("lasersaurtemp", line, hostname=self.host)
            time.sleep(2)

    def stop(self):
        self.serial.close()

if __name__ == "__main__":
    instance = Communicator("space.makerspaceleiden.nl")
    instance.loop()
    instance.stop()
