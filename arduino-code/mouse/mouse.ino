const byte NORTHP = 1, SOUTHP = 2, EASTP = 4, WESTP = 8;
const byte NORTH=0, SOUTH=1, EAST=2, WEST=3;
const byte startX=0, startY=0;
const byte DEST=0, HOME=1;
  

const byte data[98] = {177,55,177,53,219,89,55,178,19,90,92,203,74,53,217,37,163,72,107,3,54,202,124,177,74,51,37,151,201,22,220,163,121,124,165,166,203,78,179,48,50,54,145,75,1,21,220,179,23,204,167,204,166,200,49,39,232,121,74,53,166,150,149,180,180,163,88,55,203,78,218,90,55,200,51,35,39,163,1,121,106,53,177,53,147,104,90,51,92,146,124,235,126,163,51,106,107,54};
  
  //robot data
byte maze[98]; //array of walls
byte dist[196]; //array of floodfill values
byte visited[25];
byte posX=0, posY=0; //current position of the robot
byte heading = EAST; //heading of the robot
byte go = DEST; //Determines if the robot is going to the destination or back to the starting point
boolean finished = false; //becomes true when destination is reached
byte minNeighbour = 100; //value of the minimum neighbour
  
  //stack variables
byte stackCount = 0;
const byte stackLimit = 200;
byte stack[stackLimit];

void setup() 
{
  Serial.begin(250000);
  
  initialize();
  printMaze();
  
  while(!finished)
  {
    lowestFirst();
    floodFill();
    printMaze();
    if(!finished)move1();
//    delay(500);
  }
  printMaze();
  
  for(byte x=0; x<14; x++) for(byte y=0; y<14; y++)
    dist[x+14*y] = abs(x-startX) + abs(y-startY);
  
  byte tempX = posX, tempY = posY;
  for(byte x=0; x<14; x++) for(byte y=0; y<14; y++)
  {
    posX=x;
    posY=y;
    floodFill();
  }
  posX = tempX;
  posY = tempY;
  
  printMaze();
  
  finished = false;
  go = HOME;
  
  while(!finished)
  {
    lowestFirst();
    floodFill();
    printMaze();
    if(!finished) move1();
//    delay(500);
  }
  
  printMaze();
  
  for (byte i=0; i<=6; i++) for(byte j=0; j<=6; j++)
  {
    dist[i+14*j] = 12-i-j;
    dist[i+7+14*j] = 6-j+i;
    dist[i+14*(j+7)] = 6+j-i;
    dist[i+7+14*(j+7)] = j+i;
  }
  
  virtualWalls();
  printMaze();
  
  tempX = posX;
  tempY = posY;
  for(byte x=0; x<14; x++) for(byte y=0; y<14; y++)
  {
    posX=x;
    posY=y;
    floodFill();
  }
  posX = tempX;
  posY = tempY;
  
  heading = back();
  printMaze();
}

void loop() {
  // put your main code here, to run repeatedly:

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

//  for(byte x=0; x<50; x++) Serial.println("");
}

byte dec(byte address, byte dir)
{
  byte val = data[address/2];
  byte walls[4] = {0,0,0,0};
  if (address%2==0) val/=16;
  walls[WEST] = (val&8)==8?1:0;
  walls[EAST] = (val&4)==4?1:0;
  walls[SOUTH] = (val&2)==2?1:0;
  walls[NORTH] = (val&1)==1?1:0;
  return walls[dir];
}

void initialize()
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
  for(byte i=0; i<14; i++)
  {
    enc(i,1);
    enc(i+182,2);
    enc(14*i,8);
    enc(14*i+13,4);
  }
  visit(posX+14*posY);
}

