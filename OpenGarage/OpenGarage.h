/* OpenGarage Firmware
 *
 * OpenGarage library header file
 * Mar 2016 @ OpenGarage.io
 *
 * This file is part of the OpenGarage library
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _OPENGARAGE_H
#define _OPENGARAGE_H

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <DHTesp.h>
#include <AM2320.h>
#include <DallasTemperature.h>

#include "defines.h"

struct OptionStruct {
	String name;
	uint ival;
	uint max;
	String sval;
};

struct LogStruct {
	ulong tstamp;	// time stamp
	uint status;	// door status
	uint dist;		// distance
	byte sn2;			// switch sensor value
};

class OpenGarage {

public:
	static OptionStruct options[];
	static byte state;
	static byte alarm;
	static byte led_reverse;
	static ulong previous_millis;
	static Ticker relay_ticker;
	static int double_click_delay_ms;
	static int alarm_double_click_delay_ms;

	static void begin();
	static void options_setup();
	static void options_load();
	static void options_save();
	static void options_reset();
	static void restart() { ESP.restart();} //digitalWrite(PIN_RESET, LOW); }
	static uint read_distance(); // centimeter
	static void init_sensors(); // initialize all sensor
	static void read_TH_sensor(float& C, float &H);
	static byte get_mode()   { return options[OPTION_MOD].ival; }
	static byte get_button() { return digitalRead(PIN_BUTTON); }
	static byte get_switch() { pinMode(PIN_SWITCH, INPUT_PULLUP); return digitalRead(PIN_SWITCH); }
	static byte get_led()    { return led_reverse?(!digitalRead(PIN_LED)):digitalRead(PIN_LED); }
	static void set_led(byte status)   { digitalWrite(PIN_LED, led_reverse?(!status):status); }
	static void set_relay(byte status) { digitalWrite(PIN_RELAY, status); }

	static bool click_relay() {
		// don't allow a click if we are already in the middle of one, unless it's the second click of a double-click
		if(previous_millis != 0){
			DEBUG_PRINT(F("Relay already open: "));
			DEBUG_PRINT(previous_millis);
			DEBUG_PRINT(F(", "));
			DEBUG_PRINTLN(double_click_delay_ms);
			return false;
		}
		unguarded_click_relay();
		return true;
	}

	static bool double_click_relay(uint double_click_delay) {
		DEBUG_PRINT(F("Start Double-Click: "));
		DEBUG_PRINTLN(double_click_delay);
		// total double-click activity time is: click delay + double click delay + click delay
		// We don't know exactly when the garage starts opening, so let's assume it's halfway through the first click delay
		uint temp = double_click_delay - options[OPTION_CDT].ival/2;
		if(temp > double_click_delay){ // temp is a uint, so if it's bigger than double-click-delay, it's because we went negative
			DEBUG_PRINT(F("Double-Click Time Error: "));
			DEBUG_PRINTLN(temp);
			return false;
		}
		double_click_delay_ms = temp;
		return click_relay();
	}
	static int find_option(String name);
	static void log_reset();
	static void write_log(const LogStruct& data);
	static bool read_log_start();
	static bool read_log_next(LogStruct& data);
	static bool read_log_end();
	static void play_note(uint freq);
	
	static void set_alarm(byte ov=0, uint alarm_double_click_delay_ms=-1) { // ov = override value
		if(ov) alarm = ov*10+1;
		else alarm = options[OPTION_ALM].ival * 10 + 1;
		OpenGarage::alarm_double_click_delay_ms = alarm_double_click_delay_ms;
	}
	
	static void finish_alarm_click_relay() {
		if(OpenGarage::alarm_double_click_delay_ms != -1){
			double_click_relay(OpenGarage::alarm_double_click_delay_ms);
		} else{
			click_relay();
		}
	}

	static void reset_to_ap() {
		options[OPTION_MOD].ival = OG_MOD_AP;
		options_save();
		restart();
	}
	static void config_ip();
	static void play_startup_tune();

private:
	static File log_file;
	static void button_handler();
	static void led_handler();

	static void unguarded_click_relay() {
		previous_millis = millis();
		set_relay(HIGH);
		relay_ticker.once_ms(options[OPTION_CDT].ival, finish_relay);
	}

	static void finish_relay(){
		set_relay(LOW);
		if(double_click_delay_ms < 0){
			previous_millis = 0;
		}
		else{
			DEBUG_PRINT(F("Finish Double-Click: "));
			DEBUG_PRINT(double_click_delay_ms);
			DEBUG_PRINT(F(", "));
			DEBUG_PRINTLN(previous_millis);
			uint temp = double_click_delay_ms;
			double_click_delay_ms = -1;
			relay_ticker.once_ms(temp, unguarded_click_relay);
		}
	}

	static DallasTemperature *ds18b20;
	static AM2320* am2320;
	static DHTesp* dht;
};

#endif  // _OPENGARAGE_H_
