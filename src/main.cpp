#include <Arduino.h>

#include "Adafruit_ST7735.h"

#include "ArduinoJson.h"

#include <Wire.h>
#include "RTClib.h"

#include "utils.h"
#include "model/Hora.h"
#include "model/Fecha.h"
#include "model/Clima.h"
#include "lcd.h"
#include "red.h"


#define ligth 15 //D2

Hora hora = Hora();
Fecha fecha = Fecha();
Clima clima = Clima();
Lcd lcd = Lcd();
Red red = Red();

RTC_DS3231 rtc;

void updateDataWeater() {
    bool exito = false;
    do {
        String datosTiempo = red.getWeather(true);
        exito = clima.parserJson(datosTiempo);
    } while (!exito);
}

void updateFullDate() {
    Serial.println("obteniendo full date");
    bool exito = false;
    do {
        String datosTiempo = red.getFullDate();
        Serial.println(datosTiempo);
        exito = clima.parseFullDate(datosTiempo, hora, fecha, rtc);

    } while (!exito);
}

static unsigned long timeMillis;
static unsigned long timeMillisHours = 0;
int hours = -1;




void setBrightness() {

    int l = 1;
    if (hora.hora >= 8 and hora.hora < 19) {
        l = 255;
    } else if (hora.hora == 19) {
        l = 200;
    } else if (hora.hora == 20) {
        l = 150;
     }else if (hora.hora == 21) {
        l = 100;
    } else if (hora.hora == 22) {
        l = 50;
    } else if (hora.hora >= 23 || hora.hora < 7) {
        l = 10;
    } else if (hora.hora == 7) {
        l = 150;
    }

    Serial.println("brillo: " + String(l));
    analogWrite(ligth, l);

}

void setup() {
    pinMode(ligth, OUTPUT);

    if (!rtc.begin()) {
        Serial.println(F("Couldn't find RTC"));
        while (1);
    }
    if (rtc.lostPower()) {
        // Fijar a fecha y hora de compilacion
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

        // Fijar a fecha y hora específica. En el ejemplo, 21 de Enero de 2016 a las 03:00:00
        // rtc.adjust(DateTime(2016, 1, 21, 3, 0, 0));
    }
    fecha.refresh(rtc.now());
    hora.refresh(rtc.now());
    Serial.begin(19200);

    setBrightness();

    updateDataWeater();

    //updateFullDate();

    analogWrite(ligth, 255);
    lcd.cargandoDatos();


    lcd.clear();
    lcd.printPlantilla();
    lcd.printFecha(fecha);
    lcd.printHora(hora);
    lcd.printClima(clima);
    lcd.printClimaActual(clima, hora);
    timeMillis = millis();

    hours = hora.hora;

}
static unsigned long timeSegundosMillis = 0;
bool primaryLoad = true;
bool checkHora = true;
void loop() {

    if (millis() - timeMillis > 1000) {
        timeMillis = millis();
        fecha.refresh(rtc.now());
        hora.refresh(rtc.now());

        if (hora.minuto == 0) {
            lcd.clear();
            updateDataWeater();
            lcd.printClima(clima);
        }
        lcd.printHora(hora);
        lcd.printFecha(fecha);

    }

    if (millis() - timeSegundosMillis > 60000 || primaryLoad) {
        timeSegundosMillis = millis();
        if ((hora.hora != hours and hora.minuto == 0) || primaryLoad)  {
            Serial.println(hora.toString());
            lcd.printClimaActual(clima, hora);
            hours = hora.hora;
            primaryLoad = false;
            setBrightness();
        }
    }
    if (millis() - timeSegundosMillis > 60000 ) {
        if (hora.hora == 0 && hora.minuto == 0 && checkHora) {
            updateFullDate();
            checkHora = false;
        }
        if (hora.hora == 0 && hora.minuto == 1 && !checkHora) {
            checkHora = true;
        }
    }

}