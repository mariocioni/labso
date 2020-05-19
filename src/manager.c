/*
 * manager.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "distance_calculator.h"
#include <time.h>
#include <wait.h>
#define MAX_I 10
#define delay 1

void read_comp(FILE *fp);
FILE *shared_fp, *PFC1_log;
int status;
int main(int argc, char **argv) {
//initialize
    FILE *fp;
	int pidPFC3;
    if (argc != 2){
    if ((fp = fopen("G18.txt", "r")) == NULL) {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);}
	}
	else{
		fp = fopen(argv[1], "r");
		}
	shared_fp=fopen("sharedFile.txt","w+");
	PFC1_log=fopen("./log/speedPFC1.log","w+");
		
	pidPFC3=fork();
	if (pidPFC3==0){	
		printf ("PFC3 starts \n");
		read_comp(fp);
		exit(status);
	}
	else{
		int p=0;
		//printf ("meanwhile parent.. \n");
		double speed_pfc3;
		int count=0,tmp=0;
		while(1){
				if (fscanf(shared_fp, "%lf %d\n ", &speed_pfc3, &count)>0){
					if (count<0)
						break;
					if(count>tmp) {
						fprintf(PFC1_log, "%lf %d\n", speed_pfc3, count); 
						printf("Parent received %lf at count %d and temp %d\n",speed_pfc3, count, tmp);
						tmp=count;
					}	
				}
			}
			printf("parent ended at count =%d and tmp = %d \n",count,tmp);
		}
	wait(&status);	
	fclose(fp);
	fclose(shared_fp); 
	fclose(PFC1_log);
    return 0;
}

// actual reading and computing
void read_comp(FILE *fp){
	char *line = NULL;char output[50];
    char lat_st[10], longit_st[11], times_st[7]; //to store latitude, long. and time
    double lat, longit, prev_lat, prev_longit, dist, speed =0;
    long times, prev_times;
	size_t len = 0;
	int i=1;
	char fileString[7]="      "; 
	char ourString[]="$GPGLL";
	while(getline(&line, &len, fp) != -1) {	
		strncpy(fileString, line, 6);
		if(strcmp(fileString, ourString)==0){
			//Extract info from line read: lat. long and time	
			substring(line,lat_st,8,9);
			substring(line,longit_st,20,10);
			lat=atof(lat_st)/100.0;
			longit=atof(longit_st)/100.0;
			substring(line,times_st,33,6);
			times=atol(times_st);
			if(i>1){
				dist=haversine_dist(lat,longit,prev_lat,prev_longit);
				speed = dist/(times-prev_times);
				//printf("Lat.=%f Long.=%f Distance in mt=%f Speed=%f \n",lat,longit,dist, speed);
			}
			//communicate with parent	
			fseek(shared_fp,0,0);
			comm(speed,i);
			prev_lat=lat; prev_longit=longit; prev_times=times;// to be used next round
			i++;
			if(i>MAX_I){
				fseek(shared_fp,0,0);
				printf("I'll send the -10 signal to parent now\n");
				comm(speed,-10);
//				fprintf(shared_fp, "%lf %d\n", 100.19, -10); 
				break;		//reads up to MAX_I rows of input
			}
		}	
	}
	
	free(line);
	exit(status);
}


// C substring function definition
void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}
// Converts hhmmss to seconds
long tosec(long k){
	long tmp=k/10000;
	long h=tmp*3600;
	tmp*=10000;
	tmp=k-tmp;
	long m=tmp/100; 
	long sec = tmp-m*100;
	m*=60;
	return (sec+h+m);
}

void comm(double s, int count){
	fseek(shared_fp,0,0);
	fprintf(shared_fp, "%lf %d\n", s,count); 
	fseek(shared_fp,0,0);
	//printf("PCF3 wrote %lf - %d\n",s, count);
	//kill(getppid(),SIGUSR1);
	printf ("Child sent: %lf %d\n", s,count);
	sleep(delay);
	}
