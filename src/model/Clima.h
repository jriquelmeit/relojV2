class HoraClima{
public:
    int temp = 0;
    int windchill = 0;
    String desc = "";

    String toString(){
        return String(temp) +"-"+ String(windchill) +"-"+ desc;
    }

};


class Dia{
public:
    int min = 0;
    int max = 0;
    String nombre = "";

    String toString(){
        return String(min)  + " - " + String(max) + nombre;
    };

};

class Clima{

private:
    Dia hoy;
    Dia siguiente;
    HoraClima horaClima[25] ;
public:

    Clima(){
        hoy = Dia();
        siguiente = Dia();
    }

    Dia getHoy(){
        return hoy;
    }
    Dia getSiguiente(){
        return siguiente;
    }
    HoraClima getHoraClima(int hour){
        return horaClima[hour];
    }
    bool parserJson(String weather){
        DynamicJsonBuffer jsonBuffer;

        JsonObject &root = jsonBuffer.parseObject(weather);

        hoy.min = root["a"]["1"].as<int>();
        hoy.max = root["a"]["2"].as<int>();
        hoy.nombre = root["day"]["a"].as<String>();

        siguiente.min = root["b"]["1"].as<int>();
        siguiente.max = root["b"]["2"].as<int>();
        siguiente.nombre = root["day"]["b"].as<String>();


        for (int i = 0; i < 24; i++) {
            horaClima[i] = HoraClima();
            horaClima[i].temp = root["hour"][i]["temp"].as<int>();
            horaClima[i].windchill = root["hour"][i]["windchill"].as<int>();
            horaClima[i].desc = root["hour"][i]["desc"].as<String>();
        }

        Serial.println(weather);

        return root.success();

    }

    void dummy(){


        hoy.max = 21;
        hoy.min = 12;
        hoy.nombre = "Sabado";

        siguiente.max = 34;
        siguiente.min = 20;
        siguiente.nombre = "Domingo";
    }

};

