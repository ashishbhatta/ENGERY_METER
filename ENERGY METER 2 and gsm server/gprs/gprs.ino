void setup() {
  Serial.begin(9600);
  Serial.print("AT+CGATT=1\r"); 
  Serial.print("AT+CSTT=\"smart\"\r"); 
  Serial.print("AT+CIICR\r");
  Serial.print("AT+CIFSR\r");
  Serial.print("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"\r");
  Serial.print("AT+CIPSEND");
  Serial.print("api.thingspeak.com/update?api_key=HOBVRIDFU480R0HM&field1=2GET https://api.thingspeak.com/update?api_key=HOBVRIDFU480R0HM&field1=2\r");
  Serial.print("0x1A\r");
  Serial.print("AT+CIPSHUT\r");
  
 

}

void loop() {
  // put your main code here, to run repeatedly:

}
