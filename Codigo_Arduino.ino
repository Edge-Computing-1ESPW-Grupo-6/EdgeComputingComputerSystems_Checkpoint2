// incluindo biblioteca do LCD
#include <LiquidCrystal.h>

LiquidCrystal lcd (8, 9, 13, 12, 11, 10);  // define os pinos do display LCD

int led_vermelho2 = 2; // define o pino do LED vermelho 2
int led_amarelo2 = 3; // define o pino do LED amarelo 2
int led_vermelho1 = 4; // define o pino do LED vermelho 1
int led_amarelo1 = 5; // define o pino do LED amarelo 1  
int led_verde = 6; // define o pino do LED verde
int buzzerPin = 7; // define o pino da buzina
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

// variáveis de alarme para leituras fora da faixa ideal
bool luzMeia = false;
bool luzAlta = false;
bool umidAlta = false;
bool umidBaixa = false;
bool tempAlta = false;
bool tempBaixa = false;
bool exibirAlerta = false;

// variáveis para controle do loop de mensagens exibidas no LCD
int mensagens[3] = {1, 2, 3};
int mensagem_atual = 0;
unsigned long tempo_mensagem_anterior = 0;
unsigned long tempo_mensagem_atual = 0;
const unsigned long intervalo = 1700;

void setup() {
  lcd.begin(16, 2); // inicializa o display LCD
  pinMode(led_verde, OUTPUT); // define o pino do LED verde como saída
  pinMode(led_amarelo1, OUTPUT); // define o pino do LED amarelo 1 como saída
  pinMode(led_amarelo2, OUTPUT); // define o pino do LED amarelo 2 como saída
  pinMode(led_vermelho1, OUTPUT); // define o pino do LED vermelho 1 como saída
  pinMode(led_vermelho2, OUTPUT); // define o pino do LED vermelho 2 como saída
  pinMode(buzzerPin, OUTPUT); // define o pino do piezo como saída
  pinMode(sensorLDR, INPUT); // define o pino do LDR como entrada
  pinMode(sensorTemp, INPUT); // define o pino do sensor de temperatura como entrada
  pinMode(potenciometro, INPUT); // define o pino do potenciometro como entrada
  Serial.begin(9600); //inicia a comunicação serial

}

