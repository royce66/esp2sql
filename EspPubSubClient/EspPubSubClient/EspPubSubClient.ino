#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <dht11.h>
#define DHT11PIN 5  // DHT11資料接腳

const char* ssid = "你的Wi-Fi網路名稱";
const char* password = "你的Wi-Fi網路密碼";
const char* mqttServer = "mqtt.thingspeak.com";  // MQTT伺服器位址
const char* mqttUserName = "cubie";  // 使用者名稱，隨意設定。
const char* mqttPwd = "你的MQTT API Key";  // MQTT密碼
const char* clientID = "yard001";      // 用戶端ID，隨意設定。
const char* topic = "channels/頻道ID/publish/你的寫入API KEY";

unsigned long prevMillis = 0;  // 暫存經過時間（毫秒）
const long interval = 20000;  // 上傳資料的間隔時間，20秒。
String msgStr = "";      // 暫存MQTT訊息字串

int temp;  // 暫存溫度
int hum;   // 暫存濕度

WiFiClient espClient;
PubSubClient client(espClient);
dht11 DHT11;

void setup_wifi() {
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID, mqttUserName, mqttPwd)) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // 等5秒之後再重試
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqttServer, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 等待20秒
  if (millis() - prevMillis > interval) {
    prevMillis = millis();

    // 讀取DHT11的溫濕度資料
    int chk = DHT11.read(DHT11PIN);
    if (chk == 0) {
      temp = DHT11.temperature;
      hum = DHT11.humidity;
    }

    // 組合MQTT訊息；field1填入溫度、field2填入濕度
    msgStr=msgStr+"field1="+temp+"&field2="+hum;
    
    // 宣告字元陣列
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];

    Serial.print("Publish message: ");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize); // 把String字串轉換成字元陣列格式
    client.publish(topic, msg);       // 發布MQTT主題與訊息
    msgStr = "";
  }
}
