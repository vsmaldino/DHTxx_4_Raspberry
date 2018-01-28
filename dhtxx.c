/*
 *  dhtxx.c:
 *	read temperature and humidity from DHT11(??) or DHT22 sensor
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "dhtxx.h"

#define DHTxx_MAX_TIMINGS	5060
#define DHTxx_MAXVALS 200
#define DHTxx_MINRETRIES 2
#define DHTxx_MINDELAYTIME 2200 /* milliseconds */

int DHTxx_read_data(uint8_t dhtpin, float *ftemperature, float *fhumidity, uint8_t *effvals)
/*
 * dhtpin  -> Pin number using WiringPi numeration
 * ftemperature 
 * fhumidity
 * effvals -> number of HIGH intervals, for debug purpose
 * 
 * RETURN VALUE:
 * -1 -> Insufficient number of HIGH intervals
 * -2 -> Too many HIGH intervals
 * -3 -> No sync, case with 83 HIGH intervals 
 * -4 -> No sync, case with 84 HIGH intervals
 * -5 -> Parity check error
 * -6 -> WiringPI initializing error
*/
{
	uint16_t i;
	uint8_t  calcparity;
	uint8_t  j;
	uint8_t  k;
	uint8_t  jndex;
	uint8_t  firstbitpos;
	uint8_t  samples[DHTxx_MAX_TIMINGS];
	uint16_t values[DHTxx_MAXVALS][2];
	uint8_t  data[5] = {0,0,0,0,0};
	int sign;

	if ( wiringPiSetup() == -1 )
		return -6;
	
	/* activating phase */

	/* pull down pin for 1 milliseconds */
	pinMode( dhtpin, OUTPUT );
	digitalWrite( dhtpin, LOW );
	delay( 1 );
	
	/* pull up pin for 30 microseconds */
	digitalWrite( dhtpin, HIGH );
	delayMicroseconds( 30 );

	/* prepare to read the pin */
	pinMode( dhtpin, INPUT );

	/* sampling phase */
	for ( i = 0; i < DHTxx_MAX_TIMINGS; i++ ) {
		samples[i]=digitalRead( dhtpin );
		delayMicroseconds( 1 );
	}
	
	/* compacting */
	j=0;
	values[j][0]=samples[0];
	values[j][1]=0;
	for (i=0; i<DHTxx_MAX_TIMINGS; i++) {
		if (values[j][0] != samples[i]) {
			if (j>=DHTxx_MAXVALS)
			  return -2;
			j++;
			values[j][0]=samples[i];
			values[j][1]=0;
		}
		(values[j][1])++;
	}
	*effvals=j;
	/* !! warning: effvals contains 1 minus the states */
	
	/* calculates & checks */
	if (*effvals < 82) {
		/* less than 83 states */
		return -1;
	}
	else {
		if (*effvals > 83) {
			/* more than 84 states */
			return -2;
		}
		else {
			if (*effvals == 82) {
				/* can be, but first state should be HIGH
				 * this is the case when the first low transition has
				 * been lost
				*/
				if (values[0][0] != HIGH) {
					/* something wrong with 83 transitions */
					return -3;
				}
				else {
					/* bit flow starts at positions 3 */
					firstbitpos=2;
				}
			}
			else {
				/* let me check if the first state is LOW */
				if (values[0][0] != LOW ) {
					/* something strange */
					return -4;
				}
				else {
					/* bit flow starts at positions 3
					 * this is the normal situation 
				    */
				    firstbitpos=3;
				}
			}
		}
	}
	/* here only if bit flow seems to be ok
	 * bit flow starts at firstbitpos
	*/
	jndex=firstbitpos;
	for (j=0; j<5; j++) {
		for (k=0; k<8; k++) {
			data[j]<<=1;
			if (values[jndex][1] > 30) {
				data[j]+=1;
				/* useless to add 0 ;-) */
			}
			jndex+=2;
		}
	}
	
	/*
	 * check parity 
	*/
	calcparity=0;
	
	calcparity=data[0] + data[1] + data[2] + data[3];
	if (calcparity != data[4]) {
		/* parity error */
		/*
		printf("data[0]: %d\n", data[0]);
		printf("data[1]: %d\n", data[1]);
		printf("data[2]: %d\n", data[2]);
		printf("data[3]: %d\n", data[3]);
		printf("data[4]   : %d\n", data[4]);
		printf("calcparity: %d\n", calcparity);
		*/
		return -5;
	}
	
	if ( data[2] & 0x80 ) {
		/* temp is negative */
		sign=-1;
		data[2] &= 0x7F;
	}
	else {
		sign=1;
	}

	
	*fhumidity=(float) (data[0]*256+data[1]) / 10;
	*ftemperature=(float) sign*(data[2]*256+data[3]) / 10;

	
	return 3-firstbitpos; /* 0 all ok, 1 recovered */
} /* DHTxx_read_data */

int DHTxx_read_m_data(uint8_t dhtpin, float *ftemperature, float *fhumidity, uint8_t retries) {
/*
 * Executes "retries" time a reading using the "read_dht_data" function
 * returned value for temp and hum are an average of successfully readed values
 * 
 * ftemperature (out)
 * fhumidity (out)
 * retries (in) -> number of reading from sensor, min 1
 * RETURN VALUE:
 * -1 -> some error
 *  0 -> OK
*/
	uint8_t effretries;
	uint8_t i;
	uint8_t j;
	uint8_t goodread;
	float *ftemps;
	float *fhums;
	float fworktemp;
	float fworkhum;
	uint8_t  effvals;
	int retval;
	
	if (retries < 1) {
		effretries=DHTxx_MINRETRIES;
	}
	else {
		effretries=retries;
	}
	
	/* allocate enough space */
	ftemps=malloc(retries*sizeof(float));
	fhums=malloc(retries*sizeof(float));
	
	goodread=0;
	j=0;
	for (i=0; i<effretries; i++) {
		retval=DHTxx_read_data(dhtpin, &fworktemp, &fworkhum, &effvals);
		if (retval >= 0) {
			goodread=1;
			ftemps[j]=fworktemp;
			fhums[j]=fworkhum;
			j++;
		}
		if (i<effretries-1) {
			delay(DHTxx_MINDELAYTIME);
		}
		/* else it is unuseful to wait */
	}
	if (goodread == 1) {
		*ftemperature=0;
		*fhumidity=0;
		for (i=0; i<j; i++) {
			*ftemperature+=ftemps[i];
			*fhumidity+=fhums[i];
		}
		*ftemperature=*ftemperature/j;
		*fhumidity=*fhumidity/j;
		return 0;
	}
	else
	{
		return -1;
	}
	
} /* DHTxx_read_m_data */

