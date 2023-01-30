 #include <movingAvg.h>
 #include <PID_v1.h>
 #include <Wire.h>

  //I2C
#define SCL 5
#define SDA 4

  //directions
#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3

  //motions
#define GO 0
#define LEFT 1
#define RIGHT 2
#define STOP 3
#define BACK 4

  //Run Type
#define DEST 0
#define HOME 1
#define SPEED 2

  //Motors
#define EN_L 11   //motor driver enable left
#define EN_R 10   //motor driver enable right

#define MR_A1 8   //motor direction right
#define MR_A2 13  //motor direction right
#define ML_A1 9   //motor direction left
#define ML_A2 12  //motor direction left

 //encoder
#define ENC_R 3   //encoder right
#define ENC_L 2   //encoder left

  //LED
#define LED_L_FR 4
#define LED_L_DIAG 5
#define LED_R_FR 7
#define LED_R_DIAG 6

  //photo transisters
#define PT_R_FR A3
#define PT_R_DIAG A2

#define PT_L_FR A0
#define PT_L_DIAG A1

  //LED states
#define OFF HIGH
#define ON LOW

#define delay_time 100
#define square 520

byte startX=0, startY=0;
byte startHeading = 0;

  //Gyro
const int MPU = 0x68; // MPU6050 I2C address
double gyroInp , gyroOut, gyroSet;
float GyroX = 0, GyroY = 0, GyroZ =0;
float AccX = 0, AccY = 0, AccZ = 0;
float yaw=0;
float elapsedTime, currentTime, previousTime;

  //robot memory data
byte maze[98]; //array of walls
byte dist[196]; //array of floodfill values
byte visited[25]; //records visited cells
byte posX=0, posY=0; //current position of the robot
byte heading = EAST; //heading of the robot
byte go = DEST; //Determines if the robot is going to the destination or back to the starting point
boolean finished = false; //becomes true when destination is reached
byte minNeighbour = 100; //value of the minimum neighbour

byte wallLeft, wallRight, wallFront; //Current wall readings

  //movement data
byte nxtMove = GO; //next Move
byte currMove = STOP; //current Move
boolean checked = false; //if wall check has been executed
boolean halt = false; //robot stops moving
byte preDist = 0;  //distance travelled prior to discovering initial position

	//queue variables
byte queueCount = 0;
byte queueLimit = 200;
byte queue[200];
byte queued[25];  //true if cell has been added to queue

byte spd = 100; //SPEED

volatile int count_r,count_l = 0; //encoder Counts

  //PID parameters
double stopSet, stopInp, stopOut; //wall stop PID parameters
double trackSet , trackInp, trackOut; //track PID parameters
double encLSet, encLInp, encLOut; //encoder Left PID parameters
double encRSet, encRInp, encROut; //encoder Right PID parameters
int encCount, prevEncCount; //encoder Count parameters

int loopCount = 0; //counts the number of loop()

  //Sensor parameters
double sensorSum; //sum of front IR readings linearized
int L_offset=240; //midval for left
int R_offset=240; //midval for right
byte wallThresh=80;  //wall Threshold
int ir_l_diag, ir_r_diag, ir_l_fr, ir_r_fr, ir_error; //IR values

PID stopPID(&stopInp, &stopOut, &stopSet,2,0,0.3, DIRECT);
PID encLPID(&encLInp, &encLOut, &encLSet,2,0.5,0.5, DIRECT);
PID encRPID(&encRInp, &encROut, &encRSet,2,0.5,0.5, DIRECT);
PID trackPID(&trackInp, &trackOut, &trackSet,2,0,0.4, DIRECT);
PID gyroPID(&gyroInp, &gyroOut, &gyroSet, 25,0,0.5, DIRECT);//15 6 0.1

movingAvg avgD(50);  //Diagonal IR readings
movingAvg avgF(50);  //Front IR readings

void setup() 
{
  initializePins();
  initializeMemory();
  initializePID();
  initialilzeGyro();

  avgD.begin();
  avgF.begin();

  Serial.begin(19200);
//   printMaze();
}

