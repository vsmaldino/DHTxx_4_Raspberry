# DHTxx_4_Raspberry
Library for reading for reading Temperature and Humidity from DHT11/22 specifically written for Raspberry.

Why this library?
Similar libraries available on the Web are simply porting from Arduino code, but Raspberry DIFFERS from Arduino!

Raspberry is a multitask system, this is a major problem when dealing with serial line timings as in the case of DHT11/22.

Reading a single bit from DHT11/22 takes from about 80 microseconds (bit 0) to about 120 microseconds (bit 1); reading the entire 40 bit packet requires about 4 milliseconds.

The goal of this library is to supply a reasonably reliable function for reading data from the sensor.

To achieve this goal, unlike what happens in the Arduino code, the sampling phase was rewritten and was made as simple as possible in order to minimize the interrupt risk. ONly at the end of sampling phase, bit recognition starts.

There are 2 function available:

DHTxx_read_data. Which executes a SINGLE data read from the sensor.

DHTxx_read_m_data. Which eexecutes MULTPLE data read from the sensor, returning average values for Temperature and Humidity
