import serial
import serial.tools.list_ports
import struct
from datetime import datetime
from models import get_session, Accident
from telegram_bot.handlers.accidentHandler import accident_message


class Bridge:

    def __init__(self):
        self.in_buffer = []
        self.ser = None
        self.port_name = None

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
                    self.check_integrity()
                    self.in_buffer = []
                else:
                    self.in_buffer.append(value)

    def check_integrity(self):
        #print(self.in_buffer)
        num_val = int.from_bytes(self.in_buffer[1], byteorder='little')
        data_count = len(self.in_buffer[2:])
        if self.in_buffer[0] != b'\x7F' or data_count != num_val:
            print(data_count, num_val)
            print('ERROR: package drop, invalid lenght')
            return False

        if num_val == 31:
            self.accident_report()
        elif num_val == 12:
            self.check_for_free_fall()
        else:
            print('ERROR: no function for this package')

    def check_for_free_fall(self):
        data = self.in_buffer[2:]
        ac_x = round(struct.unpack('f', b''.join(data[:4]))[0], 2)
        ac_z = round(struct.unpack('f', b''.join(data[4:8]))[0], 2)
        ac_y = round(struct.unpack('f', b''.join(data[8:12]))[0], 2)

        print('AcX: ', ac_z, '\t\tAcY: ', ac_y, '\t\tAcX: ', ac_x)

    def accident_report(self):
        self.ser.write(b'REC')
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
        print(lat, lng, hour, date, frontal, tilt, fire, fall, tmp, license_plate, date_time)

        '''
        with get_session() as session:
            accident = Accident(car_id=license_plate, date_time=date_time, temperature=tmp, fire=fire, frontal=frontal,
                                tilt=tilt, fall=fall, lat=lat, lng=lng, reported=False)
            session.add(accident)
            session.commit()
            accident_message(accident)
        '''


if __name__ == "__main__":
    my_serial = Bridge()
    my_serial.setup_serial()
    my_serial.loop()
