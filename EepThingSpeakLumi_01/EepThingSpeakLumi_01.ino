#include <ESP8266WiFi.h>
// try use NodeMCU board to impliment
//*-- IoT Information
#define ssid    "RSy_home"
#define ssid2    "Royce's iPhone"
#define password    "330450614510"
#define HOST    "api.thingspeak.com" // ThingSpeak IP Address: 184.106.153.149
#define PORT 80
// 使用 GET 傳送資料的格式
// GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&filed2=[data 2]...;
String ghead = "GET /update?key=UJ4MLH0NMJY3MCNA";
#define TIME_UPLOAD 99        // per 3min upload
#define COUNT_UL 10           // reset to 1 if count big than 10
byte count = TIME_UPLOAD;
byte cnt1 = 0;
bool onf = false;
const int BUTTON_PIN = 4;    // Define pin the button is connected to
const int ON_BOARD_LED = 2;  // Define pin the on-board LED is connected to
const int RGB_B_PIN = 12;    // RGB Blue LED
const int RGB_G_PIN = 13;    // RGB Green LED
const int RGB_R_PIN = 15;    // RGB Red LED
const int LDR_PIN = A0;      // Define the analog pin the LDR is connected to

int lumi0, lumi1;

WiFiServer server(80);

void setup() {
  pinMode(ON_BOARD_LED, OUTPUT);       // Initialize the LED_BUILTIN pin as an output
  //pinMode(RGB_G_PIN, OUTPUT);
  //pinMode(RGB_B_PIN, OUTPUT);
  //pinMode(RGB_R_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Initialize button pin with built-in pullup.
  digitalWrite(ON_BOARD_LED, HIGH);    // Ensure LED is off
  analogWrite(RGB_G_PIN, 200);
  analogWrite(RGB_B_PIN, 200);
  analogWrite(RGB_R_PIN, 200);
  Serial.begin(115200);
  delay(2000);              // wait 2sec

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("SSID scanning! ");
  // scan....
  int n = WiFi.scanNetworks();
  int thessid;
  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i)== ssid ) {
      WiFi.begin(ssid,password); //trying to connect the modem
      thessid = i;
      break;
    }
    if (WiFi.SSID(i)== ssid2) {
      WiFi.begin(ssid2,password); //trying to connect the modem
      thessid = i;
      break;
    }
  }
  Serial.println("");
  Serial.print(WiFi.SSID(thessid));
  Serial.println(" connecting !");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);           // wait 1sec
    Serial.print(".");
  }
  Serial.println("");
  Serial.print(WiFi.SSID(thessid));
  Serial.println(" connected !");
  analogWrite(RGB_G_PIN, 0);
  analogWrite(RGB_B_PIN, 200);
  analogWrite(RGB_R_PIN, 0);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
  delay(1000);           // wait 1sec
}

void loop() 
{
  int thld = 500;
  lumi1 = analogRead(LDR_PIN);  // Read the light intensity
  if ( lumi1 > thld)             // 500 threadhold of luminance
  {
    analogWrite(RGB_G_PIN, 0);
    analogWrite(RGB_B_PIN, 30);
    analogWrite(RGB_R_PIN, 0);
    if (lumi0 < thld)
    {
      cnt1 +=1;
      if ( cnt1 > COUNT_UL) { cnt1=1;}
      count = 0;
      Upload2ThingSpeak(lumi1,cnt1);
      Serial.println("upper edge trigger");
    }
  }
  else
  {
    analogWrite(RGB_G_PIN, 0);
    analogWrite(RGB_B_PIN, 0);
    analogWrite(RGB_R_PIN, 30);
    if (lumi0 > thld)
    {
      count = 0;
      Upload2ThingSpeak(lumi1,cnt1);
      Serial.println("lower edge trigger");
    }
  }
  String ttl1 = "ESP8266 Luminance Detecter & Upload to SingSpeak";
  String sub1 = "by Royce 2019/08/01";
  HTTPpub(ttl1, sub1);
  delay(5000);                  // wait 5sec
  lumi0 = lumi1;                // svae last value to lumi0
  if( count == TIME_UPLOAD )
  {
    count = 0;
    Upload2ThingSpeak(lumi1,cnt1);
  }
  else
  {
    Serial.print( "count=");
    Serial.print( count +=1 );
    Serial.print( "  lumi1=");
    Serial.print( lumi1 );
    Serial.println();
  }
} //end of main loop

void Upload2ThingSpeak(float field1,float field2)
{
    // 設定 ESP8266 作為 Client 端
    WiFiClient client;
    if( !client.connect( HOST, PORT ) )
    {
        Serial.println( "connection failed" );
        return;
    }
    else
    {
        // 準備上傳到 ThingSpeak IoT Server 的資料
        // 已經預先設定好 ThingSpeak IoT Channel 的欄位
        // field1：Luminance；field2：counts
        // GET https://api.thingspeak.com/update?api_key=UJ4MLH0NMJY3MCNA&field1=0
        String getStr = ghead + "&field1=" + String(field1) + "&field2=" + String(field2); // + " HTTP/1.1\r\n";
        //Serial.println(getStr);
        client.print( getStr );
        client.print( "Host: api.thingspeak.com\n" );
        client.print( "Connection: close\r\n\r\n" );
        delay(500);               // wait 0.5sec
        while(client.available()){
          String line = client.readStringUntil('\r');
          Serial.print(line);
        }   // 處理遠端伺服器回傳的訊息，程式碼可以寫在這裡！
        Serial.println();
        client.stop();
        Serial.println(" Upload Thinkspek success!!");
    }
}

void HTTPpub(String ttl1, String sub1)
{
  WiFiClient client = server.available();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Refresh: 60");  // refresh the page automatically every 5 sec
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("<p style='text-align: center;'>&nbsp;</p>");
  client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>");
  client.print(ttl1);
  client.print("</strong></span></p>");
  client.print("<p style='text-align: center;'><span style='font-size: x-large;'><strong>");
  client.print(sub1);
  client.print("</strong></span></p>");
  client.print("<p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: large;'>Luminance = ");
  client.println(lumi1);
  client.print("</strong></span></p>");
  client.print("<p style='text-align: center;'>&nbsp;</p>");
  client.print("<p style='text-align: center;'>&nbsp;</p>");
  client.print("<p style='text-align: center;'>&nbsp;");
  client.print("</p>");
  client.println("</html>");
}
