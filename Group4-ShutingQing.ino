#include <Servo.h>
//------ wifi
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "Miranda"
#define STAPSK  "96115jrci470x"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
int n=1;

//------ wifi end


int motorL1=4; //left
int motorL2=5;
int motorR1=0; //right
int motorR2=15;
int pwmA = 13;
int pwmB = 2;
int PullUP = 3;
int speed = 400;
int agale = 350;//time for delay when trun left in obstacle avoidance

Servo s; //motor server, which used to turning the sensor
int trig=12; //signal out pin
int echo=14; //signal recieve pin
int S; //save the distance

void setup() {
    Serial.begin(115200); 
    pinMode(trig,OUTPUT); 
    pinMode(echo,INPUT);
    pinMode(motorL1,OUTPUT);
    pinMode(motorL2,OUTPUT);
    pinMode(motorR1,OUTPUT);
    pinMode(motorR2,OUTPUT);
    pinMode(PullUP,OUTPUT);
    digitalWrite(PullUP,HIGH);
    s.attach(16); 
    s.write(80); //initiallize the server
    tone(12,800,500);
    delay(2000);
    
    Serial.begin(115200);
    
    //------ wifi
    
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //------ wifi end
    
    
}


void line(){
    digitalWrite(motorR1,HIGH); //go stright on
    digitalWrite(motorL1,HIGH);
    digitalWrite(motorR2,LOW);
    digitalWrite(motorL2,LOW);
    analogWrite(pwmA,590);//right
    analogWrite(pwmB,615);//left
}

void L(){
   
    digitalWrite(PullUP,HIGH);//turn left
    digitalWrite(motorL1,LOW);
    digitalWrite(motorL2,HIGH);
    digitalWrite(motorR1,HIGH);
    digitalWrite(motorR2,LOW);
    analogWrite(pwmA,speed);
    analogWrite(pwmB,speed);

}

void R(){
    digitalWrite(PullUP,HIGH);//turn right
    digitalWrite(motorL1,HIGH);
    digitalWrite(motorL2,LOW);
    digitalWrite(motorR1,LOW);
    digitalWrite(motorR2,HIGH);
    analogWrite(pwmA,600);
    analogWrite(pwmB,400);
    
}


void back(){ 
    digitalWrite(PullUP,HIGH);//go back
    digitalWrite(motorL1,LOW);
    digitalWrite(motorR1,LOW);
    digitalWrite(motorR2,HIGH);
    digitalWrite(motorL2,HIGH);
    analogWrite(pwmA,650);
    analogWrite(pwmB,605);
    
}

void lull(){
    digitalWrite(motorL1,LOW);//stop
    digitalWrite(motorL2,LOW);
    digitalWrite(motorR1,LOW);
    digitalWrite(motorR2,LOW);
}


void range(){//detect the obstacles with sensor
    digitalWrite(trig,LOW); //signal out
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(20);
    digitalWrite(trig,LOW);
    int distance = pulseIn(echo,HIGH); 
    distance = distance/58; //caculating the distance
    S = distance; //
    Serial.println(S); //print to the monitor
    //if (S<30){
    //tone(12,800,50);
    //delay(50); //延时
    //}
}

void turn(){ //to decide if we should turning and which direction should to turn
    lull();
    s.write(150); //detect the left
    delay(400); //server running
    range(); 
    delay(1000);
    s.write(60); //finished detect, turn back the server
    delay(400); 
    if (S>20)//there are enough room in left and we should turn left
    {
        L();
        delay(agale);//delay time will determind how much it will change its direction
    } 
    else if(S<=20)//no enough room for turning left, so we detect the right
    {
        s.write(0); //server turn to right
        delay(400);
        range(); 
        s.write(60); 
        delay(400);
        if(S>20)
        {
            R();
            delay(agale);
        }
    }
    else //no enough space for either turnning right or left 
    {
        back();
        delay(700);//back for a while so that we can have more space
        int x=random(1); //got a random number to avoiding dead loop
        if (x=0){R();delay(agale);}
        else
        {L();delay(agale);} 
    } 
}

int Median(int a[],int N) // get the median of several consistant signal values
{
    int i,j,m;
    int t;
    for(i=0;i<N-1;i++)
    {
        m=i;
        for(j=i+1;j<N;j++)
        if(a[j]>a[m]) m=j;
        t=a[i];a[i]=a[m];a[m]=t;
    }
    return a[(N-1)/2];
 } 


float get_Signal() // get 5 consistant signal values
{
    int test=0;
    int test_signal[5];
    for(int i=0;i<=4;i++)//test 5 times to get the average value and decreasing mistake
    {
        test_signal[i]=WiFi.RSSI();
        delay(100);
    }
    test=Median(test_signal,5); //get the median 
    return test;
}


void go(int direction) // turn to a direction
{
    if(direction==1) //go straight ahead
    {
        line();
        delay(agale);
    }
    else if(direction==2)// 45 degree clockwise
    {
        R();
        delay(290);//need to test
        lull();
        delay(550);
    }
    else if(direction==3)// 90 degree clockwise
    {
        R();
        delay(580);//need to test
        lull();
        delay(550);
    }
    else if(direction==4)// 135 degree clockwise
    {
        R();
        delay(870);//need to test
        lull();
        delay(550);        
    }
    else if(direction==5)// 180 degree clockwise
    {
        R();
        delay(1150);
        lull();
        delay(550);
    }
    else if(direction==6) // 225 degree clockwise
    {
        L();
        delay(795);//need to test
        lull();
        delay(550);        
    }
    else if(direction==7) // 270 degree clockwise
    {
        L();
        delay(530);//need to test
        lull();
        delay(500);
        
    }
    else if(direction==8) // 315 degree clockwise
    {
        L();
        delay(265);//need to test
        lull();
        delay(550);        
    }
}


int xh1=-300, xh2=-300, xh3=-300;//xh1,xh2,xh3 are three most recently signal value, xh3 is the latest value
int flag_stop = 0;//find the beacon and stop
int startflag=3;//mark the xh1,xh2,xh3 are still in their initial value


void loop() { 
    
    digitalWrite(PullUP,HIGH);
    s.write(60);
    range(); //get the distance S
    if(S<10){ //if too close
        back(); //back up
        delay(100); 
    }
    if(S<=30&&S>10){//proper distance
        turn(); //call the turnning function so that we can avoid the obstacle
    }
    if(S>30)
    { //space enough， go ahead and detecting the wifi signal
  
        if(flag_stop) // find the beacon and stop
        {
          lull();
        }
        else // finding beacon
        {
        line(); // move on since the last loop (in the last loop, it just turn to a new direction, but didn't go ahead, since it needs to make sure there is no obstacle in that direction)
        delay(800);
        //lull();

        //upadate the latest 3 signal values and remove noise peaks
        if(xh2>xh1+2) 
        {
          xh1=xh1+2;//remove the noise peak
        }
        else
        {
          xh1=xh2;
          }
        if(startflag!=0) // judge whether it's the initializing value
        {
          xh1=xh2;
          startflag--;
          }
        xh2=xh3;
        xh3=get_Signal();//xh1,xh2,xh3 is three most recently signal value
        Serial.printf("xh1:%d, xh2:%d,xh3:%d\n", xh1, xh2,xh3);

        //find the beacon and stop
        if(xh3>-57)
         {
          lull();
          flag_stop=1;
          }
        else if(xh3<xh1 && xh2<xh1) // if the wifi signal is decreasing, then turn left
        {
            Serial.printf("left");

            xh2=-300;
            startflag=2;
            go(7);//turn left
            lull();
        }
        
        }    
     }
    
}
