#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <wiringPiI2C.h>
#include "capteur.h"

double cpuTemp() {

	FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	double T;

	if (fp == NULL) {
		perror("Unable to open file!");
		exit(1);
	}
	fscanf (fp, "%lf", &T);
	T /= 1000;

	fclose(fp);
	return T;
}

double cpuFreq() {

	FILE *fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
	double F;

	if (fp == NULL) {
		perror("Unable to open file!");
		exit(1);
	}
	fscanf (fp, "%lf", &F);
	F /= 1000;

	fclose(fp);
	return F;
}


const char* bme280() {


	int fd = wiringPiI2CSetup(BME280_ADDRESS);
	if (fd < 0) {
		printf("Device not found");
		return -1;
	}

	bme280_calib_data cal;
	readCalibrationData(fd, &cal);

	wiringPiI2CWriteReg8(fd, 0xf2, 0x01);   // humidity oversampling x 1
	wiringPiI2CWriteReg8(fd, 0xf4, 0x25); // pressure and temperature oversampling x 1, mode normal

	bme280_raw_data raw;
	getRawData(fd, &raw);

	int32_t t_fine = getTemperatureCalibration(&cal, raw.temperature);
	float t = compensateTemperature(t_fine); // C
	float p = compensatePressure(raw.pressure, &cal, t_fine) / 100; // hPa
	float h = compensateHumidity(raw.humidity, &cal, t_fine);       // %
	float a = getAltitude(p);                         // meters

	char *datas = malloc(255);
//	    snprintf(datas, sizeof(datas), "{\"sensor\":\"bme280\", \"humidity\":%.2f, \"pressure\":%.2f,"
//				" \"temperature\":%.2f, \"altitude\":%.2f}\n", h, p, t, a);


	sprintf(datas,
			"{\"sensor\":\"bme280\", \"humidity\":%.2f, \"pressure\":%.2f,"
					" \"temperature\":%.2f, \"altitude\":%.2f}\n", h, p, t, a);

	return datas;
}

int32_t getTemperatureCalibration(bme280_calib_data *cal, int32_t adc_T) {
	int32_t var1 = ((((adc_T >> 3) - ((int32_t) cal->dig_T1 << 1)))
			* ((int32_t) cal->dig_T2)) >> 11;

	int32_t var2 = (((((adc_T >> 4) - ((int32_t) cal->dig_T1))
			* ((adc_T >> 4) - ((int32_t) cal->dig_T1))) >> 12)
			* ((int32_t) cal->dig_T3)) >> 14;

	return var1 + var2;
}

void readCalibrationData(int fd, bme280_calib_data *data) {
	data->dig_T1 = (uint16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T1);
	data->dig_T2 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T2);
	data->dig_T3 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_T3);

	data->dig_P1 = (uint16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P1);
	data->dig_P2 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P2);
	data->dig_P3 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P3);
	data->dig_P4 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P4);
	data->dig_P5 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P5);
	data->dig_P6 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P6);
	data->dig_P7 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P7);
	data->dig_P8 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P8);
	data->dig_P9 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_P9);

	data->dig_H1 = (uint8_t) wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H1);
	data->dig_H2 = (int16_t) wiringPiI2CReadReg16(fd, BME280_REGISTER_DIG_H2);
	data->dig_H3 = (uint8_t) wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H3);
	data->dig_H4 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4) << 4)
			| (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H4 + 1) & 0xF);
	data->dig_H5 = (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5 + 1) << 4)
			| (wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H5) >> 4);
	data->dig_H6 = (int8_t) wiringPiI2CReadReg8(fd, BME280_REGISTER_DIG_H6);
}

float compensateTemperature(int32_t t_fine) {
	float T = (t_fine * 5 + 128) >> 8;
	return T / 100;
}

float compensatePressure(int32_t adc_P, bme280_calib_data *cal, int32_t t_fine) {
	int64_t var1, var2, p;

	var1 = ((int64_t) t_fine) - 128000;
	var2 = var1 * var1 * (int64_t) cal->dig_P6;
	var2 = var2 + ((var1 * (int64_t) cal->dig_P5) << 17);
	var2 = var2 + (((int64_t) cal->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) cal->dig_P3) >> 8)
			+ ((var1 * (int64_t) cal->dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) cal->dig_P1) >> 33;

	if (var1 == 0) {
		return 0;  // avoid exception caused by division by zero
	}
	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t) cal->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) cal->dig_P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t) cal->dig_P7) << 4);
	return (float) p / 256;
}

float compensateHumidity(int32_t adc_H, bme280_calib_data *cal, int32_t t_fine) {
	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t) 76800));

	v_x1_u32r = (((((adc_H << 14) - (((int32_t) cal->dig_H4) << 20)
			- (((int32_t) cal->dig_H5) * v_x1_u32r)) + ((int32_t) 16384)) >> 15)
			* (((((((v_x1_u32r * ((int32_t) cal->dig_H6)) >> 10)
					* (((v_x1_u32r * ((int32_t) cal->dig_H3)) >> 11)
							+ ((int32_t) 32768))) >> 10) + ((int32_t) 2097152))
					* ((int32_t) cal->dig_H2) + 8192) >> 14));

	v_x1_u32r = (v_x1_u32r
			- (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
					* ((int32_t) cal->dig_H1)) >> 4));

	v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
	v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	float h = (v_x1_u32r >> 12);
	return h / 1024.0;
}

void getRawData(int fd, bme280_raw_data *raw) {
	wiringPiI2CWrite(fd, 0xf7);

	raw->pmsb = wiringPiI2CRead(fd);
	raw->plsb = wiringPiI2CRead(fd);
	raw->pxsb = wiringPiI2CRead(fd);

	raw->tmsb = wiringPiI2CRead(fd);
	raw->tlsb = wiringPiI2CRead(fd);
	raw->txsb = wiringPiI2CRead(fd);

	raw->hmsb = wiringPiI2CRead(fd);
	raw->hlsb = wiringPiI2CRead(fd);

	raw->temperature = 0;
	raw->temperature = (raw->temperature | raw->tmsb) << 8;
	raw->temperature = (raw->temperature | raw->tlsb) << 8;
	raw->temperature = (raw->temperature | raw->txsb) >> 4;

	raw->pressure = 0;
	raw->pressure = (raw->pressure | raw->pmsb) << 8;
	raw->pressure = (raw->pressure | raw->plsb) << 8;
	raw->pressure = (raw->pressure | raw->pxsb) >> 4;

	raw->humidity = 0;
	raw->humidity = (raw->humidity | raw->hmsb) << 8;
	raw->humidity = (raw->humidity | raw->hlsb);
}

float getAltitude(float pressure) {
	// Equation taken from BMP180 datasheet (page 16):
	//  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

	// Note that using the equation from wikipedia can give bad results
	// at high altitude.  See this thread for more information:
	//  http://forums.adafruit.com/viewtopic.php?f=22&t=58064

	return 44330.0
			* (1.0 - pow(pressure / MEAN_SEA_LEVEL_PRESSURE, 0.190294957));
}

