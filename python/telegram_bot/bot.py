import logging

from handlers import addCarHandler, helpHandler, deleteCarHandler
from telegram.ext import Updater, MessageHandler, Filters, CommandHandler
from common.secret import telegram_key
from telegram_bot.handlers.listCarHandler import get_car
from common.settings import Setting

logging.basicConfig(
    filename=Setting.TELEGRAM_LOG_PATH,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

logger = logging.getLogger(__name__)


def start_bot():
    global updater
    """Start the bot."""
    # Create the Updater and pass it your bot's token.
    # Make sure to set use_context=True
    updater = Updater(telegram_key, use_context=True)

    # Get the dispatcher to register handlers  (callbacks)
    dp = updater.dispatcher

    # add an handler for each command
    dp.add_handler(helpHandler.get_help_handler())
    dp.add_handler(helpHandler.get_help())

    dp.add_handler(addCarHandler.add_car_handler())

    dp.add_handler(deleteCarHandler.delete_car_handler())

    dp.add_handler(CommandHandler('lista', get_car))
    dp.add_handler(MessageHandler(Filters.text & (~Filters.command) & Filters.regex('^Lista$'), get_car))

    # Start the Bot (polling of messages)
    # this call is non-blocking
    updater.start_polling()

    return updater


if __name__ == '__main__':

    # start bot
    updater = start_bot()

    # idle (blocking)
    updater.idle()
