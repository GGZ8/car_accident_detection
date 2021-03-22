import time

from common.models import get_session, Accident
from telegram_bot.handlers.accidentHandler import accident_message
from datetime import datetime

time.sleep(5)

with get_session() as session:
    accident = Accident(car_id="AA111AA", date_time=datetime.now(), temperature=40, fire=False, frontal=True,
                        tilt=False, fall=False, lat=44.540967, lng=10.931247, reported=False)
    session.add(accident)
    session.commit()
    accident_message(accident)
