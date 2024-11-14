#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Wire.h>
#include "DHT.h"

// Configurações Wi-Fi
const char* ssid = "DavidMedeiros";
const char* password = "DavidMedeiros69*";

// Configurações do Firebase
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

#define DHTPIN 14 // Pino de dados do DHT11, na porta DIGITAL 14
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Função para obter data e hora atuais
String getDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Falha ao obter o horário";
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

int analogicoMQ2 = 34;
int digitalMQ2 = 25;
int buzzer = 27;
int ledOK = 18;
int ledALERTA = 19;
int ledPERIGO = 21;
int alarmeMQ2 = 1500;
bool flagInicioCritico = false;
int millisSituacaoCritica = 0;

void setup() {

  Serial.begin(115200);

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado ao Wi-Fi");

  // Configurar sincronização de horário NTP
  configTime(0, 0, "pool.ntp.org");

  // Configurar Firebase
  config.host = "meio-ambiente-12345.firebaseio.com";
  config.api_key = "AIzaSyAiiEqdMOtivQJJXaaAIOGVjQBWl_QWLak";
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(ledOK, OUTPUT);
  pinMode(ledALERTA, OUTPUT);
  pinMode(ledPERIGO, OUTPUT);
  pinMode(analogicoMQ2, INPUT);

}

void loop() {
  int valorMQ2 = analogRead(analogicoMQ2);
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Obter data e hora atuais
  String dataHora = getDateTime();

  //dados da serial para depuracao dos dados
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.print(" °C | Umidade: ");
  Serial.print(umidade);
  Serial.println(" %");
  Serial.print("Detecção de gás: ");
  Serial.println(valorMQ2);
  Serial.print("Data: ");
  Serial.println(dataHora);
  Serial.print("------------------------------");
  Serial.println();

  //Tratamento dos dados para ligacao dos leds de alerta
  if(valorMQ2 <= 2048){
    digitalWrite(ledOK, HIGH);
    digitalWrite(ledPERIGO, LOW);
  }
  else if(valorMQ2 >= 2049 && valorMQ2 <= 3072){
    //piscaMediano
    digitalWrite(ledOK, LOW);
    digitalWrite(ledPERIGO, LOW);
    flagInicioCritico = false;

    digitalWrite(ledALERTA, HIGH);
    delay(500);
    digitalWrite(ledALERTA, LOW);
    delay(500);
  }
  else{
    //piscaFodeu
    digitalWrite(ledOK, LOW);
    digitalWrite(ledALERTA, LOW);
    
    if (flagInicioCritico == false){
      flagInicioCritico = true;
      digitalWrite(ledPERIGO, HIGH);
      millisSituacaoCritica = millis();
    }

    if (flagInicioCritico == true && millisSituacaoCritica + 100 < millis()){
      digitalWrite(ledPERIGO, !digitalRead(ledPERIGO));
      millisSituacaoCritica = millis();
    }
  }

  // Estrutura JSON para enviar os dados
  FirebaseJson json;
  json.set("temperatura", temperatura);
  json.set("umidade", umidade);
  json.set("co2", valorMQ2);
  json.set("dataHora", dataHora);

  // Enviar dados para o Firestore
  String path = "/main" + String(millis());
  if (Firebase.setJSON(firebaseData, path.c_str(), json)) {
    Serial.println("Dados enviados com sucesso!");
  } else {
    Serial.println("Falha ao enviar dados:");
    Serial.println(firebaseData.errorReason());
  }

  // Aguardar 10 segundos antes de enviar novamente
  delay(10000);
}