void loop() 
{
  pullAccMeter(); //checks accelerometer if robot is lifted
  
  loopCount++;  //loop counter (used for temrinating condition)
  
  getIRValues();
  trackInp = ((double)ir_error)/20;
  
  encLInp = count_l;
  encRInp = count_r;
  encCount = (count_r+count_l)/2;

  stopInp = sensorSum;

  encLPID.Compute();
  encRPID.Compute();
  trackPID.Compute();
  stopPID.Compute();

//  Serial.print(ir_l_diag);
//  Serial.print('\t');
//  Serial.print(ir_r_diag);
//  Serial.print('\t');
//  Serial.print(ir_error);
//  Serial.print('\t');
//  Serial.println(trackOut);
  
  if (halt)
  {
    ML(0);
    MR(0);
    delay(100);
    if (nxtMove==LEFT) turnLeft();
    if (nxtMove==RIGHT) turnRight();
    if (nxtMove==BACK) turnBack();
    count_l=0;
    count_r=0;
    currMove = STOP;
    nxtMove = GO;
    if (!finished) halt =  false;
  }
  else if (encCount<square/4 && currMove==STOP)
  {
    ML((trackOut+spd)/2);
    MR((-trackOut+spd)/2); 
  }
  else if(encCount>square/2 && !checked)
  {
    checked = true;
    if (preDist==100)
    {
      updatePos();
      wallCheck();
      floodFill();
      lowestFirst();
    }
    else
    {
      preDist++;
      wallLeft = (ir_l_diag>L_offset/2)?1:0;                      //Different condition for wall checking
      wallRight = (ir_r_diag>R_offset/2)?1:0;
      wallFront = (ir_r_fr>wallThresh & ir_l_fr>wallThresh)?1:0;
      
      if (wallLeft==0)
      {
        startX = 13;
        posX = 13-preDist;
        for(int x=13; x>13-preDist; x--)
        {
          visit(x);
          setVal(x,power(2,SOUTH));
          setVal(x+14,power(2,NORTH));
        }
        heading = WEST;
        startHeading = WEST;
        preDist = 100;
        wallCheck();
        floodFill();
        lowestFirst();
      }
      else if (wallRight==0)
      {
        startX = 0;
        posX = preDist;
        for(int x=0; x<preDist; x++)
        {
          visit(x);
          setVal(x,power(2,SOUTH));
          setVal(x+14,power(2,NORTH));
        }
        heading = EAST;
        startHeading = EAST;
        preDist = 100;
        wallCheck();
        floodFill();
        lowestFirst();
      }
    }
//    printMaze();
  }
  else if(encCount>square/2 && wallFront==1)
  {
    ML(stopOut);
    MR(stopOut);
    if (encCount!=prevEncCount) 
    {
      loopCount=0;
      prevEncCount = encCount;
    }
    else if (loopCount>=50) resetCount();
  }
  else if(encCount>square/4 && nxtMove!=GO)
  {
    ML(encLOut);
    MR(encROut);
//      Serial.print((trackOut+spd)*encOut/spd);
//      Serial.print('\t');
//      Serial.println((-trackOut+spd)*encOut/spd);
  }
  else
  {
    ML(trackOut+spd);
    MR(-trackOut+spd);
  }

  if(encCount>square && wallFront==0)
  {
    resetCount();
  }

  if (finished && halt)
  {
    ML(0);
    MR(0);
    delay(1000);
    if(go == DEST)
    {
      for(int x=0; x<14; x++) for(int y=0; y<14; y++)
        dist[x+14*y] = absol(x,startX) + absol(y,startY);
      floodFill();
      finished = false;
      halt = false;
      currMove = STOP;
      nxtMove = GO;
      go = HOME;
    }
    else if(go == HOME)
    {
      for (byte i=0; i<=6; i++) for(byte j=0; j<=6; j++)
        {
          dist[i+14*j] = 12-i-j;
          dist[i+7+14*j] = 6-j+i;
          dist[i+14*(j+7)] = 6+j-i;
          dist[i+7+14*(j+7)] = j+i;
        }
      virtualWalls();
      
      floodFill();;
      finished = false;
      halt = false;
      delay(100);
      turnBack();
      currMove = STOP;
      nxtMove = GO;
      count_r=0;
      count_l=0;
      prevEncCount=0;
      go = DEST;
    }
  }
}

