/* Create a WiFi access point and provide a web server on it.
Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.*/   
#include <ESP8266WiFi.h>
#include <SPI.h>

#define Vref 0.1 // tension de reference du MCP3301 en V

const char* ssid     = "Galaxy";
const char* password = "D4+1EU7R";
const int csPin = D8;  // Chip Select MCP3301
const int LM335 = A0;

// IP address of the server you want to connect to
IPAddress serverIP(192, 168, 154, 156);
const uint16_t serverPort = 1234;

WiFiClient client;//creation d une instance de WiFiClient

int i=0, adcValue=0;
float tempSC=0, maxi=0, tensionmV=0, mini=0, moy=0, acc=0;

byte highByte=0x00, lowByte=0x00;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(csPin, OUTPUT);
  pinMode(LM335, INPUT);
  digitalWrite(csPin, HIGH);  // Désactive le MCP3301 (slave) 
  WiFi.begin(ssid, password);

  Serial.print("\nConnecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to : ");
  Serial.println(ssid);
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to server...");
  delay(100);
}

void loop() {
  if (!client.connected()) {
    Serial.print(".");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("\nConnected to server");
    }
  }

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
  
  String data = String(tensionmV) + "," + String(tempSC);
  client.print(data);
  delay(1000);
}
