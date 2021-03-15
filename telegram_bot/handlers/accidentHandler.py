import logging

from telegram import Location, Bot, ParseMode
from secret import telegram_key

logger = logging.getLogger()


def accident_message(accident):
    data = accident.date_time.strftime("%d/%m/%Y")
    hour = accident.date_time.strftime("%H:%M:%S")
    msg = f"Incidente rilevato il {data} alle {hour} UTC per l'auto con targa *{accident.car.license_plate}*\.\nPosizione dell'incidente:"
    bot = Bot(token=telegram_key)
    bot.send_message(chat_id=accident.car.chat_id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)
    bot.sendLocation(chat_id=accident.car.chat_id, location=Location(latitude=accident.lat, longitude=accident.lng))