void resetCount()
{
  checked=false;
  count_r=0;
  count_l=0;
  prevEncCount=0;
  if (nxtMove!=GO) halt=true;
  currMove = nxtMove;  
}

void lowestFirst()
{
  if ((go==DEST)&&((posX==6&&posY==6)||(posX==7&&posY==6||(posX==6&&posY==7)||(posX==7&&posY==7)))) 
  {
    finished = true;
    nxtMove = STOP;
    currMove = STOP;
    return;
  }

  if ((go==HOME)&&(posX==startX&&posY==startY))
  {
    finished = true;
    nxtMove = STOP;
    currMove = STOP;
    return;
  }

  cellConforms(posX,posY);
  
  if(readMaze(posX+14*posY,heading)==0 && dist[getCell(heading)]==minNeighbour) nxtMove = GO;
  else if(readMaze(posX+14*posY,right())==0 && dist[getCell(right())]==minNeighbour) nxtMove = RIGHT;
  else if(readMaze(posX+14*posY,left())==0 && dist[getCell(left())]==minNeighbour) nxtMove = LEFT;
  else nxtMove = BACK; 
}

void getIRValues()
{
  ir_r_diag = IR_read(LED_R_DIAG, PT_R_DIAG);
  ir_l_diag = IR_read(LED_L_DIAG, PT_L_DIAG);
  
  ir_r_fr = IR_read(LED_R_FR, PT_R_FR);
  ir_l_fr = IR_read(LED_L_FR, PT_L_FR);

  if(ir_l_diag > L_offset && ir_r_diag < R_offset)
    ir_error = L_offset - ir_l_diag;
  else if(ir_l_diag < L_offset && ir_r_diag > R_offset)
    ir_error = ir_r_diag - R_offset;
  else if(ir_l_diag > wallThresh && ir_r_diag < wallThresh)
    ir_error = L_offset - ir_l_diag;
  else if(ir_r_diag > wallThresh && ir_l_diag < wallThresh)
    ir_error = ir_r_diag - R_offset;
  else
    ir_error = 0;
  ir_error = avgD.reading(ir_error);
  
  sensorSum =  (double)(avgF.reading(ir_r_fr + ir_l_fr ));
  sensorSum = 60*log(sensorSum);
  sensorSum = sensorSum<200?200:sensorSum;
}

void wallCheck()
{
  wallLeft = (ir_l_diag>L_offset/2)?1:0;
  wallRight = (ir_r_diag>R_offset/2)?1:0;
  wallFront = (ir_r_fr>wallThresh & ir_l_fr>wallThresh)?1:0;
  
  setVal(posX+14*posY,wallLeft*power(2,left())+wallRight*power(2,right())+wallFront*power(2,heading));

  if (posX!=0) setVal(posX-1+14*posY,readMaze(posX+14*posY,WEST)*4);
  if (posX!=13) setVal(posX+1+14*posY,readMaze(posX+14*posY,EAST)*8);
  if (posY!=0) setVal(posX+14*(posY-1),readMaze(posX+14*posY,NORTH)*2);
  if (posY!=13) setVal(posX+14*(posY+1),readMaze(posX+14*posY,SOUTH));
}

void initializeMemory()
{
  posX = startX;
  posY = startY;
  for (byte i=0; i<=6; i++) for(byte j=0; j<=6; j++)
  {
    dist[i+14*j] = 12-i-j;
    dist[i+7+14*j] = 6-j+i;
    dist[i+14*(j+7)] = 6+j-i;
    dist[i+7+14*(j+7)] = j+i;
  }

//  for(int x=0; x<14; x++) for(int y=0; y<14; y++)
//    dist[x+14*y] = absol(x,6) + absol(y,1);
//  dist[6+14*1] = 0;
//  dist[7+14*1] = 0;
//  dist[6+14*2] = 0;
//  dist[7+14*2] = 0;

  //Surrounding walls
  for(byte i=0; i<14; i++)
  {
   setVal(i,1);
   setVal(i+182,2);
   setVal(14*i,8);
   setVal(14*i+13,4);
  }
  visit(posX+14*posY);
}

