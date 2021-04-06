import logging
import os

from flask_restful import Api, Resource
from flask import Flask, request
from common.settings import Setting
from common.models import get_session, Accident, Car
from marshmallow import Schema, fields, ValidationError, validate
from telegram_bot.handlers import accidentHandler
from datetime import datetime

app = Flask(__name__)
api = Api(app)

os.makedirs(os.path.dirname(Setting.FLASK_API_LOG_PATH), exist_ok=True)
logging.basicConfig(
    #filename=Setting.FLASK_API_LOG_PATH,
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)


AccidentSchema = Schema().from_dict({
    'lat': fields.Float(required=True),
    'lng': fields.Float(required=True),
    'frontal': fields.Bool(required=True),
    'tilt': fields.Bool(required=True),
    'fire': fields.Bool(required=True),
    'fall': fields.Bool(required=True),
    'temp': fields.Float(required=True),
    'license_plate': fields.Str(required=True, validate=validate.Length(7)),
    'date': fields.DateTime(required=True),
})


def validate_json_data(body):
    try:
        AccidentSchema().load(body)
        return True
    except ValidationError as err:
        print(err.messages)
        return False


class AccidentReport(Resource):

    @staticmethod
    def get():
        data = {}
        with get_session() as session:
            try:
                accidents = session.query(Accident).all()
                if len(accidents):
                    data['accidents'] = [a.serialize_pos for a in accidents]
                    return data, 200
                else:
                    return None, 204
            except Exception as e:
                logging.info(e)
                return None, 300

    @staticmethod
    def post():
        if request.is_json:
            json = request.get_json()
        else:
            return None, 300

        if not validate_json_data(json):
            return None, 300
        else:
            json['date'] = datetime.strptime(json['date'], '%Y-%m-%d %H:%M:%S%z')

        with get_session() as session:
            try:
                if session.query(Car).get(json['license_plate']) is None:
                    car = Car(license_plate=json['license_plate'], chat_id=None)
                    session.add(car)
                    session.commit()

                accident = Accident(car_id=json['license_plate'], date_time=json['date'], temperature=json['temp'],
                                    fire=json['fire'], frontal=json['frontal'], tilt=json['tilt'], fall=json['fall'],
                                    lat=json['lat'], lng=json['lng'], reported=False)
                session.add(accident)
                session.commit()
                if session.query(Car).get(json['license_plate']).chat_id is not None:
                    accident_data = {'car_id': json['license_plate'], 'date': json['date'], 'lat': json['lat'],
                                     'lng': json['lng'], 'chat_id': accident.car.chat_id}
                    accidentHandler.accident_message(accident_data)
                return None, 200
            except Exception as e:
                logging.info(e)
                return None, 500


api.add_resource(AccidentReport, f'/{Setting.API_VERSION}/accidents')

if __name__ == '__main__':
    app.run(host=Setting.FLASK_RUN_HOST, port=Setting.FLASK_RUN_API_PORT)
