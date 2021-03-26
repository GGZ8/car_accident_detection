import logging

from telegram import Location, Bot, ParseMode
from common.secret import telegram_key

logger = logging.getLogger()


def accident_message(accident):
    data = accident['date'].strftime("%d/%m/%Y")
    hour = accident['date'].strftime("%H:%M:%S")
    msg = f"Incidente rilevato il {data} alle {hour} per l'auto con targa *{accident['car_id']}*\.\nPosizione dell'incidente:"
    bot = Bot(token=telegram_key)
    bot.send_message(chat_id=accident['chat_id'], text=msg, parse_mode=ParseMode.MARKDOWN_V2)
    bot.sendLocation(chat_id=accident['chat_id'], location=Location(latitude=accident['lat'], longitude=accident['lng']))
