/*
 * th.c  capture Temperature and Humidity readings, write them to sql database
 *	https://projects.drogon.net/raspberry-pi/wiringpi/

rev 1.0 12/01/2013 WPNS built from Gordon Hendersen's rht03.c
rev 1.1 12/01/2013 WPNS don't retry, takes too long
rev 1.2 12/01/2013 WPNS allow one retry after 3-second delay
rev 1.3 12/01/2013 WPNS make cycle time variable
rev 1.4 12/01/2013 WPNS add mysql stuff in
rev 1.5 12/01/2013 WPNS do 60 second cycle, cleanup, trial run
rev 1.6 12/01/2013 WPNS clean up output format
rev 1.7 12/02/2013 WPNS allow more retries, minor cleanups
rev 1.79 12/04/2013 WPNS release to instructables

 */

#include <stdio.h>

#include <wiringPi.h>

#include <mysql/mysql.h>

#include <maxdetect.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#define MAX_TIME 85
#define DHT11PIN 7
int dht11_val[5]={0,0,0,0,0};

// #define	RHT03_PIN	7
// #define CYCLETIME      60
// #define RETRIES         3

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

void dht11_read_val()
{
  uint8_t lststate=HIGH;
  uint8_t counter=0;
  uint8_t j=0,i;
  // float farenheit;

  char SQLstring[64];            // string to send to SQL engine
  // char TimeString[64];           // formatted time
  // time_t rawtime;
  // struct tm * timeinfo;

  // setup mysql connection
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) finish_with_error(con);

  if (mysql_real_connect(con, "localhost", "root", "password",
       "monitoring", 0, NULL, 0) == NULL) finish_with_error(con);
  // end setup mysql

  for(i=0;i<5;i++)
     dht11_val[i]=0;
  pinMode(DHT11PIN,OUTPUT);
  digitalWrite(DHT11PIN,LOW);
  delay(18);
  digitalWrite(DHT11PIN,HIGH);
  delayMicroseconds(40);
  pinMode(DHT11PIN,INPUT);

  for(i=0; i<MAX_TIME; i++)
  {
    counter=0;
    while(digitalRead(DHT11PIN)==lststate){
      counter++;
      delayMicroseconds(1);
      if(counter==255)
        break;
    }
    lststate=digitalRead(DHT11PIN);
    if(counter==255)
       break;
    // top 3 transistions are ignored
    if((i>=4)&&(i%2==0)){
      dht11_val[j/8]<<=1;
      if(counter>16)
        dht11_val[j/8]|=1;
      j++;
    }
  }

  // verify cheksum and print the verified data
  if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF)))
  {
    // farenheit=dht11_val[2]*9./5.+32;
    // printf("Humidity = %d.%d %% Temperature = %d.%d *C\n",dht11_val[0],dht11_val[1],dht11_val[2],dht11_val[3]);

    fflush(stdout);

    sprintf(SQLstring,"INSERT INTO TempHumid VALUES(unix_timestamp(now()),%d.%d,%d.%d)",dht11_val[2],dht11_val[3],dht11_val[0],dht11_val[1]);
    //      printf("%s\n",SQLstring);
    if (mysql_query(con, SQLstring)) finish_with_error(con);
  }
  else
    printf("Invalid Data!!\n");
}

int main(void)
{
  printf("Interfacing Temperature and Humidity Sensor (DHT11) With Raspberry Pi\n");
  fflush(stdout);
  if(wiringPiSetup()==-1)
    exit(1);
  while(1)
  {
     dht11_read_val();
     delay(3000);
  }
  return 0;
}

/*
 ***********************************************************************
 * The main program
 ***********************************************************************
 */

// int main (void)
// {
//   int temp, rh ;                 // temperature and relative humidity readings
//   int loop;                      // how many times through the loop?
//   time_t oldtime,newtime;        // when did we last take a reading?
//   //  int deltime;                   // how many seconds ago was that?

//<<<<<<< HEAD
  //if (mysql_real_connect(con, "localhost", "root", "password",
//			 "monitoring", 0, NULL, 0) == NULL) finish_with_error(con);
//=======
//   // char SQLstring[64];            // string to send to SQL engine
//   // char TimeString[64];           // formatted time
//   // time_t rawtime;
//   // struct tm * timeinfo;
//>>>>>>> 823fcdc42ff9a033ec1552a4c4e53df5f204c2f4

//   int status;                   // how did the read go?

//   temp = rh = loop = 0 ;
//   oldtime = (int)time(NULL);

//   wiringPiSetup () ;
//   piHiPri       (55) ;

//   printf("rh.c rev 1.79 12/04/2013 WPNS %sCycle time: %i seconds, %i retries\n",ctime(&oldtime),CYCLETIME,RETRIES);
//   // fflush(stdout);

//   // MYSQL *con = mysql_init(NULL);

//   // if (con == NULL) finish_with_error(con);

//   // if (mysql_real_connect(con, "localhost", "root", "password",
// 		// 	 "Monitoring", 0, NULL, 0) == NULL) finish_with_error(con);

//   // wait for an interval to start and end
//   printf("Sync to cycletime...");
//   fflush(stdout);
//   while ((((int)time(NULL))%CYCLETIME)) delay(100);
//   oldtime = (int)time(NULL);
//   while (!(((int)time(NULL))%CYCLETIME)) delay(100);
//   printf("\n");
//   fflush(stdout);

//   for (;;)
//     {
//       // wait for an interval to start
//       while ((((int)time(NULL))%CYCLETIME)) delay(100);

// /*****************************************************************/

//       // read new data
//       temp = rh = -1;
//       loop=RETRIES;

//       status = readRHT03 (RHT03_PIN, &temp, &rh);
//       while ((!status) && loop--)
// 	{
// 	  printf("-Retry-");
// 	  fflush(stdout);
// 	  delay(3000);
// 	  status = readRHT03 (RHT03_PIN, &temp, &rh);
// 	}

//       newtime = (int)time(NULL);
//       //      deltime = newtime-oldtime;
//       time(&rawtime);
//       timeinfo = localtime (&rawtime);
//       strftime (TimeString,64,"%F %T",timeinfo);

//       printf ("%s  Temp: %5.1f, RH: %5.1f%%\n", TimeString, temp / 10.0, rh / 10.0) ;
//       // fflush(stdout);
//       oldtime = newtime;

//       // sprintf(SQLstring,"INSERT INTO TempHumid VALUES(unix_timestamp(now()),%5.1f,%5.1f)",(temp / 10.0),(rh / 10.0));
//       // //      printf("%s\n",SQLstring);
//       // if (mysql_query(con, SQLstring)) finish_with_error(con);

// /*****************************************************************/

//       // wait for the rest of that interval to finish
//       while (!(((int)time(NULL))%CYCLETIME)) delay(100);

//     }

//   return 0 ;
// }
