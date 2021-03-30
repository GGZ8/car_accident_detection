import requests
from datetime import datetime
from dateutil import tz

key = ('lat', 'lng', 'frontal', 'tilt', 'fire', 'fall', 'temp', 'license_plate', 'date')
date = datetime.now().replace(microsecond=0).astimezone(tz.tzlocal()).strftime('%Y-%m-%d %H:%M:%S%z')
value = (44.610255, 10.935767, True, False, False, False, 40, "AA111BA", date)
json_data = dict(zip(key, value))
print(json_data)
response = requests.post('http://192.168.1.18:8000/api/v1/accidents', json=json_data)