void floodFill()
{
  
  for(byte x=0; x<196; x++) if(dist[x]==0) 
  {
    add(x);
    queuefy(x);
  }
  
  while(queueCount!=0)
  {
    byte check = remove();
    byte x = check%14;
    byte y = check/14;
    cellConforms(x,y);
    int chkDist = dist[check];
    if (readMaze(check,NORTH)==0 && !hasQueued(x+14*(y-1))) 
    {
      add(x+14*(y-1));
      dist[x+14*(y-1)] = chkDist+1;
      queuefy(x+14*(y-1));
    }
    if (readMaze(check,SOUTH)==0 && !hasQueued(x+14*(y+1))) 
    {
      add(x+14*(y+1));
      dist[x+14*(y+1)] = chkDist+1;
      queuefy(x+14*(y+1));
    }
    if (readMaze(check,EAST)==0 && !hasQueued(x+1+14*y)) 
    {
      add(x+1+14*y);
      dist[x+1+14*y] = chkDist+1;
      queuefy(x+1+14*y);
    }
    if (readMaze(check,WEST)==0 && !hasQueued(x-1+14*y)) 
    {
      add(x-1+14*y);
      dist[x-1+14*y] = chkDist+1;
      queuefy(x-1+14*y);
    }
  }

  for( byte x=0; x<25; x++) queued[x]=0;
}

boolean cellConforms(byte x, byte y)
{
  minNeighbour = 100;
  
  if(readMaze(x+14*y,NORTH)==0 && dist[x+14*(y-1)]<minNeighbour) minNeighbour = dist[x+14*(y-1)];
  if(readMaze(x+14*y,SOUTH)==0 && dist[x+14*(y+1)]<minNeighbour) minNeighbour = dist[x+14*(y+1)];
  if(readMaze(x+14*y,EAST)==0 && dist[x+1+14*y]<minNeighbour) minNeighbour = dist[x+1+14*y];
  if(readMaze(x+14*y,WEST)==0 && dist[x-1+14*y]<minNeighbour) minNeighbour = dist[x-1+14*y];
  if((readMaze(x+14*y,NORTH)+readMaze(x+14*y,SOUTH)+readMaze(x+14*y,WEST)+readMaze(x+14*y,EAST))==4) minNeighbour = 98;
  if(dist[x+14*y]==minNeighbour+1) return true;
  return false;
}

void initializePins()
{
  pinMode(MR_A1,OUTPUT);
  pinMode(MR_A2,OUTPUT);
  pinMode(ML_A1,OUTPUT);
  pinMode(ML_A2,OUTPUT);
  pinMode(EN_L,OUTPUT);
  pinMode(EN_R,OUTPUT);
  pinMode(LED_L_DIAG,OUTPUT);
  pinMode(LED_L_FR,OUTPUT);
  pinMode(LED_R_DIAG,OUTPUT);
  pinMode(LED_R_FR,OUTPUT);

    // LED initial OFF state
  digitalWrite(LED_L_FR, OFF);
  digitalWrite(LED_L_DIAG, OFF);
  digitalWrite(LED_R_FR, OFF);
  digitalWrite(LED_R_DIAG, OFF);

  attachInterrupt(digitalPinToInterrupt(ENC_R),count_R, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_L),count_L, RISING);
}

void initializePID()
{
  trackPID.SetOutputLimits(-255,255);
  trackPID.SetSampleTime(10);
  trackPID.SetMode(AUTOMATIC);

  encRPID.SetOutputLimits(-spd,spd);
  encRPID.SetSampleTime(10);
  encRPID.SetMode(AUTOMATIC);

  encLPID.SetOutputLimits(-spd,spd);
  encLPID.SetSampleTime(10);
  encLPID.SetMode(AUTOMATIC);

  gyroPID.SetOutputLimits(-150,150);
  gyroPID.SetSampleTime(10);
  gyroPID.SetMode(AUTOMATIC);

  stopPID.SetOutputLimits(-spd,spd);
  stopPID.SetSampleTime(10);
  stopPID.SetMode(AUTOMATIC);

    //Set points
  stopSet= 432; //432 for wood, 445 for foam
  encLSet = square+10;
  encRSet = square+10;
  trackSet = 0;
}

void initialilzeGyro()
{
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  delay(20);
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
}

void setVal(byte address, byte val)
{
  if (address%2==0) val*=16;
  maze[address/2] = maze[address/2] | val;
}

