import logging
import re

from telegram import Update, ReplyKeyboardMarkup, ParseMode, InlineKeyboardButton, InlineKeyboardMarkup
from telegram.ext import CallbackContext, CallbackQueryHandler, ConversationHandler, MessageHandler, Filters, \
    CommandHandler
from common.models import get_session, Car
from .helpHandler import reply_keyboard as default_reply_keyboard

pattern = '\D\D\d\d\d\D\D'
logger = logging.getLogger()
CHOOSE = 1


def select_car(update: Update, context: CallbackContext) -> int:
    logging.info("Seleziona auto da eliminare")
    chat_id = update.effective_chat.id
    with get_session() as session:
        cars = session.query(Car).filter_by(chat_id=chat_id).all()
        if len(cars) == 0:
            msg = "Non hai memorizzato nessuna auto"
            context.bot.send_message(chat_id=chat_id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)
        else:
            msg = "Scegli la targa dell'auto che vuoi eliminare \(o /cancel per terminare\):"
            keyboard = [[InlineKeyboardButton(c.license_plate, callback_data=c.license_plate) for c in cars]]
            reply_markup = InlineKeyboardMarkup(keyboard)
            context.bot.send_message(chat_id=chat_id, text=msg, parse_mode=ParseMode.MARKDOWN_V2,
                                     reply_markup=reply_markup)
    return CHOOSE


def delete_car(update: Update, context: CallbackContext):
    logging.info("Elimino auto")
    query = update.callback_query
    chat_id = update.effective_chat.id
    # recupero valori
    if query.data is not None:
        text = query.data
    else:
        text = update.message.text.upper()

    # controlli
    if len(text) > 7 or not re.match(pattern, text):
        logging.info(f"Targa {text} errata")
        update.message.reply_text("Formato targa errato \(Es\. AA111AA\).\nReinserisci \(o /cancel per terminare\):")
        return CHOOSE

    with get_session() as session:
        try:
            car = session.query(Car).get(text)
            if car is None or car.chat_id != chat_id:
                msg = f"Tra le tue auto non esite nessuna con targa *{text}*. (/lista per visualizzarle) "
            else:
                session.delete(car)
                session.commit()
                msg = f"Auto con targa *{text}* eliminata correttamente"
        except Exception as e:
            logging.info(f"ERROR: {e}")
            msg = f"Errore durante l'eliminazione"

    context.bot.send_message(chat_id=chat_id, text=msg, parse_mode=ParseMode.MARKDOWN_V2)
    return ConversationHandler.END


def cancel(update: Update, context: CallbackContext):
    logging.info('Eliminazione annullata')
    context.bot.send_message(chat_id=update.effective_chat.id, text='Eliminazione annullata',
                             reply_markup=ReplyKeyboardMarkup(default_reply_keyboard, one_time_keyboard=True,
                                                              resize_keyboard=True))


def delete_car_handler():
    conv_handler = ConversationHandler(
        entry_points=[MessageHandler(Filters.text & (~Filters.command) & Filters.regex('^❌ Elimina$'), select_car),
                      CommandHandler('elimina', select_car)],
        states={
            CHOOSE: [
                CallbackQueryHandler(delete_car)
            ],
        },
        fallbacks=[CommandHandler('cancel', cancel)],
    )
    return conv_handler
