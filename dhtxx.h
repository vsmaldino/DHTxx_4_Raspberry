#include <wiringPi.h>
/* included to supply some utility function like delay */


int DHTxx_read_m_data(uint8_t dhtpin, float *ftemperature, float *fhumidity, uint8_t retries);
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

int DHTxx_read_data(uint8_t dhtpin, float *ftemperature, float *fhumidity, uint8_t *effvals);
/*
 * dhtpin  -> Pin number using WiringPi numeration
 * ftemperature (out)
 * fhumidity (out)
 * effvals (out) -> number of HIGH intervals, for debug purpose
 * 
 * RETURN VALUE:
 * -1 -> Insufficient number of HIGH intervals
 * -2 -> Too many HIGH intervals
 * -3 -> No sync, case with 83 HIGH intervals 
 * -4 -> No sync, case with 84 HIGH intervals
 * -5 -> Parity check error
*/

