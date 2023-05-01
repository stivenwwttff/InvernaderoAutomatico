#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"
#include <ESP32Servo.h> // Incluimos la biblioteca del 


// Digital pin connected to the DHT sensor
#define DHTPIN 4     


//Definimos pines led de emergencia
#define ledVerde 5
#define ledRojo 19
#define ledAmarillo 18

//definimos luz led
int luzled = 2;

//definimos sensor de luiz
int sensorluz = 34;


//definimos pin servomotor
#define servomotor 21

//definimos sensor humumedad suelo
int humTierra = 32;

//definimos bocina
#define bocina 13

//definimos ventilador
int ventilador = 12;

//definimos bomba de agua
#define bomba 22


// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

//creamos las funciones para telegram
// Wifi network station credentials
#define WIFI_SSID "JHE292114"//nombre de la red
#define WIFI_PASSWORD "210363_11" //contrasena del wifi
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "5652956699:AAHNGGIvFja2w7mZ1xogQE9rIed0JDlu5RE"
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")

    from_name = "Guest";
    if (text == "/ledon")
    {
    
    bot.sendMessage(chat_id, "Led is ON", "");
    }
    if (text == "/ledoff")
    {
    
    bot.sendMessage(chat_id, "Led is OFF", "");
    }
    //condiciones para  temp, hum, y temp F
    if (text == "/temp")
    {
    float t = dht.readTemperature();
    char cadena[50];
    sprintf(cadena,"La temperatura es %0.2f", t);
    bot.sendMessage(chat_id, cadena);
    }
    if (text == "/hum")
    {
    float h = dht.readHumidity();
    char cadena[50];
    sprintf(cadena,"La humedad del aire es %0.2f", h);
    bot.sendMessage(chat_id, cadena);
    }

    if (text == "/humT")
    {
    int sensorValue = map(analogRead(humTierra), 4090, 0, 0, 100);
    char cadena[50];
    sprintf(cadena,"La humedad de la tierra es %0.2f", sensorValue);
    bot.sendMessage(chat_id, cadena);
    }
    //fin de condiciones
    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Flash Led Bot example.\n\n";
      welcome += "/temp : Ver temperatura Temp\n";
      welcome += "/hum : Ver humedad Hum\n";
      welcome += "/humT : Ver humedad de tierra HumT\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}
//----------------------------------

// Creamos un objeto Servo
Servo myservo;

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();

   // Adjuntamos el objeto myservo al pin de salida
  myservo.attach(servomotor);

  // Configurar el pin del relé como salida
  pinMode(ventilador, OUTPUT); 

 // Configurar el pin del rele como salida
   pinMode(bomba, OUTPUT); 
   
//configuramos los led de emergencia
   pinMode(ledVerde, OUTPUT);
   pinMode(ledRojo, OUTPUT);
   pinMode(ledAmarillo, OUTPUT);
    pinMode(luzled, OUTPUT);
//configuramos la bocina 
   pinMode(bocina, OUTPUT);   

//configuramos el sensor de humedad tierra
  pinMode(humTierra, INPUT);     

//telegram
// attempt to connect to Wifi network:
Serial.print("Connecting to Wifi SSID ");
Serial.print(WIFI_SSID);
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
while (WiFi.status() != WL_CONNECTED)
{
Serial.print(".");
delay(500);
}
Serial.print("\nWiFi connected. IP address: ");
Serial.println(WiFi.localIP());
Serial.print("Retrieving time: ");
configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
time_t now = time(nullptr);
while (now < 24 * 3600)
{
Serial.print(".");
delay(100);
now = time(nullptr);
}
Serial.println(now);
}

void loop(){
  // Wait a few seconds between measurements.
  delay(2000);

  //telegram
  if (millis() - bot_lasttime > BOT_MTBS)
  {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages)
  {
  Serial.println("got response");
  handleNewMessages(numNewMessages);
  numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  bot_lasttime = millis();
  }

  //almacenamos valor del sensor y convertimos a porcentaje
  int sensorValue = map(analogRead(humTierra), 4090, 0, 0, 100);

  //almacenamos valor del sensor de luz
  // variable para almacenar el valor de la fotocelda
  int celdaValue = analogRead(sensorluz);
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

 

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("%  Temperature tierra: "));
  Serial.print(sensorValue);
  Serial.print(F("%"));
  Serial.print(F("%  luz: "));
  Serial.print(celdaValue);
  
  
  
  
  //condicion para temperatura
  if (t>30){
      // Encender el relé durante 1 segundo
      digitalWrite(ventilador, HIGH);
      Serial.println("Relé encendido ventilador"); 
      //delay(1000);
      
      //encendemos el led rojo
      digitalWrite(ledRojo, HIGH);
      digitalWrite(bocina, HIGH);                   
      
  }else{
      if(t>20 & t<30){
        //encendemos led amarillo
        digitalWrite(ventilador, LOW);
         digitalWrite(ledAmarillo, HIGH);
         digitalWrite(ledRojo, LOW);
         digitalWrite(ledVerde, LOW);      
                         
      }else{
      // Apagar el relé durante 1 segundo
            digitalWrite(ventilador, LOW);
            Serial.println("Relé apagado ventilador");
            //delay(1000);      
            digitalWrite(ledVerde, HIGH);
            digitalWrite(ledRojo, LOW);
            digitalWrite(ledAmarillo, LOW);         
      }           
  }
  
  //condicional para humedad 
    if (humTierra<50){
         
        //servomotor
        for (int pos = 0; pos <= 180; pos++) {
          myservo.write(pos); // Escribimos la posición en el servomotor
          delay(15); // Esperamos 15 milisegundos para que se mueva
        }
  
        // Giramos el servomotor de 180 a 0 grados en incrementos de 1 grado
        for (int pos = 180; pos >= 0; pos--) {
          myservo.write(pos); // Escribimos la posición en el servomotor
          delay(15); // Esperamos 15 milisegundos para que se mueva
        }  
        //BOMBA DE AGUA    
        digitalWrite(bomba, HIGH);
        Serial.println("bomba Encendida"); 
                    
    }else{
        digitalWrite(bomba, LOW);
      Serial.println("bomba apagado");
      //delay(1000);            
  }
 
  //condicion para fotocelda
   if(celdaValue>2000){
      digitalWrite(luzled, HIGH);
    }else{
       digitalWrite(luzled, LOW); 
    }
  
}

