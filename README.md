# Car Accident Detection

## Introduzione

Car Accident Detection è un sistema in grado di rilevare incidenti stradali usando un microcontrollore **[Arduino](https://www.arduino.cc/)**, **[Flask](https://flask.palletsprojects.com/en/1.1.x/)** e **[Telegram](https://telegram.org/)**.

Il prototipo realizzato simula un dispositivo che tramite l'utilizzo di diversi sensori riesce a determinare lo stato della vettura su cui viene installato, riuscendo così a segnalare incidenti. Fornisce inoltre un servizio di notifica dell'incidente ai parenti e a un sistema di gestione centralizzato che possa permettere un rapido intervento.

## Requisiti

```
Bootstrap-Flask==1.5.1
Flask==1.1.2
Flask-RESTful==0.3.8
Flask-SQLAlchemy==2.5.1
haversine==2.3.0
marshmallow==3.11.1
PyMySQL==1.0.2
pyserial==3.5
python-dateutil==2.8.1
python-telegram-bot==13.4.1
requests==2.25.1
SQLAlchemy==1.4.3
```


## Arduino

Il dispositivo relizzato con arduino usa: 
- Sonar Sensor
- Inertial Measurement Unit (IMU)
- Flame Sensor e light Sensor
- GPS

per determinare lo stato attuale della vettura e quando avviene un incidente il sistema calcola le coordinate gps e invia tutti i dati.

## Flask

È stata realizzata una pagina di gestione che permette di visualizzare tutti gli incidednti in real time. Ogni 5 secondi viene effettuata una richiesta per verificare se sono avvenuti nuovi incidenti e vengono mostrati sulla dashboard.
Come si vede dall'immagine, viene mostrata una mappa con cui è possibile interagire selezionando il marker desiderato per ottenenere i dettagli sull'incidente. 
È poi possibile contrassegnare come risolto un incidente eliminandolo dalla mappa. 

<img src="documentation/static/web.png">

## Telegram

Telegram è stato utilizzato per realizzare un bot che permetta di registrare le auto nel sistema, in questo modo l'utente può essere avvisato se avviene un incidente alle vetture che ha censito.
È possibile:
- Inserire auto
- Ottenere la lista della auto
- Eliminare auto

Se avviene un incidente all'utente arriverà un messaggio con la posizione dell'incidente come mostrato nel video. 

<img src="documentation/static/telegram.gif" width="250">