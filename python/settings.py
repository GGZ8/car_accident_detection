from secret import user, password, ip
import os


class Setting:
    current_dir = os.path.dirname(os.path.abspath(__file__))
    SECRET_KEY = b'WAJX)pwa4sVZn3mxFje(Hhy_GdJXveDG@vj8JPQrkmqfCzbEs,w&B5gZqyWT'
    USE_PROXYFIX = False

    APPLICATION_ROOT = '/'
    STATIC_FOLDER = 'static'

    FLASK_APP = 'app.py'
    FLASK_RUN_HOST = '0.0.0.0'
    FLASK_RUN_PORT = 80

    FLASK_DEBUG = 0
    FLASK_ENV = "development"
    #FLASK_ENV = "production"

    DEBUG = True
    TESTING = True

    SESSION_TYPE = 'sqlalchemy' #'redis'

    FLASK_LOG_PATH = f"{current_dir}/log/flask_log.log"
    TELEGRAM_LOG_PATH = f"{current_dir}/log/telegram_log.log"

    # Database

    SQLALCHEMY_DATABASE_URI = f"sqlite:///{current_dir}/test.sqlite"  # = 'mysql://username:password@localhost/db_name'
    DB_PATH = f"{current_dir}/test.sqlite"
    #SQLALCHEMY_DATABASE_URI = f'mysql+pymysql://{user}:{password}@{ip}/db'
    SQLALCHEMY_ECHO = True
    SQLALCHEMY_TRACK_MODIFICATIONS = False

#    CACHE_TYPE = "simple"  # Flask-Caching related configs
    CACHE_DEFAULT_TIMEOUT = 100