
#include <SoftwareSerial.h>

#define RELAY  2
#define SENSOR A0

/* Commands: 
 *   status  --> To read the sendor data
 *   relay:1 --> Turn on the relay
 *   relay:0 --> Turn off the relay
 */

SoftwareSerial dbgSerial(10, 11); // RX, TX
//Rx of Ard(10) --- Tx of EsP
//Tx of Ard(11) --- Rx of EsP

char str[50];
int relayStatus=0;

void setup()
{
  // Open serial communications and wait for port to open:
  dbgSerial.begin(9600);
  dbgSerial.setTimeout(5000);

  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  pinMode(RELAY,OUTPUT);
}
void loop()
{  
   int analogValue;
   
    if(getString(str)>0)                      /*Check for recived Command, Function will return non zero number if command is received */
    {
        if(strncmp(str,"status",6)==0)        
        {
          Serial.write("Sensor Data:");      /* Send the Sensor data connected to A0 */
          analogValue = analogRead(SENSOR);
          Serial.println(analogValue); 
          
          dbgSerial.write("Sensor Data:");
          analogValue = analogRead(SENSOR);
          dbgSerial.println(analogValue);  
        }
        else if(strncmp(str,"relay",5)==0)
        {
          if(str[6] == '0')                  /* Turn Off the relay as requested */
          {
             relayStatus = 0;
             digitalWrite(RELAY,0);
             Serial.write("Relay Turned Off\r\n");
          }
          else if(str[6] == '1')            /* Turn On the relay as requested */
          {
             relayStatus = 1;
             digitalWrite(RELAY,1);
             Serial.write("Relay Turned On\r\n");
          }
          else
          {
            Serial.write("Relay Status:");
            Serial.println(relayStatus);  
          }
        }
    }
}


/* Read Command from ESP */
unsigned int getString(char *ptr)
{
   unsigned int i=0;
   char ch;

   while(1)
   {
      if(dbgSerial.available())
       {
          ch = dbgSerial.read();   /* Read a char from ESP */
      
          if(ch !=-1)
            {
                Serial.write(ch);   /* Send the Reecived char to Serial Port. Used for debugging */
     
                if((ch == '\n') || (ch == '\r'))
                  {
                    ptr[i] = 0;     /* Received EOL(string), break the loop and service the request*/
                    break;
                  }
                else if((ch == '\b') && (i))
                 {
                     i--;
                 }
                else
                 {
                    ptr[i++] = ch;  /* Copy the data received from ESP to buffer */
                  }
             }
       }
   }
   return i;
}
