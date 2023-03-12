/* Create a WiFi access point and provide a web server on it.
Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.*/   
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <SPI.h>

#define Vref 0.1 // tension de reference du MCP3301 en V

const int csPin = D8;  // Chip Select MCP3301
const char *ssid = "ICIESP";// Identifiant WiFi
const char *password = "BASTIAN123";// Mot de passe WiFi
const int LM335 = A0;

char header[] = "<!DOCTYPE html><html><head><title>Thermocouple</title><meta charset=\"utf-8\"></head><body>";
char footer[] = "</body></html>";

int i=0, adcValue=0;
float tempSC=0, maxi=0, tensionmV=0, mini=0, moy=0, acc=0;

byte highByte=0x00, lowByte=0x00;

ESP8266WebServer server(80); // On instancie un serveur qui ecoute sur le port 80

void PageWeb(){  
  char buffer[1500];

  strcpy(buffer, header);
  sprintf(buffer, "%s<br><h1>Tension thermocouple K</h1><br>", buffer);
  sprintf(buffer, "%sTension : %.1fmV", buffer, tensionmV);//enregistre une chaine dans le tampon de caracteres
  sprintf(buffer, "%s<br><br><br><h1>Temp&eacuterature jonction de r&eacutef&eacuterence<br></h1>", buffer);
  sprintf(buffer, "%sTemp&eacuterature SC : %.1f&degC", buffer, tempSC);
  sprintf(buffer, "%s<table border=\"1\"><tr><td>Maximum</td><td>%.1f&degC</td></tr>", buffer, maxi);
  sprintf(buffer, "%s<tr><td>Minimum</td><td>%.1f&degC</td></tr>", buffer, mini);
  sprintf(buffer, "%s<tr><td>Moyenne</td><td>%.1f&degC</td></tr></table>", buffer, moy);
  strcat(buffer, footer);
  server.send(200, "text/html", buffer);//200->http
}

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(csPin, OUTPUT);
  pinMode(LM335, INPUT);
  digitalWrite(csPin, HIGH);  // Désactive le MCP3301 (slave) 
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", PageWeb);
  server.begin();// on demarre le serveur web 
  Serial.println("HTTP server started");
  delay(100);
}

void loop() {
  //Gestion mesure Soudure Froidre SF
  tempSC = analogRead(LM335)*3.3/1023;//Converti la valeur numerique en tension
  tempSC = tempSC*100-273;//Tension > kelvin > celsius
  Serial.print("SF : ");Serial.print(tempSC);Serial.println(" °C");
  if(tempSC >= maxi){maxi=tempSC;}
  if(tempSC <= mini | i==1){mini=tempSC;}
  acc += tempSC;
  moy = acc / i;
  i++;

  //Gestion mesure Soudure Chaude SC
  digitalWrite(csPin, LOW);  // Active le MCP3301 (slave)
  // Lit les deux octets de données renvoyés par le MCP3301
  highByte = SPI.transfer(0);
  lowByte = SPI.transfer(0);
  digitalWrite(csPin, HIGH); // Désactive le MCP3301
  // Calcule la valeur de tension à partir des octets lus
  adcValue = ((highByte << 8) | lowByte) & 0x0FFF;
  tensionmV = 1000 * adcValue * Vref / 4095;
  // Affiche la valeur de tension sur la console série
  //Serial.print("adcValue : ");Serial.println(adcValue, BIN);
  Serial.print("Tension : ");Serial.print(tensionmV);Serial.println(" mV");  
  
  server.handleClient();// a chaque iteration, la fonction handleClient traite les requetes

  delay(500);
}
