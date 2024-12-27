
1. select the "Board >> Genuino Mega" in Tools menu above 
2. connect the sensor as follows
  54 current ph 1
  55 voltage ph 1
  56 current ph 2
  57 voltage ph 2
  58 current ph 3
  59 voltage ph 3
3. Make sure all the required libary are already added to Arduino IDE (and in proteus for simunation purpose only)
connect Lcd as mension below
*/
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

DS3231  rtc(SDA, SCL);// Init the DS3231 using the hardware interface
Time  t;              // Init a Time-data structure
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
long startmils = millis();
int i;
int analogpin[6]={54,55,56,57,58,59};
float v_cali[3] = {1.372315,1.372315,1.372315};
float c_cali[3] = {0.01381069458,0.01381069458,0.01381069458}; 
float impoted[3]  = {0.0 , 0.0 , 0.0};
float exported[3] = {0.0 , 0.0 , 0.0};
float c[3];
long v[3];
long p[3];
float S[3];
float c_rms[3];
float v_rms[3];
float p_rms[3];
float pf[3];
float net_exported = 0.0;
float net_imported = 0.0;
int count;
String dataString = "";
String temporary = "";
const int chipSelect = 53;

byte uparrow[8] = {
 0b00000,
 0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b11111,
  0b00000
};
byte downarrow[8] = {
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte temp[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b00000
};
byte degree[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
}; 
byte gsm[8] = {
0b11111,
  0b10001,
  0b10001,
  0b10000,
  0b10111,
  0b10001,
  0b11111,
  0b00000 
};



///////////////////////////////////////////////////////////////////////////////////////////////////
/*setup*/     /*setup*/     /*setup*/     /*setup*/
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
Serial.begin(9600);
rtc.begin();
sd();
lcd.createChar(0, uparrow);
lcd.createChar(1, downarrow);
lcd.createChar(2, temp);
lcd.createChar(3, degree);
lcd.createChar(4, gsm);
lcd.begin(16,4);
lcd.print("--EnERgY-MeTEr--");
lcd.setCursor(0,1);
lcd.write((uint8_t)2);
Serial.print(rtc.getTemp());
Serial.print(" C");
lcd.write((uint8_t)3);
lcd.print("C");
Serial.println(" _ _   __ ___  __  __                            ");
Serial.println("--------| | | |__  |  |__ |__|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
Serial.println("--------| | | |__  |  |__ |  \\----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*MAin Function*/     /*MAin Function*/     /*MAin Function*/     /*MAin Function*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  for (int j=0 ; j<=2 ; j++)
  {
  squareandsum();      //find sum of squares for rms calculation
  meanroot();          //find mean and then performs squareroot     
  printcurrent(j);     //print calculated values to LCD
  squareandsum();      //function is repeated to create delay for LCD
  meanroot();
  printvoltage(j);
  squareandsum();
  meanroot();
  printpower(j);
  squareandsum();
  meanroot();
  energy();            //calculates energy in kWs
  t = rtc.getTime();   // Get data from the DS3231
  
  /*save data to Sd when logical and condition is fullfilled*/
  
  if (t.min % 2 == 0  &&  t.sec >= 42)  
   {
    Serial.println("____________________________________________________________________________________________________________________________________________");
    energy();
    datalog();
    Serial.println("_____________________________________________________________________________________________________________________________________________");
   }  
}
printnetimported();
printnetexported();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
        /*end of main function*/             /*end of main function*/                  /*end of main function*/
////////////////////////////////////////////////////////////////////////////////////////////////////


/* This function find sum of squares needed for rms and power calculation */
void squareandsum()    
{
  for(i=0;i<=2;i++)
  {
     
    for(count=0;count<2000;count++)
    { 
  
    long temp = analogRead(analogpin[2*i]) - 512L;
    long temp2 = analogRead(analogpin[2*i+1]) - 512L;
    c[i] += sq(temp);
    v[i] += sq(temp2);
    p[i] += temp * temp2 * v_cali[i] * c_cali[i];
 
    delayMicroseconds(400); 
    } 
  }
}

/*function calculates the all the parameters*/
void meanroot()
  {
    //Serial.println(p[1]);
    for(i=0;i<=2;i++)
    {
     c_rms[i] = sqrt((c[i]/2000.0))*c_cali[i];
     v_rms[i] = sqrt((v[i]/2000.0)) * v_cali[i]; 
     p_rms[i] = p[i]/2000.0;
     p[i] = 0;
     v[i] = 0;
     c[i] = 0;
     S[i] = c_rms[i] * v_rms[i];
     pf[i] = p_rms[i] / S[i] ;
     //power in Watt-sec
    // Ws[i] +=3* p_rms[i];
     Serial.print("for phase");
     Serial.println(i+1);
     Serial.print("current      ");
     Serial.println(c_rms[i]);
     Serial.print("Voltage      ");
     Serial.println(v_rms[i]);
     Serial.print("Active Power ");
     Serial.println(p_rms[i]);
     Serial.print("Appara Power ");
     Serial.println(S[i]);
     Serial.print("Power Factor ");
     Serial.println(pf[i]);
     Serial.println("--------------------------");
    }
  }

  /*    This function calutates energy in kWs unit */
  void energy()
  {
    Serial.println("--------------------------");
    for(i=0; i<=2; i++)
    {
      if(p_rms[i]>=0)
      {
        //Serial.print("time");
        //Serial.println(millis() - startmils);
        impoted[i] += p_rms[i]*(millis() - startmils)/1000000;  //unit kWs
        Serial.print("impoted ");
        Serial.print(i +1 );
        Serial.print(" ");
        Serial.println(impoted[i]);
     }
      else
      {
        Serial.print("exported ");
        Serial.print(i );
        Serial.print(" ");
        exported[i] = -1*p_rms[i]*(millis() - startmils);
        Serial.println(exported[i] );
      }
  }
   startmils = millis();
} 

/* Printing current through 16x2 LCD */

 void printcurrent(int x)
 {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("---PHASE \"");
    lcd.print(x+1);
    lcd.print("\"---");
    lcd.setCursor(0,1);
    lcd.print("AMPS ");
    lcd.print(c_rms[x]);
    
 }

 /* Printing voltage through 16x2 LCD */
void printvoltage(int x)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("---PHASE \"");
    lcd.print(x+1);
    lcd.print("\"---");
    lcd.setCursor(0,1);
    lcd.print("VOLT ");
    lcd.print(v_rms[x]);
}  

/* Printing power through 16x2 LCD */
void printpower(int x)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("---PHASE \"");
    lcd.print(x+1);
    lcd.print("\"---");
    lcd.setCursor(0,1);
    lcd.print("WATT ");
    lcd.print(p_rms[x]);
}

/* LOGING kWs To SD card*/

void datalog()
{  
    dataString = rtc.getTimeStr();
    dataString += ",";
    for (int i = 0; i < 3; i++) 
     {
      temporary = String(impoted[i]);
      while(temporary.length() < 8)
       {
         temporary += "0";
       }
      dataString += temporary;
      dataString += ",";
      temporary = String(exported[i]); 
      while(temporary.length() < 8)
       {
         temporary += "0";
       }
      
       dataString += temporary;
        if(i < 2)
        {
          dataString += ",";
        }
     }
    
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) 
    {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println("Displaying the valve written to datalog.csv :");
      Serial.println(dataString);
    }         
}

