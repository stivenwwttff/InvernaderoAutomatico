
#include "DHT.h"
#include <ESP32Servo.h> // Incluimos la biblioteca del Servo



#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

//Definimos pines led de emergencia
#define ledVerde 5
#define ledRojo 19
#define ledAmarillo 18

//definimos luz led
#define luzled 22

//definimos sensor de luiz
#define sensorluz 23

//definimos pin servomotor
#define servomotor 21

//definimos sensor humumedad suelo
int humTierra = 32;

//definimos bocina
#define bocina 13

//definimos ventilador
int ventilador = 12;

//definimos bomba de agua
#define bomba 14



// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);

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

//configuramos la bocina 
   pinMode(bocina, OUTPUT);   

//configuramos el sensor de humedad tierra
  pinMode(humTierra, INPUT);     
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  int sensorValue = map(analogRead(humTierra), 4090, 0, 0, 100);
  
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
    digitalWrite(bomba, HIGH);
    Serial.println("bomba Encendida"); 
    //delay(1000); 
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
  }else{
      digitalWrite(bomba, LOW);
      Serial.println("bomba apagado");
      //delay(1000);            
  }
 
 
  
}