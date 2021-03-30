import logging
import os

from flask_bootstrap import Bootstrap
from flask import Flask, render_template, jsonify
from common.settings import Setting
from common.secret import google_api
from common.models import db, Accident, get_session

os.makedirs(os.path.dirname(Setting.FLASK_LOG_PATH), exist_ok=True)
logging.basicConfig(
    filename=Setting.FLASK_LOG_PATH,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)

logger = logging.getLogger(__name__)

custom_config = Setting()

app_name = "Car Accident Detection"
app = Flask(app_name)
bootstrap = Bootstrap(app)

app.config.from_object(custom_config)
db.init_app(app)

if not os.path.isfile(Setting.DB_PATH):
    db.create_all(app=app)


@app.route('/', methods=['GET'])
def index():
    with get_session() as session:
        try:
            accidents = session.query(Accident).all()
            for a in accidents:
                a.reported = False
            session.commit()
        except Exception as e:
            logging.info(f"ERROR: {e}")
    return render_template('index.html', google_api=google_api, development=Setting.DEBUG)


@app.errorhandler(404)
def page_not_found(error):
    logging.info(error)
    return render_template('404.html'), 404


@app.route('/update', methods=['GET'])
def update():
    data = {}
    with get_session() as session:
        try:
            accidents = session.query(Accident).filter_by(reported=False).all()
            if len(accidents):
                data['accidents'] = [a.serialize for a in accidents]
                for a in accidents:
                    a.reported = True
                session.commit()
                return jsonify(data), 200
            else:
                return {}, 204
        except Exception as e:
            logging.info(data)
            data['message'] = e
            return jsonify(data), 300


@app.route('/refresh/<accident_id>', methods=['GET'])
def refresh(accident_id):
    data = {}
    with get_session() as session:
        try:
            accident = session.query(Accident).get(accident_id)
            if accident is not None:
                data['data'] = accident.serialize
                return jsonify(data), 200
            else:
                data['message'] = "Ops! You are accessing something that does not exist"
                return jsonify(data), 300
        except Exception as e:
            logging.info(data)
            data['message'] = e
            return jsonify(data), 300


@app.route('/solved/<accident_id>', methods=['POST'])
def solved(accident_id):
    data = {}
    with get_session() as session:
        try:
            accident = session.query(Accident).get(accident_id)
            session.delete(accident)
            session.commit()
            logging.info(f"RIMUOVO INCIDENTE: {accident_id}")
            data['message'] = 'Accident resolved'
            return jsonify(data), 200
        except Exception as e:
            data['message'] = e
            return jsonify(data), 300


if __name__ == '__main__':
    app.run(host=Setting.FLASK_RUN_HOST, port=Setting.FLASK_RUN_PORT)