byte readMaze(byte address, byte dir)
{
  byte val = maze[address/2];
  byte walls[4] = {0,0,0,0};
  if (address%2==0) val/=16;
  walls[WEST] = (val&8)==8?1:0;
  walls[EAST] = (val&4)==4?1:0;
  walls[SOUTH] = (val&2)==2?1:0;
  walls[NORTH] = (val&1)==1?1:0;
  return walls[dir];
}

void visit(byte address)
{
  byte ad = address/8;
  visited[ad] = (visited[ad] | (byte)power(2,7-address%8));
}

boolean hasVisited(byte address)
{
  return((visited[address/8] & (byte)power(2,7-address%8)) == (byte)power(2,7-address%8));
}

	//set value of queued
void queuefy(byte address)
{
  byte ad = address/8;
  queued[ad] = (queued[ad] | (byte)power(2,7-address%8));
}

//check if queued
boolean hasQueued(byte address)
{
  return((queued[address/8] & (byte)power(2,7-address%8)) == (byte)power(2,7-address%8));
}

void virtualWalls()
{
  for(byte x=0; x<14; x++) for(byte y=0; y<14; y++) if(!hasVisited(x+14*y) && dist[x+14*y]!=0)
  {
    setVal(x+14*y,15);
    if (x!=0) setVal(x-1+14*y,power(2,EAST));
    if (x!=13) setVal(x+1+14*y,power(2,WEST));
    if (y!=0) setVal(x+14*(y-1),power(2,SOUTH));
    if (y!=13) setVal(x+14*(y+1),power(2,NORTH));
  }
}

void eliminate()
{
  for(int x=0; x<14; x++) for(int y=0; y<14; y++) 
    if(readMaze(x+14*y,NORTH)==1 & readMaze(x+14*y,SOUTH)==1 & readMaze(x+14*y,EAST)==1 & readMaze(x+14*y,WEST)==1)
      dist[x+14*y]=99;
}

void pullAccMeter(){

    Wire.beginTransmission(MPU);
    Wire.write(0x3B); // Gyro data first register address 0x43
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
    
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    //Serial.println(AccZ);

   
    if( AccZ < -0.71  ){
      while(1){
            ML(0);
            MR(0);
           // Serial.println(AccZ);
            Wire.beginTransmission(MPU);
            Wire.write(0x3B); // Gyro data first register address 0x43
            Wire.endTransmission(false);
            Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
            AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
            AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
            AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
           //Serial.println(AccZ);
           if( !( AccZ < 0.3 ) ){
              delay(5000);
              posX = startX;
              posY = startY;
              heading = startHeading;
              count_l=0;
              count_r=0;
              prevEncCount=0;
              currMove = STOP;
              nxtMove = GO;
              go = DEST;
              halt = false;
              checked = false;
              break;
           }
           delay(1);
      }
           
    }
}

  //Read IR values
int IR_read(int led, int pt)
{
  digitalWrite(led, LOW);
  delayMicroseconds(delay_time);
  int on = analogRead(pt);
  
  delayMicroseconds(delay_time);
  
  digitalWrite(led, HIGH);
  delayMicroseconds(delay_time);
  int off = analogRead(pt);
  delayMicroseconds(delay_time);
  
  return (on - off); 
}

  //Math functions
byte power(int base, int expon)
{
  byte val=1;
  for(int x=0; x<expon; x++)
    val=val*base;
  return val;
}

byte absol(byte a, byte b)
{
  return a>b? a-b: b-a;
}

  //Interrupts
void count_R()
{
  count_r++;
}

void count_L()
{
  count_l++;
}

  //Directions
byte left()
{
  switch (heading)
  {
  case EAST : return NORTH;
  case SOUTH : return EAST;
  case WEST : return SOUTH;
  case NORTH : return WEST;
  }
  return 0;
}

byte right()
{
  switch (heading)
  {
  case EAST : return SOUTH;
  case SOUTH : return WEST;
  case WEST : return NORTH;
  case NORTH : return EAST;
  }
  return 0;
}

byte back()
{
  switch (heading)
  {
  case EAST : return WEST;
  case SOUTH : return NORTH;
  case WEST : return EAST;
  case NORTH : return SOUTH;
  }
  return 0;
}

