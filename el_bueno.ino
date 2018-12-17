#include <SoftwareSerial.h>
#include <String.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define PMTK_SET_NMEA_OUTPUT "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"// "$PMTK220, 10000*2F" //Once every 10 s, 100 mhz
 
const int RX = 4;
const int TX = 3;
const int ledPin = 13;

 int byteGPS=-1;
 char linea[300] = "";
 char comandoGPR[7] = "$GPRMC";
 int cont=0;
 int bien=0;
 int conta=0;
 int indices[13];
 byte trama[8];
 char lat[9];
 char lon[9];


SoftwareSerial gps(RX, TX);
 
void setup()
{
  trama[0] = 0x11;
  trama[1] = 0x09;
  
  //115200
   Serial.begin(115200);
  
     gps.begin(9600);
     gps.println(PMTK_SET_NMEA_OUTPUT);
   for (int i=0;i<300;i++){       // Initialize a buffer for received data
     linea[i]=' ';
   }
}
 
void loop()
{
   if (gps.available()>0)
   {
      byteGPS = gps.read();
   if (byteGPS == -1) {           // See if the port is empty yet
     delay(100); 
   } else {
     // note: there is a potential buffer overflow here!
     linea[conta]=byteGPS;        // If there is serial port data, it is put in the buffer
     conta++;                      
     if (byteGPS==13){            // If the received byte is = to 13, end of transmission
       // note: the actual end of transmission is <CR><LF> (i.e. 0x13 0x10)
       digitalWrite(ledPin, LOW); 
       cont=0;
       bien=0;
       // The following for loop starts at 1, because this code is clowny and the first byte is the <LF> (0x10) from the previous transmission.
       for (int i=1;i<7;i++){     // Verifies if the received command starts with $GPR
         if (linea[i]==comandoGPR[i-1]){
           bien++;
         }
       }
       if(bien==6){               // If yes, continue and process the data
         for (int i=0;i<300;i++){
           if (linea[i]==','){    // check for the position of the  "," separator
             // note: again, there is a potential buffer overflow here!
             indices[cont]=i;
             cont++;
           }
           if (linea[i]=='*'){    // ... and the "*"
             indices[12]=i;
             cont++;
           }
         }
         Serial.println("");      // ... and write to the serial port
         Serial.println("");
         Serial.println("---------------");
         for (int i=0;i<12;i++){
           switch(i){
             case 0 :Serial.print("Time in UTC (HhMmSs): ");break;
             case 1 :Serial.print("Status (A=OK,V=KO): ");break;
             case 2 :Serial.print("Latitude: ");break;
             case 3 :Serial.print("Direction (N/S): ");break;
             case 4 :Serial.print("Longitude: ");break;
             case 5 :Serial.print("Direction (E/W): ");break;
             case 6 :Serial.print("Velocity in knots: ");break;
             case 7 :Serial.print("Heading in degrees: ");break;
             case 8 :Serial.print("Date UTC (DdMmAa): ");break;
             case 9 :Serial.print("Magnetic degrees: ");break;
             case 10 :Serial.print("(E/W): ");break;
             case 11 :Serial.print("Mode: ");break;
             case 12 :Serial.print("Checksum: ");break;
           }
           int truelat;
           int truelon;
           int velocity;
           for (int j=indices[i];j<(indices[i+1]-1);j++){
             if (i == 2){
             lat[j - indices[2]] = linea[j+1];
             Serial.print(parsearLatitud(lat));
             truelat = parsearLatitud(lat);
             }
             if (i == 3){
              if (linea[j+1] == 'S'){
              truelat = truelat*-1;  
              Serial.print(linea[j+1]); 
                } 
             }
             if (i == 4){
             lon[j - indices[4]] = linea[j+1];
             Serial.print(parsearLongitud(lon));
             }
             if (i == 6){
             velocity = atoi(&linea[j+1]);
             Serial.print(velocity);
             }
             if (i == 10){
              if (linea[j+1] == 'W'){
              truelon = truelon*-1;  
              Serial.print(linea[j+1]); 
                } 
             }
             else {
             Serial.print(linea[j+1]); 
             }
           }
           truelat = 40.45;
           truelon = 3.73;
           velocity = 20;
           char* buff;
           char* buff1;
           char* buff2;
           //LongToHex(truelat,buff);
           //LongToHex(truelon,buff1);
           //LongToHex(velocity,buff2);
           
           trama[2] = buff;
           trama[3] = buff1;
           trama[4] = buff2;
           Serial.println("");
         }
         Serial.println("-----TRAMA-----");
         Serial.println(trama[0]);
         Serial.println(trama[1]);
         Serial.println(trama[2]);
         Serial.println(trama[3]);
         Serial.println(trama[4]);
         Serial.println("-----TRAMA-----");
         Serial.println("---------------");
       }
       conta=0;                    // Reset the buffer                 
     }
   }
 }
}
double parsearLatitud(const char *latitud)
{
       unsigned char dd[2];
       unsigned char mm[7];
       double decimal1, decimal2;

       double coordenada, latitudParseada;
       int grados;
       double minutos;

       coordenada = atof(latitud);
       grados = (int) floor(coordenada / 100);
       minutos = fmod(coordenada, 100);
       minutos = minutos / 60;
       latitudParseada = grados + minutos;

       //decimal2 = (float) decimal2 / 60;

       //decimal1 = (float) decimal1 + decimal2;

       //return decimal1;
       return latitudParseada;
}

float parsearLongitud(const char *longitud)
{
       double longitudParseada;
       unsigned char dd[3]; // = "";
       unsigned char mm[7]; // = "";
       int p;
       double coordenada;
       int grados;
       double minutos;

       double decimal1, decimal2;

       coordenada = atof(longitud);
       grados = (int) floor(coordenada / 100);
       minutos = fmod(coordenada, 100);
       minutos = minutos / 60;
       longitudParseada = grados + minutos;

       //decimal2 = (float) decimal2 / 60;

       //decimal1 = (float) decimal1 + decimal2;

       //return decimal1;
       return longitudParseada;
}

//400KHZ 
//0,25W

void LongToHex(long int number, unsigned char *res)
{
    long quotient, remainder;
    int m, n;
    char tem;
    short SIGNO = 0, PRIMERO = 1;
    int i, j = 0;
    memset(res, 48, 6);
    if (number < 0)
    {
       SIGNO = 1;
       number = number * -1;
    }
    quotient = number;
    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (SIGNO == 1)
        {
           remainder = 0xF - remainder;
           if (PRIMERO == 1)
           {
              remainder = remainder + 1; // Terminamos el complemento a dos sumando un 1 al primer resto, que es la última cifra realmente.
              PRIMERO = 0;
           }
        }
        if (remainder < 10)
            res[j++] = 48 + remainder;
        else
            res[j++] = 55 + remainder;
        quotient = quotient / 16;
    }
    // invertir cadena
    m = 0;
    n = 6;
    while(n > m)
    {
        tem = res[m];
        res[m++] = res[--n];
        res[n] = tem;
    }
    // Ahora que la cadena ya está montada, cambiamos los 0 iniciales por F si hay signo, esto es parte del complemento a 2 ya que
    // F - 0 = F.
    // Recorremos la cadena de 0 hasta encontrar el primer número mayor que cero y ahí paramos.
    if (SIGNO == 1)
    {
       n = 0;
       do
       {
         tem = res[n];
         if (tem == 48)
         {
            res[n] = 'f';
         }
         n++;
       } while ( (n < 6) && (tem == 48) );
    }
}
