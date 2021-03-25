import requests
from haversine import haversine, Unit
from datetime import datetime, timedelta

threshold_light = 234
threshold_flame = 60
threshold_temp = 30


def detect_fire(flame, light, temp):
    # No luce, ma detection di fuoco
    if light < threshold_light and flame > threshold_flame:
        return True

    # Luce e controllo se c'è fuoco
    if light >= threshold_light and flame > threshold_flame:
        if temp > threshold_temp:
            return True

    return False


def detect_near_accidents(car_pos, speed):
    if speed > 100:
        threshold = 20
    else:
        threshold = 10
    accident = False
    now = datetime.now()
    response = requests.get('http://192.168.1.18/api/accidents')
    if response.status_code == 200:
        accidents = response.json()['accidents']
        for a in accidents:
            crash_time = datetime.strptime(a['date_time'], '%d-%m-%YT%H:%M:%S')
            if crash_time + timedelta(minutes=50) > now:
                accident_pos = (a['lat'], a['lng'])
                if haversine(car_pos, accident_pos, unit=Unit.KILOMETERS) < threshold:
                    accident = True

    return accident
