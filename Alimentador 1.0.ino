#include <ESP8266WiFi.h>
#include "Lib_ConexionWiFi.h"
#include <NTPClient.h>
#include <WiFiUdp.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <Servo.h>
#include <HX711.h>
#define calibration_factor 895000

#define backButton  D6
#define upButton  D3
#define downButton  D4
#define selectButton  D5
#define DT D7
#define SCK D8
#define pinservo D0

//Conexion en TX BLANCO
#define A1A 3
//Conexion en RX VIOLETA
#define A1B 1
#define seguro D0
#define led D2

int menu = 1;
int submenu = 1;
int hora_servir;
int mint_servir;
int hora_aux;
int mint_aux;
bool cond_hora; 
float peso;
bool bandera;
bool modoauto=false;
bool modomanual=false;
unsigned long tiempoanterior;
byte pos = 0;
int peso_servir;
int peso_gr;

byte velocidadInicio;
byte velocidadCerrar;
byte velocidadAbrir;

Servo servo_5;
HX711 scale; 


WiFiServer server(80); //objeto de la clase WiFiServer

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ar.pool.ntp.org", -10800);

String hora_actual;
String minuto_actual;

int hh;
int mm;
int ss;

void setup() {
  conectar_WiFi();  
    
  Serial.println("");
  Serial.println("********************************************");
  Serial.print("Conectado a la red WiFi: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("macAdress: ");
  Serial.println(WiFi.macAddress());
  Serial.println("*********************************************");
  
  
  //server.begin(); //begin() levantamos el servidor 

  lcd.begin();
  lcd.backlight();
  lcd.clear();

   pinMode(seguro,INPUT_PULLUP);
  pinMode(A1A,OUTPUT);
  pinMode(A1B,OUTPUT); 
  pinMode(led,OUTPUT);

  //inicia la balanza
  scale.begin(DT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();
  
  timeClient.begin();   

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  updateMenu();
   servo_5.attach(pinservo);
  
//  if (bandera=true){
//    modoauto=true;
//    bandera=false;
//  }

tiempoanterior=millis();
}



void loop() {

   if (digitalRead(seguro)){
  
  analogWrite(A1A,200);
  analogWrite(A1B,0);
  if (!digitalRead(seguro)){
   for (velocidadCerrar =100; velocidadCerrar>0; velocidadCerrar-=50)
  analogWrite(A1A,velocidadCerrar);
  analogWrite(A1B,0);
  delay(25);
  }
  }
  
  
//para moverse en el menu
  if (!digitalRead(downButton)){
    menu++;
    updateMenu();
    delay(100);
    while (!digitalRead(downButton));
  }
  if (!digitalRead(upButton)){
    menu--;
    updateMenu();
    delay(100);
    while(!digitalRead(upButton));
  }
  if (!digitalRead(selectButton)){
    executeAction();
    updateMenu();
    delay(100);
    while (!digitalRead(selectButton));
  }


  //ME DA LA HORA MINUTOS Y SEGUNDOS
 //timeClient.update();  // 

 //hh = timeClient.getHours();
 //mm = timeClient.getMinutes();
 //ss = timeClient.getSeconds();




  }

void hora_display() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora:");
    lcd.setCursor(8, 0);
      if (hh < 10) {
      lcd.print(0);
      lcd.print(hh);
    }
    else
    {
      lcd.print(hh);
    }
    
    lcd.print(":");
    if (mm < 10) {
      lcd.print(0);
      lcd.print(mm);
    }
    else
    {
      lcd.print(mm);
    }
    lcd.print(":");
    if (ss < 10) {
      lcd.print(0);
      lcd.print(ss);
    }
    else
    {
      lcd.print(ss);
    }

   }


void updateMenu() {
  switch (menu) {
    case 0:
      menu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Automatico");
      lcd.setCursor(0, 1);
      lcd.print(" Manual");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Automatico");
      lcd.setCursor(0, 1);
      lcd.print(">Manual");
      break;
    case 3:
      lcd.clear();
      lcd.print(">Configurar");
      lcd.setCursor(0, 1);
      lcd.print(" Tarar");
      break;
    case 4:
      lcd.clear();
      lcd.print(" Configurar");
      lcd.setCursor(0, 1);
      lcd.print(">Tarar");
      break;
    case 5:
      menu = 4;
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1: 
   while(true) {
   peso = scale.get_units(), 3;
   peso_gr = ((peso * 1000));

   if(!digitalRead(selectButton) or !digitalRead(backButton) or !digitalRead(upButton) or !digitalRead(downButton)){
    lcd.backlight();
   }
   if (!digitalRead(downButton) and !digitalRead(backButton)){
    lcd.noBacklight();
    delay(300);
   }
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Modo Automatico");
   lcd.setCursor(0, 1);
   if(peso_gr<=0){
    lcd.print("000");
    lcd.print(peso_gr);
   }
    else if(peso_gr<10){
    lcd.print("00");
    lcd.print(peso_gr);
   }
   else if(peso_gr<100 and peso_gr>=10){
    lcd.print("0");
    lcd.print(peso_gr);
   }
   else {
   lcd.print(peso_gr);
   }
   lcd.setCursor(3, 1);
   lcd.print("gr");
   lcd.print("--->");
   lcd.setCursor(10, 1);
   lcd.print(peso_servir);
   lcd.setCursor(13, 1);
   lcd.print("gr");
   Serial.print(peso_gr);
   Serial.println("------");
   Serial.println(peso_servir);
   
    if (peso_gr <= peso_servir) {
  analogWrite(A1A,0);
  analogWrite(A1B,200);
    delay(15);
  }
  
  else if (peso_gr > peso_servir; peso_gr<=(peso_servir+150)) {
  analogWrite(A1A,200);
  digitalWrite(A1B,0);
 
  delay(15);
    }
   
  else if (peso_gr > (peso_servir+150)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALERTA");
    lcd.setCursor(0, 1);
    lcd.print("Exceso de comida");
   delay(5000);
    
  }
  if (!digitalRead(selectButton) and !digitalRead(backButton) ) { 
       break;
    }   
  }
  break;
 
      
    case 2:
       while(true) {
     // if((millis() - tiempoanterior) >= 1000){
   // tiempoanterior= millis();

   if(!digitalRead(selectButton) or !digitalRead(backButton) or !digitalRead(upButton) or !digitalRead(downButton)){
    lcd.backlight();
   }
   if (!digitalRead(downButton) and !digitalRead(backButton)){
    lcd.noBacklight();
    delay(300);
   }
  peso = scale.get_units(), 3;
   
 timeClient.update();  // time update from NTP server

 hh = timeClient.getHours();
 mm = timeClient.getMinutes();
 ss = timeClient.getSeconds();

 if (hora_servir==hh and mint_servir==mm and ss==00){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sirviendo comida");
  for (pos = 0; pos <= 150; pos += 20) {    
  analogWrite(A1A,0);
  analogWrite(A1B,pos);
    delay(550); 
  }
  delay(500);

    
  analogWrite(A1A,220);
  analogWrite(A1B,0);   
  delay(2000);
   }
  
 
 
 else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hora:");
    lcd.setCursor(8, 0);
      if (hh < 10) {
      lcd.print(0);
      lcd.print(hh);
    }
    else
    {
      lcd.print(hh);
    }
    
    lcd.print(":");
    if (mm < 10) {
      lcd.print(0);
      lcd.print(mm);
    }
    else
    {
      lcd.print(mm);
    }
    lcd.print(":");
    if (ss < 10) {
      lcd.print(0);
      lcd.print(ss);
    }
    else
    {
      lcd.print(ss);
    }

   lcd.setCursor(0, 1);
   lcd.print("Servir");
   lcd.setCursor(8, 1);
   lcd.print(hora_servir);
   lcd.print(":");
   lcd.print(mint_servir);
   digitalWrite(A1A,LOW);
   digitalWrite(A1B,LOW); 
 }
   

  // }
  if (!digitalRead(selectButton) and !digitalRead(backButton) ) { 
       break;
    }    
  }
  break;
       
      
    case 3:
  delay(350);
  while (true){
   
    peso = scale.get_units(), 3;
    updateSubMenu();
    if (!digitalRead(downButton)){
    submenu++;
    updateSubMenu();
    delay(100);
    while (!digitalRead(downButton));
  }
  if (!digitalRead(upButton)){
    submenu--;
    updateSubMenu();
    delay(100);
    while(!digitalRead(upButton));
  }
  if (!digitalRead(selectButton)){
    actionSubConfig();
    updateSubMenu();
    delay(100);
    while (!digitalRead(selectButton));
  }
     if (!digitalRead(backButton)) { 
       break;
  }
 }
      break;
    case 4:
      tarar();
      break;
  }
}



