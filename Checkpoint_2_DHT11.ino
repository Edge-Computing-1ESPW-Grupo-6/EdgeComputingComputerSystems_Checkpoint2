#include "Arduino.h"
#include "WiFi.h"
#include "HTTPClient.h"

char ssid[] = "SSID do WIFI";
char pass[] = "Senha do WIFI";

char serverAddress[] = "https://api.tago.io/data";
char contentHeader[] = "application/json";
char tokenHeader[]   = "TOKEN da TAGO"; //Token do Dispositivo

HTTPClient client;

int potenciometro = A3; // define o pino do potenciometro (simulando um sensor de umidade)
int sensorTemp = A4; // define o pino do sensor de temperatura
const int sensorLDR = A5; // define o pino do sensor de luz

int luminosidade; // variável para armazenar a luminosidade lida do sensor

//variáveis para conversão de volts (resultado do sensor de temperatura) em temperatura
int tempRaw= 0; 
double tempVolt = 0;

int temp; // variável para armazenar a temperatura lida do sensor

int umidade; // variável para armazenar a umidade calculada a partir do potenciometro

// variáveis para armazenar leituras de temperatura em array
const int array_temp = 5;
float valor_temp[array_temp];
int index_temp = 0;

// variáveis para armazenar leituras de umidade em array
const int array_umid = 5;
float valor_umid[array_umid];
int index_umid = 0;

// variáveis para armazenar leituras de luminosidade em array
const int array_luz = 5;
float valor_luz[array_luz];
int index_luz = 0;




void init_wifi() {
  Serial.println("Conectando WiFi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Conectado");
  Serial.print("Meu IP é: ");
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(9600);
  init_wifi(); 

  pinMode(sensorLDR, INPUT); // define o pino do LDR como entrada
  pinMode(sensorTemp, INPUT); // define o pino do sensor de temperatura como entrada
  pinMode(potenciometro, INPUT); // define o pino do potenciometro como entrada

}

void loop() {
  lerTemp();
  lerUmi();
  lerLumi();

  char anyData[30];
  char postData[300];
  char anyData1[30];
  char bAny[30];
  int statusCode = 0;

  strcpy(postData, "{\n\t\"variable\": \"Temperatura_Media\",\n\t\"value\": ");
  dtostrf(temp, 6, 2, anyData);
  strncat(postData, anyData, 100);
  strcpy(anyData1, ",\n\t\"unit\": \"C\"");
  strncat (postData, anyData1, 100);
  strcpy(postData, "{\n\t\"variable\": \"Umidade_Media\",\n\t\"value\": ");
  dtostrf(umidade, 6, 2, anyData);
  strncat(postData, anyData, 100);
  strcpy(anyData1, ",\n\t\"unit\": \"%\"");
  strncat (postData, anyData1, 100);
  strcpy(postData, "{\n\t\"variable\": \"Luminosidade_Media\",\n\t\"value\": ");
  dtostrf(luminosidade, 6, 2, anyData);
  strncat(postData, anyData, 100);
  strcpy(anyData1, "\n\t}\n");
  strncat (postData, anyData1, 100);
  Serial.println(postData);

  client.begin(serverAddress);
  client.addHeader("Content-Type", contentHeader);
  client.addHeader("Device-Token", tokenHeader);
  statusCode = client.POST(postData);

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.println("End of POST to TagoIO");
  Serial.println();
  delay(5000);
}

void lerTemp(){
  // faz a leitura da temperatura e converte para graus Celsius
  tempRaw = analogRead(sensorTemp);
  tempVolt = (tempRaw / 1023.0) * 5000; // 5000 para resultado em millivots.
  int temp_atual = (tempVolt-500) * 0.1;
  
  // armazena as leituras de temperatura em array, substituindo o valor mais antigo a cada nova leitura
  valor_temp[index_temp] = temp_atual;
  index_temp++;
  if (index_temp >= array_temp) {
    index_temp = 0;
  }
  
  // calcula a temperatura média das últimas 5 leituras (armazenadas em array)
  int soma_temp = 0;
  for (int i = 0; i < array_temp; i++) {
    soma_temp += valor_temp[i];
  }
  temp = soma_temp / array_temp;
}

void lerUmi(){
  // faz a leitura da umidade e converte para porcentagem
  int umidadeValue = analogRead(potenciometro);
  int umid_atual = umidadeValue / 10.23;
  
  // armazena as leituras de umidade em array, substituindo o valor mais antigo a cada nova leitura
  valor_umid[index_umid] = umid_atual;
  index_umid++;
  if (index_umid >= array_umid) {
    index_umid = 0;
  }
 
  // calcula a umidade média das últimas 5 leituras (armazenadas em array)
  int soma_umid = 0;
  for (int i = 0; i < array_umid; i++) {
    soma_umid += valor_umid[i];
  }
  umidade = soma_umid / array_umid;
}

void lerLumi(){
  // verifica a luminosidade
  int luz_atual = analogRead(sensorLDR);
  
  // armazena as leituras de luminosidade em array, substituindo o valor mais antigo a cada nova leitura
  valor_luz[index_luz] = luz_atual;
  index_luz++;
  if (index_luz >= array_luz) {
    index_luz = 0;
  }
  
  // calcula a luminosidade média das últimas 5 leituras (armazenadas em array)
  int soma_luz = 0;
  for (int i = 0; i < array_luz; i++) {
    soma_luz += valor_luz[i];
  }
  luminosidade = soma_luz / array_luz;
}