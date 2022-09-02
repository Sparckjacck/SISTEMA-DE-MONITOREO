// SE INCLUYE LIBRERIAS DE SENSORES
#include "ph_grav.h" //Libreria de ph
#include "do_grav.h"    //libreria de DO
#include <OneWire.h>    // libreria de sensor de Temperatura             
#include <DallasTemperature.h> // libreria de sensor de Temperatura  
#include  <SoftwareSerial.h>    //libreria de XBee
#include <Ticker.h> 
#include <ArduinoJson.h>
// SE DEFINE PINES A CADA UNO DE LOS SENSORES
Gravity_pH pH = A0; //ph en pin A0
Gravity_DO DO = A1; // DO en pin A1
#define sensortemp A2 // Temperatura en ping A2
SoftwareSerial XBee(11, 10); // Pines XBee RX, TX
OneWire oneWire(sensortemp);// configuracione para Cx dispositivos OneWire
DallasTemperature sensors(&oneWire); //Se pasa a la referencia de Dallas

char charFloatTE [6];
char charFloatPH [6];
char charFloatDO [6];
String strFloatTE;  
String strFloatPH;
String strFloatDO;


uint8_t datos_recibidos = 0;
const uint8_t data =32 ;
char datos_usuario [data];


// CALIBRACIÓN DE SENSORES
void calibracion (char*string){
  strupr(string); 
  String serialentrada = String(string);                             
  if (strcmp(string, "CAL,7") == 0) {       
    pH.cal_mid();                                
    Serial.println("CALIBRACIÓN DE PH MEDIA");
  }
  else if (strcmp(string, "CAL,4") == 0) {            
    pH.cal_low();                                
    Serial.println("CALIBRACION DE PH BAJA");
  }
  else if (strcmp(string, "CAL,10") == 0) {      
    pH.cal_high();                               
    Serial.println("CALIBRACION DE PH ALTA");
  }
  else if (strcmp(string, "CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("CALIBRACION BORRADA");
  }
  else if(serialentrada.startsWith("CAL,DO,CLEAR")){
    DO.cal_clear();
    Serial.println("CALIBRACIÓN BORRADA");
  }
  else if(serialentrada.startsWith("CAL,DO")){
    DO.cal();
    Serial.println("SENSOR DO CALIBRADO");
  }
}



void setup() {
   Serial.begin(9600);
   sensors.begin();
   XBee.begin(9600);

  if (pH.begin()) {                                     
    Serial.println("DATO DE PH CARGADO A EEPROM");
  }
  if(DO.begin()){
    Serial.println("DATO DE DO CARGADO A EEPROM");
  }
  Serial.println();
  
}

void temp (){
sensors.requestTemperatures();       //Prepara el sensor para la lectura
  }


void loop() {
  
if (Serial.available() > 0) {
    datos_recibidos = Serial.readBytesUntil(13, datos_usuario, sizeof(datos_usuario));
  }
  if (datos_recibidos) {
      calibracion(datos_usuario);
      datos_recibidos = 0;
      memset(datos_usuario, 0, sizeof(datos_usuario));
  }
  temp ();
  enviar ();
  vector ();
}

String convertFloatToString( float temperature){
    char chartemp[10];
    String tempAsString;
    dtostrf(temperature,1,2,chartemp);// Proceso de conversion
    tempAsString = String(chartemp);  // se crea el objeto string
    return tempAsString;// reinizializa la varible
}
String convertPHFloatToString( float pH){
    char charpH[10];
    String pHAsString;
    dtostrf(pH,1,2,charpH);
    pHAsString = String(charpH);
    return pHAsString;
}
String convertDOFloatToString( float DO){
    char charDO[10];
    String DOAsString;
    dtostrf(DO,1,2,charDO);
    DOAsString = String(charDO);
    return DOAsString;
}
  void vector (){
                //string to float TE
    float valTE = sensors.getTempCByIndex(0);// se obtine el dato del sensor
    strFloatTE=convertFloatToString(valTE);// se obtiene el valor de conversion float a string
    //Serial.print(strFloatTE);
    strFloatTE.toCharArray(charFloatTE,6);      //se guarda el valor en una variable char
  
                //string to float PH
 float valPH = pH.read_ph();
   strFloatPH=convertPHFloatToString(valPH);
    //Serial.print(strFloatPH);
    strFloatPH.toCharArray(charFloatPH,6);

                //string to float DO
   float valDO = DO.read_do_percentage();
    strFloatDO=convertDOFloatToString(valDO);
    //Serial.print(strFloatDO);
    strFloatDO.toCharArray(charFloatDO,6);

String json_dataTE = "{\"S\":\"NrA\",\"T\":"+(String)charFloatTE+",\"P\":"+(String)charFloatPH+",\"O\":"+(String)charFloatDO+"}";
//String json_dataTE = "{\"SB\":\"NrB\",\"TB\":"+(String)charFloatTE+",\"PB\":"+(String)charFloatPH+",\"OB\":"+(String)charFloatDO+"}";
   XBee.print(json_dataTE);
   Serial.println(json_dataTE);
   delay(60000); 

    
  }
void enviar (){
  
 /*Serial.print("pH: ");
 Serial.println(pH.read_ph());
 Serial.print("DO: ");
 Serial.println(DO.read_do_percentage());
 Serial.print("Temp: ");
 Serial.println(sensors.getTempCByIndex(0)); //Se lee e imprime la temperatura en grados Centigrados
 delay(1000);
*/
}

  
