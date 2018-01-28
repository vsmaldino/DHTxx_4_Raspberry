#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "dhtxx.h"

#define DHT_PIN 7 /* GPIO04 */

int main() {
	float    ftemperature;
	float    fhumidity;
	/* uint8_t  effvals; */
	int retval;
	time_t timer;
	char buffer[26];
	struct tm * tm_info;
	
	printf( "Raspberry Pi DHT11/DHT22 temperature/humidity test\n" );
	
	while (1) {
		/* retval=DHTxx_read_data(&ftemperature, &fhumidity, &effvals); */
		retval=DHTxx_read_m_data(DHT_PIN, &ftemperature, &fhumidity, 4);
		if (retval >= 0) {
			time(&timer);
			tm_info = localtime(&timer);
			strftime(buffer, 26, "%d/%m/%Y %H:%M:%S", tm_info);
			
			printf("%s ", buffer);
			printf("%3.1f ", fhumidity);
			printf("%3.1f\n", ftemperature);
		}
		delay( 15000 );
	}	

	return 0;
} /* main */
