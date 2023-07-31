/* Ejemplo de uso LoRaWAN del módulo RYLR993_lite con Cayenne

 *@archivo  : RYLR993-Cayenne.ino
 * @autor   : Biblioman09
 * @version : v.1.0
 * @fecha   : 31-Julio, 2023
 * @link    : https://www.youtube.com/@Biblioman09
 * Licencia : GPLv3			   

*/

#include <DHT22.h>
#include <CayenneLPP.h>

//define pin data
#define data 13 //SDA

#define loraSerial Serial2 //Nota: pines Serial1 utilizada por memoria flash. Serial2: Rx-> 16  Tx->17
#define debugSerial Serial

//Declaración de variables globales necesarias para unir el módulo a la red
unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 9000; // Tiempo de espera entre solicitudes en milisegundos
uint8_t conexion=0;

DHT22 dht22(data); 
CayenneLPP lpp(51);


void setup() {
  debugSerial.begin(9600); //1bit=10µs
  //Esperar un máximo de 10s para serial monitor
  while(!debugSerial && millis()<10000);

  loraSerial.begin(9600,SERIAL_8N1,16,17);
   while(!loraSerial && millis()<10000); 

   joinNetwork();
         
  }


void loop() {
  //debugSerial.println(" ----> LOOP");

  String Envio="";
  lpp.reset();
  
  //Lectura de la temperatura
  float t = dht22.getTemperature();
  //debugSerial.print("Temperatura= ");debugSerial.println(t);
  
  lpp.addTemperature(1,t);
  
 //Lectura de la humedad
 float h = dht22.getHumidity();
 //debugSerial.print("Humedad= ");debugSerial.println(h);
 lpp.addRelativeHumidity(2,h);

 delay(5000);
//Inicio de la cadena a eviar canal 2 sin ACK
Envio= "AT+SEND=2:0:";

//Declaro un puntero de tipo uint8_t para obtener la dirección del buffer
uint8_t *payload_lpp =lpp.getBuffer();

//añado el payload de la medida de los sensores obtenida por la API de Cayenne  a la cadena a enviar convirtiendo los datos previamente a formato HEX 
 for (unsigned char i = 0; i < lpp.getSize(); i++)
  {
    Envio+= toHexString(payload_lpp[i]);
    
  }


//Envío la cadena por el puerto loraSerial
if (conexion==1){
loraSerial.println(Envio);
//delay(3000);

//Envío a DEBUG
//debugSerial.println("Conexión OK..");
//debugSerial.println(Envio);
//debugSerial.print("Nº de Bytes: ");debugSerial.println(lpp.getSize());

// Espero 7s hata la próxima medida
delay(20000);

}

}


//................Definición de funciones..............................

//Función para unir el módulo a la red

void joinNetwork() {
      while (true) {
    if (millis() - lastRequestTime >= requestInterval) {
      // Enviar la cadena "AT+JOIN=1" por la UART2 (Serial2)
      loraSerial.println("AT+JOIN=1");
      lastRequestTime = millis(); // Actualizar el tiempo de la última solicitud
      
    }

    // Leer la respuesta recibida, línea por línea
    while (loraSerial.available()) {
      String response = loraSerial.readStringUntil('\r');
      
      // Verificar si la respuesta contiene la palabra "+EVT:JOINED"
      if (response.indexOf("+EVT:JOINED") != -1) {
        // Si la palabra se encuentra, salir del bucle infinito
        conexion=1;
        return;
      }
    }
  }
}  
 

//Función para convertir enteros a formato hexadecimal

String toHexString(uint8_t num) {
  char buffer[8]; // El buffer debe tener espacio para dos dígitos hexadecimales más el caracter nulo ('\0')
  sprintf(buffer, "%02X", num); // %02X indica que queremos dos dígitos en formato hexadecimal
  return String(buffer);
}






    
    
