
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>  //Biblioteca do UDP.

const char *ssid = "luladrao";
const char *password = "ronaldopele";
//const char *ssid = "LAR-ECT";
//const char *password = "larfacil1234";

const int delta = 1023;

// IPAddress ip(192,168,0,43);     //ESP do Motor Shield
// IPAddress ip(192, 168, 0, 57); //Seu ESP
IPAddress ip(192, 168, 0, 18);  // Novo ESP

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(80);

// EspUDP
//int DIR = 12;
//int CIMA = 13;
//int BAIXO = 5;
//int ESQ = 15;

// Nova Pulseira
int DIR = 5;
int CIMA = 15;
int BAIXO = 13;
int ESQ = 12;
/*

/*
//EspUDP
int DIR = 12;
int CIMA = 13;
int BAIXO = 5;
int ESQ = 15;

ESPHTTP
int CIMA = 4;
int ESQ = 5;
int BAIXO  = 16;
int DIR = 0;

*/

bool once = true;

WiFiUDP udp;  // Cria um objeto da classe UDP.
String req;   // String que armazena os dados recebidos pela rede.

unsigned long previus = 0;
unsigned long previus2 = 0;

boolean listen() {  // Sub-rotina que verifica se há pacotes UDP's para serem lidos.
  if (udp.parsePacket() > 0)  // Se houver pacotes para serem lidos
  {
    req = "";  // Reseta a string para receber uma nova informaçao
    while (udp.available() > 0)  // Enquanto houver dados para serem lidos
    {
      char z = udp.read();  // Adiciona o byte lido em uma char
      req += z;             // Adiciona o char à string
    }

    // Após todos os dados serem lidos, a String estara pronta.

    //Serial.println(req);  // Printa a string recebida no Serial monitor.

    digitalWrite(D4, 0);  //-
    delay(5);             //-
    digitalWrite(D4, 1);  // Pisca o LED rapidamente apos receber a string.
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(12, OUTPUT);
  // digitalWrite(2, 0);

  pinMode(CIMA, OUTPUT);
  pinMode(ESQ, OUTPUT);
  pinMode(BAIXO, OUTPUT);
  pinMode(DIR, OUTPUT);

  // Desliga os motores

  digitalWrite(CIMA, HIGH);
  digitalWrite(ESQ, HIGH);
  digitalWrite(BAIXO, HIGH);
  digitalWrite(DIR, HIGH);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  pinMode(D4, OUTPUT);  // Habilita o LED onboard como saida.
  digitalWrite(D4, 1);  // Desliga o LED.

  udp.begin(8080);  // Inicializa a recepçao de dados UDP na porta 555
}

void loop() {
  if (once) {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());
    digitalWrite(12, HIGH);
    digitalWrite(D4, 0);
  }
  once = false;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Desconnected, Reconnecting");
    digitalWrite(12, LOW);
    once = true;
  }

  // Verifica se há pacotes para serem lidos
  if (listen()) {
    previus = millis();
    // Variáveis para o PWM de cada motor
    float val1, val2;
    val1 = 0.0;
    val2 = 0.0;

    // Serial.print("Recebi o comando, ligarei por 200ms  ");
//    Serial.print("Chegou: ");
    // Serial.println(req.substring(slash, req.length()));
//    Serial.println(req);
    int temp = req.indexOf(',');
    val1 = req.substring(0, temp).toFloat();  // Seleciona parte do primeiro PMW

    val2 = req.substring(temp + 1, req.length()).toFloat();

    val1 *= delta;
    val2 *= delta;

//    Serial.println("CIMA\tDIR\t");
//    Serial.print("\t");
//    Serial.print(val1);
//    Serial.print("\t");
//    Serial.println(val2);

    /*
    Pino 5 ---> CIMA
    Pino 4 ---> BAIXO
    Pino 16 ---> ESQ
    Pino 0 ---> DIREITA

    */

    //analogWrite(val2 > 0 ? BAIXO : CIMA, 50 + abs(val2));
    if(val2 > 0){
        analogWrite(BAIXO, abs(val2)-150 < 0 ? 0 : abs(val2)-150);
        analogWrite(CIMA,0);
        Serial.print("Escrevendo:");
        Serial.print(abs(val2)-150 < 0 ? 0 : abs(val2)-150);
        Serial.println("    no motor de BAIXO");
    }else{
        analogWrite(CIMA,abs(val2)-150 < 0 ? 0 : abs(val2)-150);
        analogWrite(BAIXO,0);

        Serial.print("Escrevendo:");
        Serial.print(abs(val2)-150 < 0 ? 0 : abs(val2)-150);
        Serial.println("    no motor de CIMA");
    }
    
    //analogWrite(val1 > 0 ? ESQ : DIR, 50 + abs(val1));

    if(val1 > 0){
        analogWrite(ESQ, abs(val1)-150 < 0 ? 0 : abs(val1)-150);
        analogWrite(DIR,0);
        Serial.print("Escrevendo:");
        Serial.print(abs(val1)-150 < 0 ? 0 : abs(val1)-150);
        Serial.println("    no motor de ESQ");
    }else{
        analogWrite(DIR,abs(val1)-150 < 0 ? 0 : abs(val1)-150);
        analogWrite(ESQ,0);
        Serial.print("Escrevendo:");
        Serial.print(abs(val1)-150 < 0 ? 0 : abs(val1)-150);
        Serial.println("    no motor de DIR");
    }
  }
  if (millis() - previus > 500) {
    analogWrite(CIMA, 0);
    analogWrite(BAIXO, 0);
    analogWrite(ESQ, 0);
    analogWrite(DIR, 0);
  }
}
