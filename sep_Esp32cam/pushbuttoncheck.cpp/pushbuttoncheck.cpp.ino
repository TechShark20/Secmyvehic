#define pushbutton 12


void setup() {
  Serial.begin(115200);
  pinMode(pushbutton ,INPUT);
  
  // put your setup code here, to run once:

}

void loop() {

 Serial.print(digitalRead(pushbutton));
 Serial.println();
 delay(500);
 
  // put your main code here, to run repeatedly:

}