void updateSubMenu() {
  switch (submenu) {
    case 0:
      submenu = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print(">Hora de Servir");
      lcd.setCursor(0, 1);
      lcd.print(" Peso de comida");
      break;
    case 2:
      lcd.clear();
      lcd.print(" Hora de Servir");
      lcd.setCursor(0, 1);
      lcd.print(">Peso de comida");
      break;
    case 3:
      submenu=2;
      break;
  }
}

void actionSubConfig() {
  switch (submenu) {
    case 1:
       configuracion_hora();
       break;
    case 2:
       configuracion_peso();
       break;
  }
}         


void configuracion_hora() {
  cond_hora=true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cambiar hora de");
  lcd.setCursor(0, 1);
  lcd.print("Servir: ");
  
  int hora_aux = hora_servir;

  while(cond_hora=true) {
  
    lcd.setCursor(8, 1);
    if (hora_aux < 10) { 
      lcd.print("0");
      lcd.print(hora_aux);
    } else { 
      lcd.print(hora_aux); 
    }
    lcd.setCursor(10, 1);
    lcd.print(':');
    lcd.setCursor(11, 1);
      if (mint_servir < 10) { 
      lcd.print("0");
      lcd.print(mint_servir);
    } else { 
      lcd.print(mint_servir);}

    delay(200);

    
   if (!digitalRead(upButton)){
       hora_aux++;
       if (hora_aux > 23) {
        hora_aux = 0;
       }
    } else if (!digitalRead(downButton)) { 
     hora_aux--;
      if (hora_aux < 0) {
        hora_aux = 23;
      }
    }
    if (!digitalRead(selectButton)) { 
      hora_servir = hora_aux;
      minuto_servir(); 
       break;     
      }      
    }    
  }
     
  void minuto_servir(){
    
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cambiar min. de");
  lcd.setCursor(0, 1);
  lcd.print("Servir: ");
  
  int mint_aux = mint_servir;
  
  while(cond_hora=true) {

    lcd.setCursor(8, 1);
  if (hora_servir < 10) { 
      lcd.print("0");
      lcd.print(hora_servir);
    } else { 
      lcd.print(hora_servir); 
    }
    lcd.setCursor(10, 1);
    lcd.print(':');
    lcd.setCursor(11, 1);
     if (mint_aux < 10) { 
      lcd.print("0");
      lcd.print(mint_aux);
    } else { 
      lcd.print(mint_aux);
      }
    
    delay(200);

    
   if (!digitalRead(upButton)){
      mint_aux += 1;
      if (mint_aux > 59) {
        mint_aux = 00;
        
      }
    } else if (!digitalRead(downButton)) { 
     mint_aux -= 1;
      if (mint_aux < 0) {
        mint_aux = 59;
      }
    }

    if (!digitalRead(selectButton)) { 
      mint_servir = mint_aux;
      cond_hora=false;
       break;
    }        
  }  
}

