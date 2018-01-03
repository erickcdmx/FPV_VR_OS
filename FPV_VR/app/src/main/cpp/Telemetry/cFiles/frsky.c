
#include "frsky.h"
#include "tID.h"

int frsky_parse_buffer(frsky_state_t *state, float td[], uint8_t *buf, int buflen) {
	int new_data = 0;
	int i;

	for(i=0; i<buflen; ++i) {
		uint8_t ch = buf[i];
		switch(state->sm_state) {
			case 0:
				if(ch == 0x5e)
					state->sm_state = 1;
				break;
			case 1:
				if(ch == 0x5e)
					state->sm_state = 2;
				else
					state->sm_state = 0;
				break;
			case 2:
				if(ch == 0x5e) {
					state->pkg_pos = 0;
					new_data = new_data | frsky_interpret_packet(state, td);
				}
				else {
					if(state->pkg_pos >= sizeof(state->pkg)) {
						state->pkg_pos = 0;
						state->sm_state = 0;
					} else {
						state->pkg[state->pkg_pos] = ch;
						state->pkg_pos++;
					}
				}
				break;
			default:
				state->sm_state = 0;
				break;
		}
	}
	return new_data;
}

int frsky_interpret_packet(frsky_state_t *state, float td[]) {
	uint16_t data;
	int new_data = 1;

	data = *(uint16_t*)(state->pkg+1);
	switch(state->pkg[0]) {
		case ID_VOLTAGE_AMP:
			td[ID_NValidR]++;
			td[ID_BATT_V] = data / 10.0f;
			//printf("voltage:%f  ", td->voltage);
			break;
		case ID_ALTITUDE_BP:
			td[ID_NValidR]++;
			td[ID_HEIGHT_B] = data;
			//printf("baro altitude BP:%f  ", td->baro_altitude);
			break;
		case ID_ALTITUDE_AP:
			//td->baro_altitude += data/100;
			//printf("Baro Altitude AP:%f  ", td->baro_altitude);
			break;
		case ID_GPS_ALTITUDE_BP:
			td[ID_NValidR]++;
			td[ID_HEIGHT_GPS] = data;
			//printf("GPS altitude:%f  ", td->altitude);
			break;
		case ID_LONGITUDE_BP:
			td[ID_NValidR]++;
			td[ID_LON] = data / 100;
			td[ID_LON] += 1.0 * (data - td[ID_LON] * 100) / 60;
			//printf("longitude BP:%f  ", td[ID_LON]);
			break;
		case ID_LONGITUDE_AP:
			td[ID_NValidR]++;
			td[ID_LON] +=  1.0 * data / 60 / 10000;
			//printf("longitude AP:%f  ", td[ID_LON]);
			break;
		case ID_LATITUDE_BP:
			td[ID_NValidR]++;
			td[ID_LAT] = data / 100;
			td[ID_LAT] += 1.0 * (data - td[ID_LAT] * 100) / 60;
			//printf("latitude BP:%f  ", td->latitude);
			break;
		case ID_LATITUDE_AP:
			td[ID_NValidR]++;
			td[ID_LAT] +=  1.0 * data / 60 / 10000;
			//printf("latitude AP:%f  ", td[ID_LAT]);
			break;
		case ID_COURSE_BP:
			td[ID_NValidR]++;
			td[ID_YAW] = data;
			//printf("heading:%f  ", td->heading);
			break;
		case ID_GPS_SPEED_BP:
			td[ID_NValidR]++;
			td[ID_VS] = (float)(1.0 * data / 0.0194384449);
			//printf("GPS speed BP:%f  ", td->speed);
			break;
		case ID_GPS_SPEED_AP:
			td[ID_NValidR]++;
			td[ID_VS] += 1.0 * data / 1.94384449; //now we are in cm/s
			td[ID_VS] = td[ID_VS] / 100 / 1000 * 3600; //now we are in km/h
			//printf("GPS speed AP:%f  ", td->speed);
			break;
		case ID_ACC_X:
			td[ID_NValidR]++;
			td[ID_ROLL] = data;
			//printf("accel X:%d  ", td->x);
			break;
		case ID_ACC_Y:
			td[ID_NValidR]++;
			td[ID_PITCH] = data;
			//printf("accel Y:%d  ", td->y);
			break;
		/*case ID_ACC_Z:
			td[ID_NValidR]++;
			td->z = data;
			printf("accel Z:%d  ", td->z);
			break;*/
		case ID_E_W:
			td[ID_NValidR]++;
			td[ID_UNKNOWN] = data;
			//printf("E/W:%d  ", td->ew);
			break;
		case ID_N_S:
			td[ID_NValidR]++;
			td[ID_UNKNOWN] = data;
			//printf("N/S:%d  ", td->ns);
			break;
		default:
			new_data = 0;
			//printf("%x\n", pkg[0]);
			break;
	}
	return new_data;
}

int frsky_read(float telemetryD[], uint8_t *buf, int buflen){
	return frsky_parse_buffer(&state,telemetryD,buf,buflen);
}
