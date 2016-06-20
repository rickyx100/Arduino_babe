#include <DS3231.h>

DS3231 rtc(SDA, SCL);

#define RELAY1  11
#define RELAY2  12

int HOURSETTING = 10;

int NextRunningHour = 0;
int NextRunningMinute = 0;
int NextRunningSecond = 0; 

int HourTolerance = 0;
int MinuteTolerance = 0;
int SecondTolerance = 5;

// simulation purpose
int second = 58;
int minute = 59;
int hour = 23;

bool IsRunning = false;
bool IsSimulation = false;

void GetCurrentTimeArray(int sz[3])
{
  char *p = rtc.getTimeStr();
  Serial.println(rtc.getTimeStr());
  char *str;
  int i = 0;
  
  while ((str = strtok_r(p, ":", &p)) != NULL && i < 3) // delimiter is the semicolon
  {
    sz[i] = ((String)str).toInt();
    i++;
  }
}

void SetRelayState(bool isReset)
{
  if(isReset)
  {
    digitalWrite(RELAY1, LOW);
    digitalWrite(RELAY2, LOW);
    delay(10000);
    return;
  }
  
  digitalWrite(RELAY1, !IsRunning);
  digitalWrite(RELAY2, IsRunning);
}

void SetNextRunningTime()
{
  int currentTime[3];

  if(IsSimulation)
  {
    SimulateCurrentTime(currentTime);
  }
  else
  {
    GetCurrentTimeArray(currentTime);
  }
  
  NextRunningHour = currentTime[0] + HOURSETTING;
  if(NextRunningHour > 23)
  {
    NextRunningHour = NextRunningHour - 24;
  }

  NextRunningMinute = currentTime[1];

  NextRunningSecond = currentTime[2];

  Serial.println("NextRunningTime: " + (String)NextRunningHour + ":" + (String)NextRunningMinute + ":" + (String)NextRunningSecond);
}

bool IsWithinHourTolerance(int currentHour)
{
  int nextHour = currentHour + HourTolerance;
  if(nextHour > 23)
  {
    nextHour = nextHour - 24;
  }
  return ((NextRunningHour == currentHour) ||  (NextRunningHour == nextHour));
}

bool IsWithinMinuteTolerance(int currentMinute)
{
  int nextMinute = currentMinute + MinuteTolerance;
  if(nextMinute > 59)
  {
    nextMinute = nextMinute - 60;
  }
  return ((NextRunningMinute == currentMinute) ||  (NextRunningMinute == nextMinute));
}

bool IsWithinSecondTolerance(int currentSecond)
{
  int nextSecond = currentSecond + SecondTolerance;
  if(nextSecond > 59)
  {
    nextSecond = nextSecond - 60;
    NextRunningMinute++;
    if(NextRunningMinute > 59)
    {
      NextRunningMinute = 0;
      NextRunningHour++;
    }
  }
  return ((NextRunningSecond >= currentSecond) &&  (NextRunningSecond <= nextSecond));
}

void SimulateCurrentTime(int sz[3])
{
  if(second < 59)
  {
    second += 1;
  }
  else
  {
    minute += 1;
    if(minute >59)
    {
      hour += 1;
      if(hour > 23)
      {
        hour = 0;
      }
      minute = 0;
    } 
    second = 0;
  }

  sz[0] = hour;
  sz[1] = minute;
  sz[2] = second;
  delay(3);
}

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  rtc.begin();
  
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);

  if(IsSimulation)
  {
    HOURSETTING = 2;
  }
  
  SetRelayState(true);

  SetNextRunningTime();
  
  SetRelayState(false);
}

void loop() 
{
  int currentTime[3];
  if(IsSimulation)
  {
    SimulateCurrentTime(currentTime);
  }
  else
  {
    GetCurrentTimeArray(currentTime);
  }
  Serial.println("CurrentTime: " + (String)currentTime[0] + ":" + (String)currentTime[1] + ":" + (String)currentTime[2]);
  // check current hour
  if(IsWithinHourTolerance(currentTime[0]))
  {
    // check current minute
    if(IsWithinMinuteTolerance(currentTime[1]))
    {
      // check current second
      if(IsWithinSecondTolerance(currentTime[2]))
      {
        SetRelayState(true);
        SetNextRunningTime();
        if(!IsRunning)
        {
          IsRunning = true;
        }
        else
        {
          IsRunning = false;
        }
        SetRelayState(false);
      }
    }
  }
  if(!IsSimulation)
    delay(500);
}


