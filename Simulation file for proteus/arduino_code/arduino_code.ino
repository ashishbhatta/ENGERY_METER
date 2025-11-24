
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
long startmils = millis();
int i;
int analogpin[6]={54,55,56,57,58,59};
float v_cali[3] = {1.372315,1.372315,1.372315};
float c_cali[3] = {0.026394,0.026394,0.026394}; 
float impoted[3];
float exported[3];
float c[3];
long v[3];
long p[3];
float S[3];
float c_rms[3];
float v_rms[3];
float p_rms[3];
float pf[3];
int count;
void readdata()
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
 
    /*Serial.println(temp);
    Serial.println(temp2);
    Serial.println(c[i]);
    Serial.println(v[i]);
    Serial.println(p[i]);*/
    delayMicroseconds(400); 
    } 
  }
}
void calculate()
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
     Serial.println("i");
     Serial.println(c_rms[i]);
     Serial.println(v_rms[i]);
     Serial.println(p_rms[i]);
     Serial.println(S[i]);
     Serial.println(pf[i]);
    }
  }
  void energy()
  {
    for(i=0; i<=2; i++)
    {
      if(p_rms[i]>=0)
      {
        Serial.print("time");
        Serial.println(millis() - startmils);
        impoted[i] += p_rms[i]*(millis() - startmils)/1000000;  //unit kWs
        Serial.print("impoted ");
        Serial.println(impoted[i]);
     }
      else
      {
        exported[i] = -1*p_rms[i]*(millis() - startmils);
      }

    startmils = millis();
  }
} 

 void printcurrent()
 {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CURRENT AMP");
    for(i=0;i<3;i++)
    {
    lcd.setCursor(0,i+1);
    lcd.print("PHASE ");
    lcd.print(i+1);
    lcd.print(": ");
    lcd.print(c_rms[i]);
    }
 }
void printvoltage()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("VOLTAGE VOLT");
    for(i=0;i<3;i++)
    {
    lcd.setCursor(0,i+1);
    lcd.print("PHASE ");
    lcd.print(i+1);
    lcd.print(": ");
    lcd.print(v_rms[i]);
    }
}   

void printpower()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("POWER WATT");
    for(i=0;i<3;i++)
    {
    lcd.setCursor(0,i+1);
    lcd.print("PHASE ");
    lcd.print(i+1);
    lcd.print(": ");
    lcd.print(p_rms[i]);
    }
}
void setup() {
Serial.begin(9600);
lcd.begin(16,4);
lcd.print("SMART METER");
}
void loop() {
  readdata();
  calculate();
  energy();
  printcurrent();
  
  readdata();
  calculate();
  energy();
  printvoltage();
  
  readdata();
  calculate();
  energy();
  printpower();
 
}
