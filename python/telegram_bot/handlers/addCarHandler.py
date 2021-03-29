import logging
import re

from telegram import ReplyKeyboardMarkup, Update, ParseMode
from telegram.ext import (
    CommandHandler,
    MessageHandler,
    Filters,
    ConversationHandler,
    CallbackContext,
)

from .helpHandler import reply_keyboard as default_reply_keyboard
from common.models import get_session, Car

TOTAL = 1
pattern = '\D\D\d\d\d\D\D'
logger = logging.getLogger()


def add_car(update: Update, context: CallbackContext) -> int:
    logging.info(f"Inserimento auto - utente {update.message.from_user.id}")
    context.bot.send_message(chat_id=update.effective_chat.id, text="Inserisci la targa dell'auto \(o /cancel per "
                                                                  "terminare\):", parse_mode=ParseMode.MARKDOWN_V2)
    return TOTAL


def read_license_plate(update: Update, context: CallbackContext) -> int:
    # recupero il valore
    text = update.message.text
    text = text.upper()
    # controlli
    if len(text) > 7 or not re.match(pattern, text):
        logging.info(f"Targa {text} errata")
        msg = "Formato targa errato \(Es\. AA111AA\)\.\nReinserisci \(o /cancel per terminare\):"
        context.bot.send_message(chat_id=update.effective_chat.id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)
        return TOTAL
    else:
        chat_id = update.effective_chat.id
        with get_session() as session:
            try:
                car = session.query(Car).get(text)
                if car is None:
                    car = Car(license_plate=text, chat_id=chat_id)
                    session.add(car)
                    session.commit()
                    msg = f"🚗 Auto con targa *{text}* aggiunta correttamente"
                    val = ConversationHandler.END
                else:
                    if car.chat_id is not None:
                        msg = f"*ERRORE* 🚗 *{text}* già registrata \!"
                        val = TOTAL
                    else:
                        car.chat_id = chat_id
                        session.commit()
                        msg = f"🚗 Auto con targa *{text}* aggiunta correttamente"
                        val = ConversationHandler.END
            except Exception as e:
                logging.info(f"ERROR: {e}")
                msg = f"Errore durante inserimento"
                val = TOTAL

        context.bot.send_message(chat_id=chat_id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)
        return val


def cancel(update: Update, context: CallbackContext) -> int:
    logging.info('Inserimento cancellato')
    update.message.reply_text(
        'Inserimento cancellato', reply_markup=ReplyKeyboardMarkup(default_reply_keyboard, one_time_keyboard=True,
                                                                   resize_keyboard=True)
    )
    return ConversationHandler.END


def add_car_handler():
    conv_handler = ConversationHandler(
        entry_points=[MessageHandler(Filters.text & (~Filters.command) & Filters.regex('^➕ Inserisci$'), add_car),
                      CommandHandler('inserisci', add_car)],
        states={
            TOTAL: [
                MessageHandler(Filters.text & ~Filters.command, read_license_plate)
            ],
        },
        fallbacks=[CommandHandler('cancel', cancel)],
    )
    return conv_handler
