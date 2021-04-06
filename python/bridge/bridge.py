from time import sleep

import requests
import serial
import serial.tools.list_ports
import struct
from datetime import datetime
from dateutil import tz
from AI import detect_fire, detect_near_accidents
from common.settings import Setting


class Bridge:

    def __init__(self):
        self.api_ip = '192.168.1.18:8000'
        self.api_version = Setting.API_VERSION
        self.in_buffer = []
        self.data = []
        self.ser = None
        self.port_name = None
        self.from_zone = tz.gettz('UTC')
        self.to_zone = tz.tzlocal()

    def setup_serial(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            if 'arduino' in port.description.lower() or 'iousbhostdevice' in port.description.lower():
                self.port_name = port.device
        print('Connecting to ' + str(self.port_name))
        if self.port_name:
            try:
                self.ser = serial.Serial(self.port_name, 9600, timeout=0)
            except serial.SerialException as e:
                print('Error no connection, error: ' + str(e))
                exit()
        else:
            print('Connect arduino')
            exit()

    def loop(self):
        while True:
            if self.ser.in_waiting > 0:
                value = self.ser.read(1)
                if value == b'\x7E':
                    self.accident_report()
                    self.in_buffer = []
                elif value == b'\xFF':
                    self.check_fire()
                    self.in_buffer = []
                elif value == b'\xFC':
                    self.check_near_accidents()
                    self.in_buffer = []
                else:
                    self.in_buffer.append(value)

    def check_integrity(self, header):
        print(self.in_buffer)
        if len(self.in_buffer) < 2:
            print(f'ERROR: package drop, too short')
            return False

        num_val = int.from_bytes(self.in_buffer[1], byteorder='little')
        data_count = len(self.in_buffer[2:])
        if self.in_buffer[0] != header or data_count != num_val:
            print(f'ERROR: package drop, invalid lenght -> {data_count} != {num_val}')
            return False

        self.data = self.in_buffer[2:]
        return True

    def check_fire(self):
        print("FIRE")
        if not self.check_integrity(header=b'\xFE'):
            return False

        flame = int.from_bytes(self.data[0], byteorder='little')
        light = int.from_bytes(self.data[1], byteorder='little')
        temp = int.from_bytes(self.data[2], byteorder='little')
        sign = bool.from_bytes(self.data[3], byteorder='little')
        if sign:
            temp = -temp

        if detect_fire(flame, light, temp):
            self.ser.write(b'FIRE')

    def accident_report(self):
        print("ACCIDENT REPORT")
        if not self.check_integrity(header=b'\x7F'):
            return False

        lat = round(struct.unpack('f', b''.join(self.data[:4]))[0], 6)
        lng = round(struct.unpack('f', b''.join(self.data[4:8]))[0], 6)
        hour = struct.unpack('I', b''.join(self.data[8:12]))[0]
        date = struct.unpack('I', b''.join(self.data[12:16]))[0]
        frontal = bool.from_bytes(self.data[16], byteorder='little')
        tilt = bool.from_bytes(self.data[17], byteorder='little')
        fire = bool.from_bytes(self.data[18], byteorder='little')
        fall = bool.from_bytes(self.data[19], byteorder='little')
        temp = round(struct.unpack('f', b''.join(self.data[20:24]))[0], 2)
        license_plate = (struct.unpack('7s', b''.join(self.data[24:]))[0]).decode('utf-8')
        date_time = datetime.combine(datetime.strptime(str(date), '%d%m%y'),
                                     datetime.strptime(str(hour), '%H%M%S%f').time())
        date_time = date_time.replace(tzinfo=self.from_zone)
        date_time = date_time.astimezone(self.to_zone)
        key = ('lat', 'lng', 'frontal', 'tilt', 'fire', 'fall', 'temp', 'license_plate', 'date')
        value = (lat, lng, frontal, tilt, fire, fall, temp, license_plate, date_time.__str__())
        json_data = dict(zip(key, value))
        print(json_data)
        self.ser.write(b'ACK')
        while requests.post(f'http://{self.api_ip}/{self.api_version}/accidents', json=json_data).status_code != 200:
            sleep(1)

    def check_near_accidents(self):
        print("SEARCH NEAR ACCIDENT")
        if not self.check_integrity(header=b'\xFD'):
            return False

        lat = round(struct.unpack('f', b''.join(self.data[:4]))[0], 6)
        lng = round(struct.unpack('f', b''.join(self.data[4:8]))[0], 6)
        speed = int.from_bytes(self.data[8], byteorder='little')
        car_pos = (lat, lng)
        response = requests.get(f'http://{self.api_ip}/{self.api_version}/accidents')
        if response.status_code == 200 and detect_near_accidents(car_pos, speed, response):
            print("Near Accident detected")
            self.ser.write(b'ON')


if __name__ == "__main__":
    my_serial = Bridge()
    my_serial.setup_serial()
    my_serial.loop()
