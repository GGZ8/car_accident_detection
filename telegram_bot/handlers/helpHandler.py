from telegram import ReplyKeyboardMarkup, ParseMode, Update
from telegram.ext import CommandHandler, Filters, MessageHandler, CallbackContext

reply_keyboard = [['Inserisci auto', 'Elimina auto', 'Lista auto', 'Aiuto']]


def help_command(update: Update, context: CallbackContext):
    """Send a message when the command /start /help o "Aiuto" is issued."""
    msg = (
        f"Ciao {update.message.from_user.first_name}\! \n"
        "Questo bot ti aiuta nel monitorare gli incidenti stradali delle tue vetture\. Ti permette di aggiungere "
        "vetture, eliminarle, modificarle ed essere informato se avviene un incidente alle vetture inserite\. \n\n "
        "*COMANDI SUPPORTATI*: \n"
        "\n/lista \- permette di visualizzare le vetture salvate"
        "\n/inserisci \- consente di inserire una vettura"
        "\n/elimina \- consente di eliminare una vettura della lista"
        "\n/help \- mostra un messaggio di aiuto"
    )
    context.bot.send_message(chat_id=update.effective_chat.id, text=msg, parse_mode=ParseMode.MARKDOWN_V2,
                             reply_markup=ReplyKeyboardMarkup(reply_keyboard, one_time_keyboard=True,
                                                              resize_keyboard=True))


def get_help_handler():
    return CommandHandler(('start', 'help'), help_command)


def get_help():
    return MessageHandler(Filters.text & (~Filters.command) & Filters.regex('^Aiuto$'), help_command)