byte getCell(byte direction)
{
  switch (direction)
  {
    case NORTH : return posX + 14*(posY-1);
    case SOUTH : return posX + 14*(posY+1);
    case EAST : return posX+1+14*posY;
    case WEST : return posX-1+14*posY;
  
  }
  return 0;
}

void updatePos()
{
  switch(heading)
  {
  case(NORTH): posY--; break;
  case(SOUTH): posY++; break;
  case(EAST): posX++; break;
  case(WEST): posX--; break;
  }
  visit(posX+14*posY);
}

//queue implementation
void add(byte data) 
{
  queue[queueCount] = data;
  queueCount++;
}

byte remove() 
{
  queueCount--;
  byte val = queue[0];
  for(byte x=0; x<queueLimit-1; x++)
    queue[x] = queue[x+1];
  queue[queueLimit-1] = 0;
  return val;
}

  //Turns
void turnLeft()
{
  heading = left();
  rotate(-90);
}

void turnRight()
{
  heading = right();
  rotate(90);
}

void turnBack()
{
  heading = back();
  rotate(180);
}

  //Motor Power
void ML(int val)
{
  if(val>255)
  {
    val=255;
  }
  if(val<-255)
  {
    val=-255;
  }

  if (val > 0) 
  {
    digitalWrite(ML_A1 , HIGH);
    digitalWrite(ML_A2 , LOW);
    analogWrite(EN_L, abs(val));
  }
  else 
  {
    digitalWrite(ML_A1 , LOW);
    digitalWrite(ML_A2 , HIGH);
    analogWrite(EN_L, abs(val));
  }
}

void MR(int val) 
{
  if(val>255)
  {
    val=255;
  }
  if(val<-255)
  {
  val=-255;
  }

  if (val > 0) 
  {
    digitalWrite(MR_A1 , HIGH);
    digitalWrite(MR_A2 , LOW);
    analogWrite(EN_R, abs(val));
  } else 
  {
    digitalWrite(MR_A1 , LOW);
    digitalWrite(MR_A2 , HIGH);
    analogWrite(EN_R, abs(val));
  }
}

void rotate(int angle) 
{
  yaw =0;
  int dirr=0;
  unsigned long t1= millis();

  gyroPID.SetMode(AUTOMATIC);
  gyroSet = angle;

  while(1)
  {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
    
    // === Read gyroscope data === //
    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = micros();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000000; // Divide by 1000 to get seconds
    
    Wire.beginTransmission(MPU);
    Wire.write(0x43); // Gyro data first register address 0x43
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
    
    GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
    GyroZ = (Wire.read()<< 8 | Wire.read()) / 32.75;
    GyroZ = GyroZ +0.74;
    yaw = yaw - GyroZ*elapsedTime; //CW-- decrease
    
    
    gyroInp = yaw;
    gyroPID.Compute();  

    ML(gyroOut);    // clockWise
    MR(-gyroOut);

    if( millis() - t1  > 500 )
    {
      ML(0);
      MR(0);
      gyroPID.SetMode(MANUAL);
      return 1; 
    }   
  }
}

void printMaze()
{
  for(byte i=0; i<14; i++)Serial.print("+---");
  Serial.println("+");
  for (byte y=0; y<14; y++) 
  {
    for (byte x=0; x<14; x++)
    {
      if(readMaze(x+14*y,WEST)==1)
        Serial.print("|");
      else
        Serial.print(" ");
      if (x==posX && y==posY)
        switch (heading)
        {
          case(NORTH):Serial.print(" ^ "); break;
          case(SOUTH):Serial.print(" V "); break;
          case(EAST):Serial.print(" > "); break;
          case(WEST):Serial.print(" < "); break;
        }
      else
      {
        if (dist[x+14*y]==99) Serial.print("|||");
        else if (dist[x+14*y]>9) Serial.print(String(dist[x+14*y])+String(" "));
        else Serial.print(String(" ")+String(dist[x+14*y])+String(" "));
      }
    }
    Serial.println("|");
    for (byte x=0; x<14; x++)
    {
      Serial.print("+");
      if(readMaze(x+14*y,SOUTH)==1)
        Serial.print("---");
      else
        Serial.print("   ");
    }
    Serial.println("+");
  }
}
