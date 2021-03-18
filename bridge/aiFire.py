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
