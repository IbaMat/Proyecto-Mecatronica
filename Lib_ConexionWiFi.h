//Libreria de wifi
#include <ESP8266WiFi.h>

//Libreria para conectarse a una red y agregar internet
#include <strings_en.h>
#include <WiFiManager.h>

//Libreria para crear un server DNS
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <Ticker.h>
#define pinLedWifi D4
//Instancia a la clase Ticker
Ticker ticker;

#define pinLedEncendido D7

//Funcion de parpadeo
void parpadeoLedWiFi (){
  //Cambiar el estado del LED
  byte estado = digitalRead(pinLedWifi);
  digitalWrite(pinLedWifi, !estado);
}

void conectar_WiFi(){
  Serial.begin(115200);
  
  //LED de conexion exitosa
  pinMode(pinLedEncendido, OUTPUT);

  //Modo del pin
  pinMode(pinLedWifi, OUTPUT);
  //Empezamos el Temporizador que hara parpadear el LED
  ticker.attach(0.2, parpadeoLedWiFi);

  //creamos una instancia de la clse wifimanager
  WiFiManager wifiManager;
  
  //descomentar para resetear configuracion
  //wifiManager.resetSettings();

  //Creamos AP y portal cautivo y comprobamos si se establece la conexion
 if(!wifiManager.autoConnect("Proyecto_ Meca")){
  Serial.println("Fallo en la conexion (timeout)");
  ESP.reset();
  delay(1000);
 }

  Serial.println("Ya esta conectado");

  //Eliminamos el temporizador (esto detiene el temporizador, no apaga el led)
  ticker.detach();

  //Apagamos el LED si queda encendido
  digitalWrite(pinLedWifi, HIGH);

 // Enciende el LED cuando la conexion es exitosa
 if (WiFi.status()  == WL_CONNECTED) {
 digitalWrite(pinLedEncendido, HIGH);
 }
}
