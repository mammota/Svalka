#include <Arduino.h>

#include <WiFi.h>
#include <Wire.h>
#include <DHT.h> 
#include <Adafruit_BMP085.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TEMP_WIRE_BUS 22
#define DHTPIN 15 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;

OneWire tempWire(TEMP_WIRE_BUS);           //объявили переменную датчиков
DallasTemperature TempSensors(&tempWire);  //объявили структуру типа Даллас термометр


char pressure_value[4];
const char* wifi_name = "Tenda_31BC98"; //Your Wifi name
const char* wifi_pass = "barcelona";    //Your Wifi password
WiFiServer server(80);                  //Port 80

void setup()
{
  Serial.begin(115200);
  dht.begin(); 
  bmp.begin();
  
  // Let's connect to wifi network 
  Serial.print("Connecting to ");
  Serial.print(wifi_name);
  WiFi.begin(wifi_name, wifi_pass);   //Connecting to wifi network

 while (WiFi.status() != WL_CONNECTED) //Waiting for the responce of wifi network
 {
    delay(500);
    Serial.print(".");
 }
    Serial.println("");
    Serial.println("Connection Successful");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());   //Getting the IP address at which our webserver will be created
    Serial.println("Type the above IP address into a browser search bar");
    server.begin();                   //Starting the server
}

void loop()
{
 
  String pressure = String(bmp.readPressure());
  // convert the reading to a char array
  pressure.toCharArray(pressure_value, 4);
  
  float hum = dht.readHumidity(); 
  float temp = dht.readTemperature(); 
  float fah = dht.readTemperature(true);
  float heat_index = dht.computeHeatIndex(fah, hum);
  float heat_indexC = dht.convertFtoC(heat_index);

  WiFiClient client = server.available();   //Checking for incoming clients

  if (client) 
  {                             
    Serial.println("new client");          
    String currentLine = "";                   //Storing the incoming data in the string
    while (client.connected()) 
    {            
      if (client.available())                  //if there is some client data available
      {                
        char c = client.read();                // read a byte
          if (c == '\n')                       // check for newline character, 
          {                     
          if (currentLine.length() == 0)      //if line is blank it means its the end of the client HTTP request
          {     
            client.print("<html><title> ESP32 Weather Station</title></html>");
            client.print("<body bgcolor=\"#E6E6FA\"><h1 style=\"text-align: center; color: blue\"> ESP32 Weather Station </h1>");
            client.print("<p style=\"text-align: center; font-size:150% \">Temperature in C: ");
            client.print(temp);
            client.print("<br/>Temperature in fah: ");
            client.print(fah);
            client.print("<br/>Humidity is: ");
            client.print(hum);
            client.print("<br/>Heat Index in C: ");
            client.print(heat_indexC);
            client.print("<br/>Heat Index in fah: ");
            client.print(heat_index);
            client.print("<br/>Pressure is: ");
            client.print(pressure_value);
            client.print("hpa");
            client.print("</p></body>");
        
            break;  // break out of the while loop:
          } 
          else 
          {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
         } 
         else if (c != '\r') 
         {  // if you got anything else but a carriage return character,
          currentLine += c;       // add it to the end of the currentLine
         }
        }
      }
    }
  }
  //lib_extra_dirs = C:\Users\Александр\Documents\Arduino\libraries\OneWire
  //lib_deps = https://github.com/adafruit/Adafruit_NeoPixel