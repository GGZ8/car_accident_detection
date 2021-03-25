from contextlib import contextmanager

from sqlalchemy import Column, Integer, String, Boolean, create_engine, Float, ForeignKey, DateTime
from flask_sqlalchemy import SQLAlchemy
from sqlalchemy.orm import sessionmaker, scoped_session, relationship
from .settings import Setting

db = SQLAlchemy()


@contextmanager
def get_session():
    # Create an engine which the Session will use for connections.
    engine = create_engine(f"{Setting.SQLALCHEMY_DATABASE_URI}")
    connection = engine.connect()
    # Create a Session
    session = scoped_session(sessionmaker(bind=engine, autocommit=False, autoflush=True))
    yield session
    session.close()
    connection.close()


class Car(db.Model):
    __tablename__ = 'car'
    license_plate = Column(String(length=7), primary_key=True)
    chat_id = Column(Integer)


class Accident(db.Model):
    __tablename__ = 'accident'
    id = Column(Integer, primary_key=True)
    car_id = Column(String(length=7), ForeignKey('car.license_plate'))
    car = relationship('Car')
    date_time = Column(DateTime, nullable=False)
    temperature = Column(Integer, nullable=False)
    fire = Column(Boolean, nullable=False)
    frontal = Column(Boolean, nullable=False)
    tilt = Column(Boolean, nullable=False)
    fall = Column(Boolean, nullable=False)
    lat = Column(Float(precision=32), nullable=False)
    lng = Column(Float(precision=32), nullable=False)
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
        }

    @property
    def serialize_pos(self):
        return {
            'date_time': self.date_time.strftime("%d-%m-%YT%H:%M:%S"),
            'lat': self.lat,
            'lng': self.lng,
        }