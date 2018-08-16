#include <ESP8266WiFi.h>

const char* ssid = "LAR-ECT";
const char* password = "larfacil1234";

int luiz[100];
WiFiServer server(80);

unsigned long previus = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  pinMode(5, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);


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
  WiFiClient client = server.available();
  if (!client) {
    //Serial.print("Checking --> ");
    //Serial.print(millis());
    //Serial.print("  --->  ");
    //Serial.print(previus);
    //Serial.println('\t');

    if (millis() - previus >= 200) {  //Se em 200ms não receber nenhuma atualização, desliga o led
      digitalWrite(5, LOW);
      digitalWrite(4, LOW);
      digitalWrite(16, LOW);
      digitalWrite(0, LOW);
    }
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {

  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val1, val2, val3, val4;
  val1 = 0;
  val2 = 0;
  val3 = 0;
  val4 = 0;


  int slash = req.indexOf("/");
  if (slash != -1) {
    previus = millis();
    digitalWrite(5, !digitalRead(5));
    //Serial.print("Recebi o comando, ligarei por 200ms  ");
    Serial.print("Chegou: ");
    Serial.println(req.substring(slash + 1, slash + 2));
    val1 = req.substring(slash + 1, slash + 2).equals("0") ? 0 : 1;
    val2 = req.substring(slash + 2, slash + 3).equals("0") ? 0 : 1;
    val3 = req.substring(slash + 3, slash + 4).equals("0") ? 0 : 1;
    val4 = req.substring(slash + 4, slash + 5).equals("0") ? 0 : 1;

  }

  digitalWrite(5, val1);
  digitalWrite(4, val2);
  digitalWrite(16, val3);
  digitalWrite(0, val4);


  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nLuiz";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

