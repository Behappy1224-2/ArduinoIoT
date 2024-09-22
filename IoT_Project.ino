#include <LWiFi.h>
#include "DHT.h"
#include <Grove_LED_Bar.h>
#include <Wire.h>
#include <SeeedOLED.h>
#define PIN_AO 16
#define WaterPin 14
#define HOST    "api.thingspeak.com" // ThingSpeak IP Address: 184.106.153.149
#define PORT    80
#define DHTPIN 2     // 溫濕度感測器連接的引腳// what pin we're connected to
#define DHTTYPE DHT22

//wi-fi網路設定
char ssid[] = "Galaxy A53";      //  your network SSID (name)
char pass[] = "";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;
// your network key Index number (needed only for WEP)
float hum;
float temp;

String GET = "GET /update?key=33P2KDMBSGJG3D7A";

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(117,185,24,248);
char server[] = "api.thingspeak.com"; 

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;


DHT __dht2(3, DHT22);
Grove_LED_Bar bar(5, 4, 0);
//土壤 :  16
//水位 : 14
//relay 10


void setup()
{
  Serial.begin(9600);
  __dht2.begin();//初始化溫溼度感測器
  bar.begin();// 初始化 LED bar
  Wire.begin();
  SeeedOled.init();
   while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
    }
    Serial.println("Connected to wifi");

    Serial.println("\nStarting connection to server...");
  SeeedOled.setHorizontalMode();
  pinMode(PIN_AO,INPUT);//設定土壤溼度感測器為輸入
  pinMode(WaterPin,INPUT);//設定水位感測器為輸入
  pinMode(10,OUTPUT);//設定繼電器為輸
}

void loop(){
  SeeedOled.clearDisplay();
  SeeedOled.setTextXY(0, 0);
  SeeedOled.setHorizontalMode();
  float a=__dht2.readTemperature();//溫溼度感測器讀取溫度、濕度數據並顯示於OLED上
  float b= __dht2.readHumidity();

  double water=analogRead(WaterPin);
  double dirt=analogRead(PIN_AO);
  SeeedOled.putString("Current Temperature: ");
  SeeedOled.putFloat(a);
  SeeedOled.setTextXY(2, 0);
  SeeedOled.putString("Current Humidity: ");
  SeeedOled.putFloat(b);

  Serial.print("水位 : ");
  Serial.println(water);
  Serial.print("土壤 : ");
  Serial.println(dirt);
  int Switch;
  if(digitalRead(10)==HIGH){
    Switch=1;
  }else{
    Switch=0;
  }
  if(dirt>3800){//如果土壤溼度大於3800，則土壤過乾
    digitalWrite(10,HIGH);//開啟繼電器
  }else{
    digitalWrite(10,LOW);//若無則關閉
  }
  int breakpoint=0;
  if(digitalRead(10)==HIGH){
    if(Switch==0){
      breakpoint=2;
    }
    Serial.println("Relay High");
  }else{
    if(Switch==1){
      breakpoint=1;
    }
    Serial.println("Relay Low");
  }
  int i=water/400;水位每上升400則LEDBar亮一格
  if(i>10){
    i=10;
  }
  if (client.connect(server, 80)) {
        Serial.println("connected to server (GET)")
        int num=0;
        if(digitalRead(10)==HIGH){
          num=1;
        }
        String getStr = GET + "&field1=" + String(a) +
                              "&field2=" + String(b) +
                              "&field3=" + String(water) +
                              "&field4=" + String(dirt) +
                              "&field5=" + String(num) +
                              "&field6=" + String(breakpoint) +
                              " HTTP/1.1\r\n";;
        client.print( getStr );
        client.print( "Host: api.thingspeak.com\n" );
        client.print( "Connection: close\r\n\r\n" );       
//        delay(1000);
    
       // if there are incoming bytes available
       // from the server, read them and print them:
          while (client.available()) {
              char c = client.read();
              Serial.write(c);
        }
      }
    // if the server's disconnected, stop the client:
        if (!client.connected()) {
            Serial.println();
            Serial.println("disconnecting from server.");
            client.stop();

            // do nothing forevermore:
             // while (true);
        }
  
  bar.setLevel(i);
  Serial.println();
  delay(16000);
}
