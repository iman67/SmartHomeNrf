#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "Node1";
void setup() {
pinMode(6, OUTPUT);
Serial.begin(9600);
radio.begin();
radio.openReadingPipe(0, address);   //Setting the address at which we will receive the data
radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
radio.startListening();              //This sets the module as receiver
}
void loop()
{
if (radio.available())              //Looking for the data.
{
char text[7] = ""; //Saving the incoming data
char state[5] = "";
radio.read(&text, sizeof(text));    //Reading the data
//Serial.print(text[5]);
//radio.read(&button_state, sizeof(button_state));    //Reading the data
//Serial.print(button_state);

if(text[0]=='2' && text[5]=='H')
{
digitalWrite(6, HIGH);
}
else if(text[0]=='2' && text[5]==' ')
{
digitalWrite(6, LOW);
}
}
delay(5);
}
