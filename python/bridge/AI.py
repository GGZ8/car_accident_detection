from haversine import haversine, Unit
from datetime import datetime, timedelta


def detect_fire(flame, light, temp):
    light_threshold = 234
    flame_threshold = 60
    temp_threshold = 65
    # No luce, ma detection di fuoco
    if light < light_threshold and flame > flame_threshold:
        return True

    # Luce e controllo se c'è fuoco
    if light >= light_threshold and flame > flame_threshold:
        if temp > temp_threshold:
            return True

    return False


def detect_near_accidents(car_pos, speed, response):
    if speed >= 100:
        speed_threshold = 20
    else:
        speed_threshold = 5

    accident = False
    now = datetime.now()
    accidents = response.json()['accidents']
    for a in accidents:
        crash_time = datetime.strptime(a['date'], '%d-%m-%YT%H:%M:%S')
        if crash_time + timedelta(minutes=30) > now:
            accident_pos = (a['lat'], a['lng'])
            if haversine(car_pos, accident_pos, unit=Unit.KILOMETERS) < speed_threshold:
                accident = True

    return accident
