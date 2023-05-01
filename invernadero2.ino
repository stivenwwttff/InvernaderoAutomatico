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
}

void loop(){
  // Wait a few seconds between measurements.
  delay(2000);
  
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

