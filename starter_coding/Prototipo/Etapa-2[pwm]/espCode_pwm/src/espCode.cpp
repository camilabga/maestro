/*
Pino 5 ---> BAIXO
Pino 4 ---> CIMA
Pino 16 ---> DIREITA
Pino 0 ---> ESQ

0 - MAX VIBRATION
700 - MIN VIBRATION

White - 4 - ESQ
Black - 5 - UP
RED - 0 - DOWN
Green - 16 - DIR


*/


#include <Arduino.h>
#include <ESP8266WiFi.h>



const char* ssid = "LAR-ECT";
const char* password = "larfacil1234";

int luiz[100];

//IPAddress ip(192,168,0,43);     //ESP do Motor Shield
IPAddress ip(192,168,0,37);       //Seu ESP
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(80);

int CIMA = 4;
int ESQ = 5;
int BAIXO  = 16;
int DIR = 0;
bool once = true;




unsigned long previus = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(12, OUTPUT);
  //digitalWrite(2, 0);

  pinMode(CIMA, OUTPUT);
  pinMode(ESQ, OUTPUT);
  pinMode(BAIXO, OUTPUT);
  pinMode(DIR, OUTPUT);

  //Desliga os motores

  digitalWrite(16, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(0, HIGH);


  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);



}

void loop() {
  if(once){
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

  }
  once = false;

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("De");
    digitalWrite(12,LOW);
  }

  // Check if a client has connected
  //Serial.print("Checking 1");
  WiFiClient client = server.available();
  if (!client) {
    //Serial.print("Checking 2");
    //Serial.print(millis());
    //Serial.print("  --->  ");
    //Serial.print(previus);
    //Serial.println('\t');

    if (millis() - previus >= 200) {  //Se em 200ms não receber nenhuma atualização, desliga o led
      // digitalWrite(CIMA, LOW);
      // digitalWrite(BAIXO LOW);
      // digitalWrite(DIR, LOW);
      // digitalWrite(ESQ, LOW);
    }
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {

  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  int slash = req.indexOf("/") + 1;
  req = req.substring(slash, req.length());
  Serial.println(req);
  client.flush();

  // Variáveis para o PWM de cada motor
  int val1, val2, val3, val4;
  val1 = 0;
  val2 = 0;
  val3 = 0;
  val4 = 0;




  if (slash != -1) {
    previus = millis();

    //Serial.print("Recebi o comando, ligarei por 200ms  ");
    Serial.print("Chegou: ");
    //Serial.println(req.substring(slash, req.length()));


    int temp = req.indexOf("-");
    val1 = req.substring(0, temp).toInt();  // Seleciona parte do primeiro PMW

    req = req.substring(temp + 1, req.length()); //Retira o primeiro PMW
    temp = req.indexOf("-");
    val2 = req.substring(0, temp).toInt();


    req = req.substring(temp + 1, req.length()); //Retira o Segundo PMW
    temp = req.indexOf("-");
    val3 = req.substring(0, temp).toInt();

    req = req.substring(temp + 1, req.length()); //Retira o Terceiro PMW
    temp = req.indexOf("-");

    val4 = req.substring(0, temp).toInt();
  }

  Serial.println("CIMA\tBAIXO\tESQ\tDIR");
  Serial.print("\t");
  Serial.print(val1);
  Serial.print("\t"); 
  Serial.print(val2);
  Serial.print("\t");
  Serial.print(val3);
  Serial.print("\t");
  Serial.println(val4);

  /*
  Pino 5 ---> CIMA
  Pino 4 ---> BAIXO
  Pino 16 ---> ESQ
  Pino 0 ---> DIREITA

*/

  analogWrite(CIMA, val1);
  analogWrite(BAIXO, val2);
  analogWrite(ESQ, val3);
  analogWrite(DIR, val4);
  




  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLuiz";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");


}
