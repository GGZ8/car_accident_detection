import logging

from telegram import Update, ParseMode
from telegram.ext import CallbackContext
from models import get_session, Car

logger = logging.getLogger()


def get_car(update: Update, context: CallbackContext):
    user_id = update.message.from_user.id
    logging.info(user_id)
    with get_session() as session:
        try:
            cars = session.query(Car).filter_by(user_id=user_id).all()
            if not len(cars):
                msg = "Non hai memorizzato nessuna auto"
            else:
                msg = "Ecco la lista delle tue auto 🚗: \n"
                for idx, c in enumerate(cars):
                    msg += f"{idx+1}\.    *{c.license_plate}*\n"
        except Exception as e:
            logging.info(f"ERROR: {e}")
            msg = f"Errore\! Riprova più tardi"

    context.bot.send_message(chat_id=update.effective_chat.id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)

