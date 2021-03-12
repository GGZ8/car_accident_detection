import serial
import serial.tools.list_ports
import struct


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
                self.ser = serial.Serial(self.port_name, 115200, timeout=0)
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
                    self.use_data()
                    self.in_buffer = []
                else:
                    self.in_buffer.append(value)

    def use_data(self):
        if self.in_buffer[0] != b'\x7F':
            return False

        num_val = int.from_bytes(self.in_buffer[1], byteorder='little')
        data = self.in_buffer[2:]
        if len(data) != num_val:
            return False

        ac_x = round(struct.unpack('f', b''.join(data[:4]))[0], 2)
        ac_z = round(struct.unpack('f', b''.join(data[4:8]))[0], 2)
        ac_y = round(struct.unpack('f', b''.join(data[8:12]))[0], 2)
        angle_x = int.from_bytes(data[-2], byteorder='little', signed=True)
        angle_y = int.from_bytes(data[-1], byteorder='little', signed=True)

        print('AcX: ', ac_z, '\t\tAcY: ', ac_y, '\t\tAcX: ', ac_x, '\t\tAngleX: ', angle_x, '\t\tAngleY: ', angle_y)


if __name__ == "__main__":
    my_serial = Bridge()
    my_serial.setup_serial()
    my_serial.loop()
