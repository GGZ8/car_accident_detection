from contextlib import contextmanager

from sqlalchemy import Column, Integer, String, Boolean, create_engine, Float, ForeignKey, DateTime
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.orm import sessionmaker, scoped_session, relationship
from settings import Setting

db = SQLAlchemy()


@contextmanager
def get_session():
    # Create an engine which the Session will use for connections.
    engine = create_engine(f"sqlite:///{Setting.SQLALCHEMY_DATABASE_URI}")
    connection = engine.connect()
    # Create a Session
    session = scoped_session(sessionmaker(bind=engine, autocommit=False, autoflush=True))
    yield session
    session.close()
    connection.close()


class TelegramUser(db.Model):
    __tablename__ = 'telegram_user'
    user_id = Column(Integer, primary_key=True)
    cars = relationship('Car')


class Accident(db.Model):
    __tablename__ = 'accident'
    id = Column(Integer, primary_key=True)
    car_id = Column(String, ForeignKey('car.license_plate'))
    car = relationship('Car')
    date_time = Column(DateTime, nullable=False)
    temperature = Column(Integer, nullable=False)
    fire = Column(Boolean, nullable=False)
    frontal = Column(Boolean, nullable=False)
    tilt = Column(Boolean, nullable=False)
    fall = Column(Boolean, nullable=False)
    lat = Column(Float, nullable=False)
    lng = Column(Float, nullable=False)
    reported = Column(Boolean, nullable=False)

    @property
    def serialize(self):
        s = self.car.license_plate
        return {
            'id': self.id,
            'license_plate':  s[:2] + " " + s[2:5] + " " + s[5:7],
            'temperature': self.temperature,
            'date': self.date_time.strftime("%d/%m/%Y"),
            'time': self.date_time.strftime("%H:%M:%S"),
            'fire': self.fire,
            'frontal': self.frontal,
            'tilt': self.tilt,
            'fall': self.fall,
            'lat': self.lat,
            'lng': self.lng,
            'reported': self.reported,
        }


class Car(db.Model):
    __tablename__ = 'car'
    license_plate = Column(String(length=7), primary_key=True)
    user_id = Column(Integer, ForeignKey('telegram_user.user_id'))
    chat_id = Column(Integer, nullable=False)
