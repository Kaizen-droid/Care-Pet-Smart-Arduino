#include <Servo.h>                                                
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

#define SERVER "pw4.kyared.com/S18030173"
#define USER "Mane"
#define PASS "123"

int hay;
String jwt;
int sesion = 0;

long tiempo;
int disparador =D2;                                               // declaramos pin8 como trigger – dato de tipo entero largo
int entrada =D3;                                                  // declaramos pin7 como echo – dato de tipo entero
float distancia;                                                  // declaramos la variable distancia como dato de tipo decimal simple
Servo miniservo;                                                  //creamos el objeto miniservo para el servo pequeño
int servo = D0;
int led = 2;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(disparador, OUTPUT);                                    // declaramos disparador como salida (ultrasonidos)
  pinMode(entrada, INPUT);                                        // declaramos input como entrada (ultrasonidos)
  Serial.begin(9600);                                             // configuramos el puerto serie en 9600 baudios
  miniservo.attach(D1);                                           // iniciamos el servo pequeño con pin 10
  pinMode(servo, OUTPUT);

  Serial.println("Conectando...");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Redmi", "09876543210");
  digitalWrite(led, HIGH);
  
}

void loop() {
  digitalWrite (disparador, HIGH);                                // lanzamos un pulso para activar el sensor
  delayMicroseconds(10);                                          // lo mantenemos en alto durante 10 milisegundos
  digitalWrite(disparador, LOW);                                  // desactivamos el pin 7
  tiempo =(pulseIn(entrada, HIGH)/2);                             // medimos el pulso de respuesta
  distancia = float(tiempo * 0.0343);                             // multiplicamos el tiempo de respuesta por velocidad del sonido
  Serial.println(distancia);                                      //imprimimos la respuesta en el puerto serie
  delay(1000);                                                    // refrescamos la pantalla cada segundo
  if(distancia>12){                                               // si la distancia es menor a Xcm=
    
   }
    if ((WiFiMulti.run() == WL_CONNECTED)) {
        digitalWrite(led, LOW);
      Serial.println("Conexion exitosa");
      if(sesion){ 
        post();
        alarma();
      }
      else login();
   }
   else{  digitalWrite(led, HIGH);}
   delay(60000);
}

void login(){
  WiFiClient client;
  HTTPClient http;
  if(http.begin(client, "http://" SERVER "/carepet/login.php?user="USER"&pass="PASS"")){
    int httpCode = http.GET();
    if(httpCode > 0){
      if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED){
        String json = http.getString();
        Serial.println(json);
        int i,j;
        String r = json.substring(10,11);
        Serial.println(r);
        if(r == "y"){
          int inicio = json.indexOf("token");
          int fin = json.indexOf("}");
          jwt = json.substring(inicio+8,fin-1);
          sesion = 1;
          Serial.println("Login exitoso");
        }else{
          Serial.println("Login fallido");
        }
      }
    }else{
      Serial.println("Error GET");
    }
  }
}

void post(){
  
    WiFiClient client;
    HTTPClient http;

    http.begin(client, "http://" SERVER "/carepet/sensor.php"); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", jwt );
    int httpCode = http.POST("valor="+String(distancia));
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
      //  const String& payload = http.getString();
        Serial.println("dato insertado");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

void alarma(){
    WiFiClient client;
    HTTPClient http;

    http.begin(client, "http://" SERVER "/carepet/alarma.php"); //HTTP
    http.addHeader("Authorization", jwt );
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        String json = http.getString();
        String r = json.substring(14,15);
        if(r != "0"){
          String tam = json.substring(26,27);
          if(tam == "G"){
             if(r == "1") descarga(5000);
             else if(r == "2") {
                  descarga(5000);
                  delay(1000);
                  descarga(5000);
             } else if(r == "3") {
                  descarga(5000);
                  delay(1000);
                  descarga(5000);
                  delay(1000);
                  descarga(5000);
             }
             
          }else if(tam == "M"){
            if(r == "1") descarga(3000);
             else if(r == "2") {
             descarga(3000);
             delay(1000);
             descarga(3000);
             } else if(r == "3") {
             descarga(3000);
             delay(1000);
             descarga(3000);
             delay(1000);
             descarga(3000);
          }}else if(tam == "C"){
             if(r == "1") descarga(2000);
             else if(r == "2") {
             descarga(2000);
             delay(1000);
             descarga(2000);
             } else if(r == "3") {
             descarga(2000);
             delay(1000);
             descarga(2000);
             delay(1000);
             descarga(2000);
             }
          }
          
          Serial.println("Descarga exitoso");
          Serial.println(tam);
        }else{
          Serial.println("No hay nada agendado");
        }
         int inicio = json.indexOf("alimentar");
          int fin = json.indexOf("}");
          String ver = json.substring(inicio+12,inicio+13);
          String ver2 = json.substring(inicio+26,inicio+27);
          if(ver == "s"){           
            if(ver2 == "G"){
              descarga(5000);
            }else if(ver2 == "M"){
              descarga(3000);
            }else if(ver2 == "C"){
              descarga(2000);
            }
          }
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  void descarga(int tim){
    miniservo.write(10);                                          // Desplazamos a la posición 10º
    delay(tim/2);                                                  // esperamos 1 segundo
    miniservo.write(160);                                         // Desplazamos a la posición 170º
    delay(tim/2);                                                  // esperamos 1 segundo
    digitalWrite(servo, HIGH);                                    // Activamos el pin 9 para arrancar el sinfín
    delay(tim);
    digitalWrite(servo, LOW);
    digitalWrite(D1, LOW);                                        // dejamos el servo pequeño parado posición 10
   }
//Con este codigo vamos hacer 2 lecturas de comida al día, si el nivel del comedero está muy bajo se pondrá en funcionamiento hasta llegar al nivel programado.
