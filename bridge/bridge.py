import serial
import serial.tools.list_ports
import struct
from datetime import datetime
from models import get_session, Accident
from telegram_bot.handlers.accidentHandler import accident_message
from dateutil import tz
from aiFire import detect_fire


class Bridge:

    def __init__(self):
        self.in_buffer = []
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
                else:
                    self.in_buffer.append(value)

    def check_integrity(self, header):
        if len(self.in_buffer) < 2:
            print(self.in_buffer)
            return False

        num_val = int.from_bytes(self.in_buffer[1], byteorder='little')
        data_count = len(self.in_buffer[2:])
        if self.in_buffer[0] != header or data_count != num_val:
            print(f'ERROR: package drop, invalid lenght -> {data_count} != {num_val}')
            return False

        return True

    def check_fire(self):
        if not self.check_integrity(b'\xFE'):
            return False
        data = self.in_buffer[2:]

        flame = int.from_bytes(data[0], byteorder='little')
        light = int.from_bytes(data[1], byteorder='little')
        temp = int.from_bytes(data[2], byteorder='little')
        sign = bool.from_bytes(data[3], byteorder='little')
        if sign:
            temp = -temp

        if detect_fire(flame, light, temp):
            self.ser.write(b'FIRE')

    def accident_report(self):
        if not self.check_integrity(b'\x7F'):
            return False

        self.ser.write(b'ACK')
        data = self.in_buffer[2:]

        lat = round(struct.unpack('f', b''.join(data[:4]))[0], 6)
        lng = round(struct.unpack('f', b''.join(data[4:8]))[0], 6)
        hour = struct.unpack('I', b''.join(data[8:12]))[0]
        date = struct.unpack('I', b''.join(data[12:16]))[0]
        frontal = bool.from_bytes(data[16], byteorder='little')
        tilt = bool.from_bytes(data[17], byteorder='little')
        fire = bool.from_bytes(data[18], byteorder='little')
        fall = bool.from_bytes(data[19], byteorder='little')
        tmp = round(struct.unpack('f', b''.join(data[20:24]))[0], 2)
        license_plate = (struct.unpack('7s', b''.join(data[24:]))[0]).decode('utf-8')
        date_time = datetime.combine(datetime.strptime(str(date), '%d%m%y'),
                                     datetime.strptime(str(hour), '%H%M%S%f').time())
        date_time = date_time.replace(tzinfo=self.from_zone)
        date_time = date_time.astimezone(self.to_zone)
        print(
            f'lat={lat}, lng={lng}, frontal={frontal}, tilt={tilt}, fire={fire}, fall={fall}, tmp={tmp}, targa={license_plate}, data={date_time}')

        with get_session() as session:
            accident = Accident(car_id=license_plate, date_time=date_time, temperature=tmp, fire=fire, frontal=frontal,
                                tilt=tilt, fall=fall, lat=lat, lng=lng, reported=False)
            session.add(accident)
            session.commit()
            if accident.car.chat_id is not None:
                accident_message(accident)


if __name__ == "__main__":
    my_serial = Bridge()
    my_serial.setup_serial()
    my_serial.loop()
