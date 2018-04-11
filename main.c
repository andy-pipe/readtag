#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#define MAX_BUF 1024

void sensorTmp007Convert(uint16_t rawAmbTemp, uint16_t rawObjTemp, float *tAmb, float *tObj)
{
  const float SCALE_LSB = 0.03125;
  float t;
  int it;
 
  it = (int)((rawObjTemp) >> 2);
  t = ((float)(it)) * SCALE_LSB;
  *tObj = t;
 
  it = (int)((rawAmbTemp) >> 2);
  t = (float)it;
  *tAmb = t * SCALE_LSB;
}

float sensorOpt3001Convert(uint16_t rawData)
{
  uint16_t e, m;
 
  m = rawData & 0x0FFF;
  e = (rawData & 0xF000) >> 12;
 
  return m * (0.01 * pow(2.0,e));
}

int main(int argc, char *argv[])
{
    int fdi, fdo;    	// FIFO Handles
    int index;		//Index auf gefundene "Needle"
    char * fifoin = "/tmp/eingang";
    char * fifoout = "/tmp/ausgang";
    char buf[MAX_BUF];
    ssize_t len = 0;
    char befehl[] = "connect\n";
    char tempstr[100];

    /* write connect to the FIFO */
    fdi = open(fifoin, O_WRONLY);
    write(fdi, befehl, (sizeof(befehl) - 1));
    printf("Sending: %s\n", befehl);

    /* read from FIFO */
    fdo = open(fifoout, O_RDONLY);
    write(fdi, befehl, (sizeof(befehl) - 1));
    while ((len = read(fdo, buf, MAX_BUF)) != 0) {
      buf[len] = 0;
      //printf("Length:   %d\n", (int)len);
      //printf("%s", buf);
      if (strstr(buf, "Connection successful")) {
        printf("Connection established\n");
        printf("Configuring services\n");
        write(fdi, "char-write-req 0x0024 01\n", 25);
        write(fdi, "char-write-req 0x0044 01\n", 25);
        write(fdi, "char-write-req 0x0022 0100\n", 27);
        write(fdi, "char-write-req 0x0042 0100\n", 27);
        write(fdi, "char-write-req 0x004A 0100\n", 27);
        }
      if (index = strstr(buf, "handle = 0x0021 value:")) {
        index = index - (int) &buf[0] + 23;
        buf[11] = 0;
	//printf("Temperaturwert: %s\s", &buf[index]);
        buf[index + 2] = 0;
        buf[index + 5] = 0;
        buf[index + 8] = 0;
        int rAT = (int)strtol(&buf[index], NULL, 16);
        rAT = rAT + (256 * (int)strtol(&buf[index+3], NULL, 16));
        int rOT = (int)strtol(&buf[index + 6], NULL, 16);
        rOT = rOT + (256 * (int)strtol(&buf[index + 9], NULL, 16));
        //printf(" rAT: %d rOT %d \n", rAT, rOT);
	float tAmb, tObj;
	sensorTmp007Convert(rAT, rOT, &tAmb, &tObj);
        printf("Umgebung:    %.2f°C\n", tAmb);
        printf("Oberfläche:  %.2f°C\n", tObj);
        sprintf(tempstr, "%.2f°C", tAmb);
        setenv("TAMB", tempstr, 1);
        sprintf(tempstr, "%.2f°C", tObj);
        setenv("TOBJ", tempstr, 1);
        }
      if (index = strstr(buf, "handle = 0x0041 value:")) {
        index = index - (int) &buf[0] + 23;
        buf[index + 5] = 0;
	//printf("Licht: %s\s", &buf[index]);
        buf[index + 2] = 0;
        int light = (int)strtol(&buf[index], NULL, 16);
        light = light + (256 * (int)strtol(&buf[index + 3], NULL, 16));
        //printf(" light: %d\n", light);
        printf("Helligkeit:  %.2f LUX\n", sensorOpt3001Convert(light));
        }
      if (index = strstr(buf, "handle = 0x0049 value:")) {
        index = index - (int) &buf[0] + 23;
        buf[index + 2] = 0;
        printf("Schalter:    %s\n", &buf[index]);
        }

     }
     return 0;
}