/* Printing importedpower through 16x2 LCD */

void printnetimported()
{
  for(i = 0; i <= 2 ;i++)
  {
    net_imported += impoted[i];
  }
    lcd.clear();
    lcd.setCursor(0,0);
    
    lcd.write((uint8_t)1);   
    lcd.print(net_imported * 2.7778e-4);
    lcd.print(" kWh");
}

/* Printing exportedpower through 16x2 LCD */
void printnetexported()
{
  for(i = 0; i <= 2 ;i++)
  {
    net_exported += exported[i];
  }
 
    lcd.setCursor(0,1);

    lcd.write((uint8_t)0);
    lcd.print(net_exported * 2.7778e-4);
    lcd.print(" kWh");
}

/* Initialize the SD card and fetch previous data from it */

void sd()
{
  Serial.println("Initializing SD card...");
if (!SD.begin(chipSelect)) 
  {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
  i = 0;
  File dataFile = SD.open("datalog.csv");
  Serial.println("fetching data from datalog.csv");
  if (dataFile) 
  {
    // read from the file until there's nothing else in it:
     dataFile.seek(dataFile.size() - 55);   
     while (dataFile.available()) {
     impoted[i] = (dataFile.readStringUntil(',')).toFloat();
     Serial.print("impoted ");
     Serial.print(i);
     Serial.println(": ");
     Serial.println(impoted[i]);
     exported[i] =  dataFile.readStringUntil(',').toFloat();
     Serial.print("exported ");
     Serial.print(i);
     Serial.println(": ");
     Serial.println(exported[i]);
      i++;  
    }
    Serial.println("last data read sucess, closing datalog.csv");
    dataFile.close();
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.csv");
    Serial.println("creating new file datalog.csv and setting all data zero");
    dataString  = rtc.getTimeStr();
    dataString += ",";
    Serial.println(dataString);
    dataString += "0.000000,0.000000,0.000000,0.000000,0.000000,0.000000";
    Serial.println(dataString);
    File dataFile = SD.open("datalog.csv", FILE_WRITE);
    // if the file is available, write to it:
    if (dataFile) 
    {
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      Serial.println("All values set to zero");
      Serial.println(dataString);
    }
  }
}
