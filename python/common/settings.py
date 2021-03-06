from .secret import user, password, ip
import os


class Setting:
    current_dir = os.path.abspath(os.path.join(os.getcwd(), os.pardir))
    print(current_dir)
    SECRET_KEY = b'WAJX)pwa4sVZn3mxFje(Hhy_GdJXveDG@vj8JPQrkmqfCzbEs,w&B5gZqyWT'
    USE_PROXYFIX = False

    APPLICATION_ROOT = '/'
    STATIC_FOLDER = 'static'

    FLASK_APP = 'app.py'
    FLASK_RUN_HOST = '0.0.0.0'
    FLASK_RUN_PORT = 80
    FLASK_RUN_API_PORT = 8000

    FLASK_DEBUG = 1
    FLASK_ENV = "development"
    #FLASK_ENV = "production"

    DEBUG = False
    TESTING = True

    SESSION_TYPE = 'sqlalchemy'

    FLASK_LOG_PATH = f"{current_dir}/log/flask_log.log"
    FLASK_API_LOG_PATH = f"{current_dir}/log/flask_api_log.log"
    TELEGRAM_LOG_PATH = f"{current_dir}/log/telegram_log.log"

    # Database

    SQLALCHEMY_DATABASE_URI = f"sqlite:///{current_dir}/test.sqlite"  # = 'mysql://username:password@localhost/db_name'
    DB_PATH = f"{current_dir}/test.sqlite"
    #SQLALCHEMY_DATABASE_URI = f'mysql+pymysql://{user}:{password}@{ip}/db'
    SQLALCHEMY_ECHO = True
    SQLALCHEMY_TRACK_MODIFICATIONS = False

    CACHE_DEFAULT_TIMEOUT = 100

    API_VERSION = 'api/v1'