void lowestFirst()
{ 
  if ((go==DEST)&&((posX==6&&posY==6)||(posX==7&&posY==7||(posX==6&&posY==7)||(posX==7&&posY==6)))) 
  {
    finished = true;
    return;
  }
  
  if ((go==HOME)&&(posX==startX&&posY==startY))
  {
    finished = true;
    return;
  }
  
  wallCheck();
  floodFill();
  
  cellConforms(posX,posY);
  
  if(readMaze(posX+14*posY,heading)==0 && dist[getCell(heading)]==minNeighbour);
  else if(readMaze(posX+14*posY,right())==0 && dist[getCell(right())]==minNeighbour) heading = right();
  else if(readMaze(posX+14*posY,left())==0 && dist[getCell(left())]==minNeighbour) heading = left();
  else heading = back();
}

void floodFill()
{
  byte n=0;
  push(posX+14*posY);
  
  if(readMaze(posX+14*posY,NORTH)==1 && posY!=0 && dist[posX+14*(posY-1)]!=0) push(posX+14*(posY-1));
  if(readMaze(posX+14*posY,SOUTH)==1 && posY!=13 && dist[posX+14*(posY+1)]!=0) push(posX+14*(posY+1));
  if(readMaze(posX+14*posY,EAST)==1 && posX!=13 && dist[posX+1+14*posY]!=0) push(posX+1+14*posY);
  if(readMaze(posX+14*posY,WEST)==1 && posX!=0 && dist[posX-1+14*posY]!=0) push(posX-1+14*posY);
    
  while(stackCount!=0)
  {
    n++;
    byte check = pop();
    byte x = check%14;
    byte y = check/14;
    
    if (!cellConforms(x,y))
    {
      if (dist[check]!=0) dist[check] = dist[check]<98?minNeighbour+1:99;
      if (readMaze(check,NORTH)==0) push(x+14*(y-1));
      if (readMaze(check,SOUTH)==0) push(x+14*(y+1));
      if (readMaze(check,EAST)==0) push(x+1+14*y);
      if (readMaze(check,WEST)==0) push(x-1+14*y);
    }
    if (n>200) break;
  }
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

void virtualWalls()
{
  for(byte x=0; x<14; x++) for(byte y=0; y<14; y++) if(!hasVisited(x+14*y) && dist[x+14*y]!=0)
  {
    enc(x+14*y,15);
    if (x!=0) enc(x-1+14*y,EASTP);
    if (x!=13) enc(x+1+14*y,WESTP);
    if (y!=0) enc(x+14*(y-1),SOUTHP);
    if (y!=13) enc(x+14*(y+1),NORTHP);
  }
}

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

void enc(byte address, byte val)
{
  if (address%2==0) val*=16;
  maze[address/2] = maze[address/2] | val;
}

void visit(byte address)
{
  byte ad = address/8;
  visited[ad] = (visited[ad] | (int)power(2,7-address%8));
}

boolean hasVisited(byte address)
{
  return((visited[address/8] & (byte)power(2,7-address%8)) == (byte)power(2,7-address%8));
}

byte power(int base, int expon)
{
  byte val=1;
  for(int x=0; x<expon; x++)
    val=val*base;
  return val;
}

void push(byte data)
{
  for(byte x = stackLimit-1; x>0; x--)
    stack[x] = stack[x-1];
  stack[0] = data;
  stackCount++;

}

byte pop()
{
  stackCount--;
  byte val = stack[0];
  for(byte x=0; x<stackLimit-1; x++)
    stack[x] = stack[x+1];
  stack[stackLimit-1]=0;
  return val;
}

///REPLACE WITH HARDWARE FUNCTIONS
void wallCheck()
{
  enc(posX+14*posY,dec(posX+14*posY,WEST)*8+dec(posX+14*posY,EAST)*4+dec(posX+14*posY,SOUTH)*2+dec(posX+14*posY,NORTH));
  if (posX!=0) enc(posX-1+14*posY,dec(posX+14*posY,WEST)*4);
  if (posX!=13) enc(posX+1+14*posY,dec(posX+14*posY,EAST)*8);
  if (posY!=0) enc(posX+14*(posY-1),dec(posX+14*posY,NORTH)*2);
  if (posY!=13) enc(posX+14*(posY+1),dec(posX+14*posY,SOUTH));
}

void move1()
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

void turnLeft()
{
  heading = left();
}

void turnRight()
{
  heading = right();
}
