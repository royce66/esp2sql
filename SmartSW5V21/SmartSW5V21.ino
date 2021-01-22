// This program is for ESP-01 work on Hassio by Mqtt
// for Topic : ha/esp/sw/003
// 12/05 add toggle push button

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
#define ssid1    "RSy_home"
#define ssid2    "Royce's_iPhone"
#define password    "330450614510"

#define mqttServer1  "192.168.31.145"
#define mqttServer2  "172.20.10.11"
#define mqttPort  1883

#define SwitchedPin 0 // 1(GND)      3(GPIO2) 5(GPIO0) 7(GPIO3 RX)
#define inPin 3       // 2(GPIO1 TX) 4(CHPD)  6(RST)   8(VCC)
#define blueLED 2     // 2 led type: 1   1 led type: 2
                      
const char* mqttDevice = "ESP01-3";   // ***
const char* mqttUser = "mqtt";
const char* mqttUserPassword = "qqq";
const char* mqttTopic = "ha/esp/sw/003";    // ***
const char* swon = "on";
const char* swoff = "off";

WiFiClient espClient;
PubSubClient client(espClient);
char* mqttServer;
int relayStatus = 0;
String switch1;
String strTopic;
String strPayload;
int i,j = 0;
int val;
bool indLED;

void MnSwDl(unsigned int dt) {
  val = digitalRead(inPin);
//  Serial.println(val);
//  Serial.print("  GIPO2: ");
  if ( val == 0 ) {
//    Serial.print(" : Manual Switched ---> ");
    if (relayStatus == 0) {
//    Serial.println("on");
      digitalWrite(SwitchedPin, HIGH);
      relayStatus = 1;
    } else {
//    Serial.println("off");
      digitalWrite(SwitchedPin, LOW);
      relayStatus = 0;
    }
    delay(1000);
  }
  delay(dt);
}

void setup_wifi() {
  Serial.begin(115200);
  delay(1000);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("SSID scanning! ");
  // scan....
  int n = WiFi.scanNetworks();
  int thessid;
  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i)== ssid1) {
      thessid = i;
//      WiFi.config(IPAddress(192,168,31,102),    // IP位址 ***
//      IPAddress(192,168,31,1),     // 閘道（gateway）位址
//      IPAddress(255,255,255,0));  // 網路遮罩（netmask）
      mqttServer = mqttServer1;
      break;
    }
    if (WiFi.SSID(i)== ssid2) {
      thessid = i;
//      WiFi.config(IPAddress(172,20,10,12),    // IP位址   ***
//      IPAddress(172,20,10,1),     // 閘道（gateway）位址
//      IPAddress(255,255,255,0));  // 網路遮罩（netmask）
      mqttServer = mqttServer2;
      break;
    }
  }
  Serial.println("");
  Serial.print(WiFi.SSID(thessid));
  Serial.println(" connecting !");
  WiFi.begin(WiFi.SSID(thessid),password); //trying to connect the AP
  pinMode(blueLED, OUTPUT);         // rs232 or LED
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);           // wait 1sec
    digitalWrite(blueLED, HIGH);
    MnSwDl(50);
    digitalWrite(blueLED, LOW);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print(WiFi.SSID(thessid));
  Serial.println(" connected !");
  Serial.print("my IP:");
  Serial.println(WiFi.localIP());
  pinMode(blueLED, OUTPUT);       // rs232 or LED
  digitalWrite(blueLED, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == mqttTopic)
    {
    switch1 = String((char*)payload);
//    Serial.print(strTopic);
//    Serial.print(" : ---> ");
    if(switch1 == swon)
      {
//        Serial.println("on");
        digitalWrite(SwitchedPin, HIGH);
        relayStatus = 1;
         digitalWrite(blueLED, LOW);
      }
    else if (switch1 == swoff)
      {
//        Serial.println("off");
        digitalWrite(SwitchedPin, LOW);
        relayStatus = 0;
         digitalWrite(blueLED, HIGH);
      }
    }
}

void reconnect() {
  Serial.print("Attempting MQTT connection...");
  // Loop until we're reconnected
  while (!client.connected()) {
    if (client.connect(mqttDevice, mqttUser, mqttUserPassword)) {
      // Once connected, publish an announcement...
      Serial.println("connected");
      client.publish(mqttTopic, mqttDevice);
      client.subscribe(mqttTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 1 seconds before retrying
    digitalWrite(blueLED, LOW);
    delay(50);
    digitalWrite(blueLED, HIGH);
    MnSwDl(50);
    }
  }
}

void setup()
{
  pinMode(blueLED, OUTPUT);
  digitalWrite(blueLED, LOW);
  setup_wifi(); 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  pinMode(inPin, INPUT);
  pinMode(SwitchedPin, OUTPUT);
  
  digitalWrite(SwitchedPin, HIGH);
  digitalWrite(blueLED, HIGH);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  indLED = relayStatus;
  if (j++ > 100 ) {
    digitalWrite(blueLED, indLED);
    j = 0;
    delay(100);
  } else {
    digitalWrite(blueLED, !indLED);
    MnSwDl(10);
  }
}
