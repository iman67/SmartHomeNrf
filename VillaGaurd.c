#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


RF24 radio(10, 53); // CE, CSN   

byte address[6] = "Node1";


LiquidCrystal_I2C lcd(0x3F,16,2);

const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

char pass[4]={'X', 'X', 'X', 'X'};
char enter[4];
byte rowPins[ROWS] = {5, 4, 3, 2}; 
byte colPins[COLS] = {8, 9, 7, 6}; 
int i=0, s=0, attempt=3, auth_counter=0;
bool auth = false, showmenu = false, entermenu = false, d = false , sleep = true;
unsigned long myTime;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  ConnectRadio();
  Start();
}
  
void loop(){
  char customKey = customKeypad.getKey();
  if(customKey && i<4 && !auth && sleep==true){
    sleep=false;
    LoginPage();
  }
  else if(customKey && i<4 && !auth && sleep==false){
     debounce();
     lcd.print('*');
     if(customKey==pass[i])  auth_counter++;
     i++;
   }
   
   else if(i>3 && !auth){
    delay(500);
    ConnectRadio();
    if(auth_counter==4)   loggedin();
    else{
      failed();      
      ConnectRadio();
      ResetSystem();
    }
    i=0;
   }
   
   if(attempt==0 && !auth)
   {
      BuzzAlarm();
      ConnectRadio();
      attempt=3;
      ResetSystem();
   }

   if(auth && showmenu==false && entermenu==false){
    if(i==0){
      firstpage();
      showmenu=true;
    }
    else if(i==1){
      secondpage();
      showmenu=true;
    }
    else if(i==2){
      thirdpage();
      showmenu=true;
    }
    else if(i==3){
      fourthpage();
      showmenu=true;
    }
   }

   else if(customKey && auth && showmenu==true){
    debounce();
    if(customKey=='A' && entermenu==false){
      if(i>0)  i--;
      else i=3;

      if(i==0 && entermenu==false)        firstpage(); 
      else if(i==1 && entermenu==false)   secondpage();
      else if(i==2 && entermenu==false)   thirdpage();
      else if(i==3 && entermenu==false)   fourthpage();
     }
     
    else if(customKey=='B' && entermenu==false){
      if(i<3) i++;
      else  i=0;
      
      if(i==0 && entermenu==false)        firstpage();
      else if(i==1 && entermenu==false)   secondpage();
      else if(i==2 && entermenu==false)   thirdpage();
      else if(i==3 && entermenu==false)   fourthpage();
    }

    else if(entermenu==true && customKey=='C'){
      entermenu=false;
      showmenu=false;
      d=false;
      lcd.clear();
    }
      
    else if(customKey=='D'){
      if(i==0){
        lock();
        ConnectRadio();
        attempt=3;
        Start();
    }
        
    else if(i==1 && entermenu==false && d==false)   nrf1on();
    else if(i==1 && entermenu==true && d==true)     nrf1off();
    else if(i==1 && entermenu==true && d==false)    nrf1on();
    
    else if(i==2 && entermenu==false && d==false)   nrf2on();
    else if(i==2 && entermenu==true && d==true)     nrf2off();
    else if(i==2 && entermenu==true && d==false)    nrf2on(); 
        
    else if(i==3 && entermenu==false && d==false)   AllON();
    else if(i==3 && entermenu==true && d==true)     AllOFF();
    else if(i==3 && entermenu==true && d==false)    AllON();
    
 }
     
     
}
  
}



// Functions
void ConnectRadio(){
  radio.begin();                  //Starting the Wireless communication
  radio.openWritingPipe(address); //Setting the address where we will send the data
  radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
  radio.stopListening();          //This sets the module as transmitter
}

void Start(){
  auth=false;
  auth_counter=0;
  i=0;
  showmenu=false;
  entermenu=false;
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.print("  Villa  Guard  ");
  lcd.setCursor(0,1);
  lcd.print(" Kasra     Iman ");
}

void LoginPage(){
  lcd.clear();
  lcd.print("Enter Passcode:"); 
  lcd.setCursor(0,1);
}

void debounce(){
  myTime = 0;
  myTime = millis();
  if(myTime==250) return;
}

void ResetSystem(){
  auth=false;
  auth_counter=0;
  i=0;
  showmenu=false;
  entermenu=false;
  lcd.clear();
  lcd.print("Enter Passcode:"); 
  lcd.setCursor(0,1);
}

void loggedin(){
  lcd.clear();
  lcd.print("    Disarmed!   ");
  lcd.setCursor(0,1);
  lcd.print("Welcome to Villa");
  auth=true;
  attempt=3;
  i=0;
  delay(1500);
  lcd.clear();
}

void failed(){
  lcd.clear();
  lcd.print("Wrong Passcode");
  lcd.setCursor(0,1);
  attempt--;
  lcd.print(attempt);
  lcd.print(" attempts left");
  delay(1500);
}

void BuzzAlarm(){
  lcd.clear();
  lcd.print("Buzzing Alarm");
  delay(1500);
}

void firstpage(){
  lcd.clear();
  lcd.print("0) Lock  <=");
  lcd.setCursor(0,1);
  lcd.print("1) NRF1");
}

void secondpage(){
  lcd.clear();
  lcd.print("0) Lock");
  lcd.setCursor(0,1);
  lcd.print("1) NRF1  <=");
}

void thirdpage(){
  lcd.clear();
  lcd.print("2) NRF2  <=");
  lcd.setCursor(0,1);
  lcd.print("3) ALL");
}

void fourthpage(){
  lcd.clear();
  lcd.print("2) NRF2");
  lcd.setCursor(0,1);
  lcd.print("3) ALL  <=");  
}


void lock(){
  lcd.clear();
  lcd.print("  Villa Locked  ");
  sleep=true;
  delay(2000);
}

void nrf1on(){
  lcd.clear();
  lcd.print("NRF1 Turned ON");
  entermenu=true;
  d=true;
  const char text[] = "1 HIGH\n";
  radio.write(&text, sizeof(text));
}

void nrf1off(){
  lcd.clear();
  lcd.print("NRF1 Turned OFF");
  entermenu=true;
  d=false;
  const char text[] = "1 LOW \n";
  radio.write(&text, sizeof(text));
}

void nrf2on(){
  lcd.clear();
  lcd.print("NRF2 Turned ON");
  entermenu=true;
  d=true;
  const char text2[] = "2 HIGH\n";
  radio.write(&text2, sizeof(text2));
}

void nrf2off(){
  lcd.clear();
  lcd.print("NRF2 Turned OFF");
  entermenu=true;
  d=false;
  const char text2[] = "2 LOW \n";
  radio.write(&text2, sizeof(text2));
}

void AllON(){
  lcd.clear();
  lcd.print("ALL Turned ON");
  entermenu=true;
  d=true;
  const char text3[] = "1 HIGH\n";
  radio.write(&text3, sizeof(text3));
  delay(100);
  ConnectRadio();
  const char text4[] = "2 HIGH\n";
  radio.write(&text4, sizeof(text4));
}

void AllOFF(){
  lcd.clear();
  lcd.print("ALL Turned OFF");
  entermenu=true;
  d=false;
  const char text3[] = "1 LOW \n";
  radio.write(&text3, sizeof(text3));
  delay(100);
  ConnectRadio();
  const char text4[] = "2 LOW \n";
  radio.write(&text4, sizeof(text4));
}