void tarar(){
   delay(350);
   while (true){ 
    peso = scale.get_units(), 3;
    peso_gr = ((peso * 1000));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Coloque el plato");
    lcd.setCursor(0, 1);
    lcd.print("Vacio");
    lcd.setCursor(8, 1);
    if(peso_gr<=0){
    lcd.print("000");
    lcd.print(peso_gr);
   }
    else if(peso_gr<10){
    lcd.print("00");
    lcd.print(peso_gr);
   }
   else if(peso_gr<100 and peso_gr>=10){
    lcd.print("0");
    lcd.print(peso_gr);
   }
   else {
   lcd.print(peso_gr);
   }
   lcd.setCursor(11, 1);
   lcd.print("gr");
    if (!digitalRead(backButton)) { 
       break;
    }
    if (!digitalRead(selectButton)) { 
    scale.tare();
    }
  }
}

void configuracion_peso(){
  delay(350);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cambiar peso de");
    lcd.setCursor(0, 1);
    lcd.print("Servir:");
    lcd.setCursor(12, 1);
    lcd.print("gr.");
    int peso_aux = peso_servir;
  while (true){ 
    //peso = scale.get_units(), 3;
    lcd.setCursor(8, 1);
    if (peso_aux<100){
      lcd.print("0");

      
      lcd.print(peso_aux);
      }
    else {
      lcd.print(peso_aux);
    }
    
    delay(200);

    
   if (!digitalRead(upButton)){
       peso_aux +=50;
       if (peso_aux > 500) {
        peso_aux = 0;
       }
    } else if (!digitalRead(downButton)) { 
     peso_aux -= 50;
      if (peso_aux < 0) {
        peso_aux = 500;
      }
    }
    if (!digitalRead(selectButton)) { 
      peso_servir = peso_aux;       
       break;     
      }
  }
}


  
