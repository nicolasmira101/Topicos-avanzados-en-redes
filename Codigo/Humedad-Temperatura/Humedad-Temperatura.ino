#include <ESP8266WiFi.h>
#include "DHT.h"

// Declaramos las variables de la red wi-fi que creamos y el nombre del servidor cloud
const char* ssid     = "Nicolas-Miranda";
const char* password = "TAR2019-1";
const char* host = "dweet.io";

// Definimos el pin al que estamos conectados
#define DHTPIN D1
#define DHTTYPE DHT11

// Creamos un objeto de tipo dht
DHT dht(DHTPIN, DHTTYPE);

// Valor de lectura del sensor de humedad del suelo
int humedad_suelo = 0;

void setup() {

  // Inicializamos la interfaz serial
  Serial.begin(115200);
  // Inicializamos el sensor DHT11
  dht.begin();
  delay(10);

  // Nos conectamos a la red Wi-Fi
  Serial.println();
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Wi-Fi conectado");
  Serial.println("La direccion es: ");
  Serial.println(WiFi.localIP());

}

void loop() {

  // Esperamos 2 segundos entre las mediciones.
  delay(3000);

  Serial.print("Conectado a ");
  Serial.println(host);

  // Usamos la clase WiFiClient para crear conexiones TCP
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Conexion fallida");
    return;
  }

  // Obtenemos la lectura de la humedad
  float humedad_aire = dht.readHumidity();

  // Obtenemos la lectura de la temperatura en grados celsius
  float temperatura = dht.readTemperature();


  humedad_suelo = analogRead(A0);
  Serial.print("Suelo: ");
  Serial.println(humedad_suelo);
  
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Humedad aire: ");
  Serial.println(humedad_aire);
  
  // Verificamos si existe alguna lectura fallida
  if (isnan(humedad_aire) || isnan(temperatura)) {
    Serial.println("Error al leer los datos del sensor DHT");
    return;
  }

  // Obtenemos la lectura de la humedad del suelo
  humedad_suelo = analogRead(A0);
  
  // Creamos una URI para las peticiones
  String url = "/dweet/for/monitor-jardin-2019?humedad_aire=";
  url += String(humedad_aire);
  url += "&temperatura=";
  url += String(temperatura);
  url += "&humedad_suelo=";
  url += String(humedad_suelo);

  // Envio la peticion
  Serial.print("URL de solicitud: ");
  Serial.println(url);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Cliente Timeout !");
      client.stop();
      return;
    }
  }

  // Leemos todas las lineas desde la respuesta
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  // Cerramos la conexion
  Serial.println();
  Serial.println("Cerrando conexion");

}
