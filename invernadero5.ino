#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"
#include <ESP32Servo.h> // Incluimos la biblioteca del 


//Definimos pin del DHT sensor
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
extern const int bomba = 22;

String chatId = "5463562694"; // ID del chat de destino

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

//creamos un objeto de la clase dht
DHT dht(DHTPIN, DHTTYPE);

//creamos las funciones para telegram
// Wifi network station credentials
#define WIFI_SSID "Galaxy_A22"//nombre de la red
#define WIFI_PASSWORD "viviana24" //contrasena del wifi
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
    float sensorValue = map(analogRead(humTierra), 4090, 0, 0, 100);
    char cadena[50];
    sprintf(cadena,"La humedad de la tierra es %0.2f", sensorValue);
    bot.sendMessage(chat_id, cadena);
    }
    //fin de condiciones
    if (text == "/start")
    {
      String welcome = "Bienvenido a nuestro sistema de invernadero inteligente.\n";
      welcome += "selecciona una de las opciones.\n\n";
      welcome += "/temp : Ver temperatura Temp\n";
      welcome += "/hum : Ver humedad Hum\n";
      welcome += "/humT : Ver humedad de tierra HumT\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

//----------------------------------
//WEB SERVER
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


//---------------------------------
// Creamos un objeto Servo
Servo myservo;

void setup() {
  Serial.begin(115200);
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

  //verificacion de conexion
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
//----------------------------------

//WEB SERVER
//veriicacion de conexion
  // Connect to Wi-Fi network with SSID and password
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
//----------------------------
}

void loop(){
  // Wait a few seconds between measurements.
  delay(2000);

//para realizar el chat automatico  
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
  //------------------------------------------------------------------------------
  
  //almacenamos valor del sensor y convertimos a porcentaje
  int sensorValue = map(analogRead(humTierra), 4090, 0, 0, 100);

  //almacenamos valor del sensor de luz
  // variable para almacenar el valor de la fotocelda
  int celdaValue = analogRead(sensorluz);
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //leer humedad
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  //leer temperatuea
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //leer temp F
  float f = dht.readTemperature(true);

  //funcion para verificar errores
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

 
  //para pruebas de ejecucion 
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
      digitalWrite(ledAmarillo, LOW); 
      digitalWrite(bocina, HIGH);
      delay(1000);     

        // Enviar mensaje a un chat telegram
        
        // Mensaje a enviar
        char cadena[50];
        sprintf(cadena,"¡LA TEMPERATURA ESTA EN UN NIVEL ALTO! %0.2f", t);
        if (bot.sendMessage(chatId,cadena)) {
          Serial.println("Mensaje enviado exitosamente");
        } else {
          Serial.println("Error al enviar el mensaje");
        }

        delay(5000); // Esperar 5 segundos antes de enviar otro mensaje              
      
  }else{
      if(t>20 & t<30){
        //encendemos led amarillo
         digitalWrite(ledAmarillo, HIGH);
         digitalWrite(ventilador, LOW);
         digitalWrite(ledRojo, LOW);
         digitalWrite(ledVerde, LOW);      
         digitalWrite(bocina, LOW);             
      }else{
      // Apagar el relé durante 1 segundo
            digitalWrite(ventilador, LOW);
            Serial.println("Relé apagado ventilador");
            //delay(1000);      
            digitalWrite(ledVerde, HIGH);
            digitalWrite(ledRojo, LOW);
            digitalWrite(ledAmarillo, LOW);  
            digitalWrite(bocina, LOW);         
      }           
  }
  
  //condicional para humedad 
    if (sensorValue<=0){
         
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

        // Enviar mensaje a un chat
        
         // Mensaje a enviar
        char cadena[50];
        sprintf(cadena,"¡LA HUMEDAD ESTA EN UN PORCENTAJE BAJO! %0.2f", h);
        if (bot.sendMessage(chatId,cadena)) {
          Serial.println("Mensaje enviado exitosamente");
        } else {
          Serial.println("Error al enviar el mensaje");
        }

        //delay(5000); // Esperar 5 segundos antes de enviar otro mensaje 
                    
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

  //WEB SERVER
    WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              
            }
            //
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 iNVERNADERO iNTELIGENTE</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>verificar temperatura C</p>");
            
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ver temp</button></a></p>");
            } else {
              //client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
                String temString = String(t);                
                client.println("<p>temperatura C"+temString+"</p>");              
            } 
               
            client.println("<p>verificar humedad %</p>");      
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ver hum</button></a></p>");
            } else {
              //client.println("<p><a href=\"/humedad\"><button class=\"button button2\">OFF</button></a></p>");
              String humString = String(h);                
              client.println("<p>humedad "+humString+"</p>");
            }              
                             
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  //-------------------------------------------------------

}
