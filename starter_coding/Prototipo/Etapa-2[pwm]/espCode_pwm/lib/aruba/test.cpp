/*
Pino 5 ---> CIMA
Pino 4 ---> BAIXO
Pino 16 ---> ESQ
Pino 0 ---> DIREITA

*/


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Pwm.h>


const char* ssid = "LAR-ECT";
const char* password = "larfacil1234";

int luiz[100];
WiFiServer server(80);
/*
int CIMA = 5;
int ESQ = 16;
int BAIXO  = 4;
int DIR = 0;
*/

Pwm cima(5);
Pwm esq(16);
Pwm baixo(4);
Pwm dir(0);

unsigned long previus = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  // pinMode(CIMA, OUTPUT);
  // pinMode(ESQ, OUTPUT);
  // pinMode(BAIXO, OUTPUT);
  // pinMode(DIR, OUTPUT);


  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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
  digitalWrite(2, HIGH);

}

void loop() {


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

  Serial.println("Var 1:\tVar 2\tVar 3:\tVar 4: ");
  Serial.print("\t");
  Serial.print(val1);
  Serial.print("\t");
  Serial.print(map(val2,0,255,200,0));
  Serial.print("\t");
  Serial.print(val3);
  Serial.print("\t");
  Serial.println(val4);

  /*
  Pino 5 ---> CIMA
  Pino 4 ---> BAIXO
  Pino 16 ---> ESQ
  Pino 0 ---> DIREITA



  analogWrite(CIMA, val1);
  analogWrite(BAIXO, val2);
  analogWrite(ESQ, val3);
  analogWrite(DIREITA, val4);
  */
  cima.write(val1);
  baixo.write(val2);
  esq.write(val3);
  dir.write(val4);


  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLuiz";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  cima.write(val1);
  baixo.write(val2);
  esq.write(val3);
  dir.write(val4);
}