void loop() {
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
  
  // verifica se a luminosidade é baixa ou nula, acende o LED verde e define os alarmes
  if (luminosidade <900) {
    digitalWrite(led_verde, HIGH);
    digitalWrite(led_amarelo1, LOW);
    digitalWrite(led_vermelho1, LOW);
    luzMeia = false;
    luzAlta = false;
  }
  // verifica se a luminosidade é média, acende o LED amarelo 1 e define os alarmes
  else if (luminosidade >= 900 && luminosidade < 950) {
  	digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo1, HIGH);
    digitalWrite(led_vermelho1, LOW);
    luzMeia = true;
    luzAlta = false;
  }
  // verifica se a luminosidade é alta, acende o LED vermelho 1 e define os alarmes
  else if (luminosidade >= 950) { 
  	digitalWrite(led_verde, LOW);
    digitalWrite(led_amarelo1, LOW);
    digitalWrite(led_vermelho1, HIGH);
    luzMeia = false;
    luzAlta = true;
   }
  // verifica se a umidade está baixa, acende o LED vermelho 2 e define os alarmes
  if (umidade < 60) {
    digitalWrite(led_vermelho2, HIGH);
    umidAlta = false;
    umidBaixa = true;
  }
     // verifica se a umidade está dentro da faixa ideal e define os alarmes
  else if (umidade >= 60 && umidade <= 80) {
    digitalWrite(led_vermelho2, LOW);
    umidAlta = false;
    umidBaixa = false;
  }
   // verifica se a umidade está alta, acende o LED vermelho 2 e define os alarmes
  else if (umidade > 80) {
    digitalWrite(led_vermelho2, HIGH);
    umidAlta = true;
    umidBaixa = false;
  }

  // verifica se a temperatura está baixa, acende o LED amarelo 2 e define os alarmes
  if (temp < 10) {
    digitalWrite(led_amarelo2, HIGH);
    tempAlta = false;
    tempBaixa = true;
  }
  // verifica se a temperatura está dento da faixa ideal e define os alarmes
  else if (temp >= 10 && temp <= 15) {
    digitalWrite(led_amarelo2, LOW);
    tempAlta = false;
    tempBaixa = false;
  }
  // verifica se a temperatura está alta, acende o LED amarelo 2 e define os alarmes
  else if (temp > 15) {
    digitalWrite(led_amarelo2, HIGH);
    tempAlta = true;
    tempBaixa = false;
  }
  
  // verifica se algum valor está fora da faixa ideal, aciona o buzzer caso sim, desliga caso não
  if (luminosidade >= 900 || umidade < 60 || umidade > 80 || temp <10 || temp > 15){
  tone(buzzerPin,600);
  }
  else {
  noTone(buzzerPin);
  }

  // verifica se algum valor está fora da faixa ideal, define a existência de um alerta caso sim, inexistência caso não
  if (luminosidade >= 900 || umidade < 60 || umidade > 80 || temp <10 || temp > 15){
  exibirAlerta = true;
  }
  else {
  exibirAlerta = false;
  }
  
  // verifica o tempo que passou desde a execução do sistema/mudança de mensagem no LCD, altera para a próxima mensagem ou reinicia o loop
  tempo_mensagem_atual = millis();
  if (tempo_mensagem_atual - tempo_mensagem_anterior == intervalo) {
  mensagem_atual++;
    // limpa o lcd para nova mensagem caso não exista alerta ativo
  	if (luzMeia == false && luzAlta == false && umidAlta == false && umidBaixa == false && tempAlta == false && tempBaixa == false){
  	lcd.clear();
  	}
    if (mensagem_atual > 2) {
      mensagem_atual = 0;
    }
    tempo_mensagem_anterior = tempo_mensagem_atual;
    
    // define as mensagens a serem escritas de acordo com as leituras de temperatura, umidade e luminosidade
    if (mensagens[mensagem_atual] == 1 && exibirAlerta == false) {
      lcd.setCursor(3, 0);
      lcd.print("Temp Total");
      lcd.setCursor(3, 1);
      lcd.print("Temp = " + String(temp) + "C");
    } if (mensagens[mensagem_atual] == 2 && exibirAlerta == false) {
      lcd.setCursor(1, 0);
      lcd.print("Umidade Total");
      lcd.setCursor(1, 1);
      lcd.print("Umidade = " + String(umidade) + "%");
    } if (mensagens[mensagem_atual] == 3 && exibirAlerta == false) {
      lcd.setCursor(2, 0);
      lcd.print("Luminosidade");
      lcd.setCursor(5, 1);
      lcd.print("Ideal");
      
    // define as mensagens a serem escritas de acordo com as leituras de temperatura, umidade e luminosidade, considerando que estejam fora da faixa ideal
    } if (mensagens[mensagem_atual] == 1 && tempAlta == true) {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Temp. ALTA");
      lcd.setCursor(3, 1);
      lcd.print("Temp = " + String(temp) + "C");
    } if (mensagens[mensagem_atual] == 1 && tempBaixa == true) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Temp. BAIXA");
      lcd.setCursor(2, 1);
      lcd.print("Temp = " + String(temp) + "C");
    } if (mensagens[mensagem_atual] == 1 && tempAlta == false && tempBaixa == false && exibirAlerta == true) {
      mensagem_atual++;
    } if (mensagens[mensagem_atual] == 2 && umidAlta == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Umidade ALTA");
      lcd.setCursor(1, 1);
      lcd.print("Umidade = " + String(umidade) + "%");
    } if (mensagens[mensagem_atual] == 2 && umidBaixa == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Umidade BAIXA");
      lcd.setCursor(1, 1);
      lcd.print("Umidade = " + String(umidade) + "%");
    } if (mensagens[mensagem_atual] == 2 && umidBaixa == false && umidAlta == false && exibirAlerta == true) {
      mensagem_atual++;
    } if (mensagens[mensagem_atual] == 3 && luzMeia == true) {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Ambiente a");
      lcd.setCursor(4, 1);
      lcd.print("MEIA LUZ");
    } if (mensagens[mensagem_atual] == 3 && luzAlta == true) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Ambiente muito");
      lcd.setCursor(5, 1);
      lcd.print("CLARO");
    } if (mensagens[mensagem_atual] == 3 && luzAlta == false && luzMeia == false && exibirAlerta == true) {
      mensagem_atual++;
    }
  }
}