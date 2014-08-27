#include "Ds3234.h"
#include <WProgram.h>
#include <inttypes.h>
#include <SPI.h>

#define SEC_ADR 	0x00
#define MINUTE_ADR 	0x01
#define HOUR_ADR 	0x02
#define DATE_ADR 	0x04
#define MONTH_ADR 	0x05
#define YEAR_ADR 	0x06
#define A1_SEC_ADR 	0x07
#define A1_MINUTE_ADR 	0x08
#define A1_HOUR_ADR 0x09
#define TEMP_ADR 	0x11

// =======================================
Ds3234::Ds3234(int csPin) {
    SPI_CS=csPin;
	pinMode(SPI_CS,OUTPUT); // chip select
	// start the SPI library:
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE3); // both mode 1 & 3 should work
	//set control register
	digitalWrite(SPI_CS, LOW);
	SPI.transfer(0x8E);
	SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
	digitalWrite(SPI_CS, HIGH);
	//delay(10);
}
//=====================================
uint8_t Ds3234::decToBcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

uint8_t Ds3234::bcdToDec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}

uint8_t Ds3234::bcdH(uint8_t val) {
	return val / 16;
}

uint8_t Ds3234::bcdL(uint8_t val) {
	return val % 16;
}

uint8_t Ds3234::h24ToH12(uint8_t val) {
	if (val == 0) return 12;
	if (val > 12) return val - 12;
	return val;
}

uint8_t Ds3234::h24AmPm(uint8_t val) {
	if (val >= 12) return 1;
	return 0;
}

uint8_t Ds3234::h12ToH24(uint8_t val, uint8_t ampm) {
	if (ampm == 0) {
		if (val == 12) return 0;
		else return val;
	} else {
		if (val == 12) return 12;
		else return val + 12;
	}
}

uint8_t Ds3234::GetSpiData(uint8_t val) {
	digitalWrite(SPI_CS, LOW);
	SPI.transfer(val);
	uint8_t ret = SPI.transfer(0x00);
	digitalWrite(SPI_CS, HIGH);
	return ret;
}

void Ds3234::SetSpiData(uint8_t address, uint8_t val) {
	digitalWrite(SPI_CS, LOW);
	SPI.transfer(address);
	SPI.transfer(val);
	digitalWrite(SPI_CS, HIGH);
}

// 現在の時間を12時間制で返す
void Ds3234::GetTime(uint8_t &ampm, uint8_t &hour, uint8_t &minute, uint8_t &sec) {
	uint8_t val, n, valHigh, valLow;
	val = GetSpiData(HOUR_ADR);
	if (bitRead(val, 6) == 0) {		//	RTCは24時間モード
		n = bcdToDec(val & B00111111);
		ampm = h24AmPm(n);
		hour = h24ToH12(n);
	} else {						// RTCは12時間モード
		ampm = bitRead(val, 5);
		hour = bcdToDec(val & B00011111);
	}

	minute = bcdToDec(GetSpiData(MINUTE_ADR));
	sec = bcdToDec(GetSpiData(SEC_ADR));
}

// 12時間制の時間をRTCにセット
void Ds3234::SetTime(uint8_t ampm, uint8_t hour, uint8_t minute, uint8_t sec) {
	uint8_t val;
	
	if (bitRead(GetSpiData(HOUR_ADR), 6) == 0) {	// RTCは24時間モード
		val = decToBcd(h12ToH24(hour, ampm));
	} else {										// RTCは12時間モード
		val = decToBcd(hour);
		if (ampm > 0) {
			val |= B01100000;
		} else {
			val |= B01000000;
		}
	}
	SetSpiData(HOUR_ADR + 0x80, val);
	
	// Min set
	SetSpiData(MINUTE_ADR + 0x80, decToBcd(minute));
	// Sec set
	SetSpiData(SEC_ADR + 0x80, decToBcd(sec));
}
// 現在の時間を24時間制で返す
void Ds3234::GetTime(uint8_t &hour, uint8_t &minute, uint8_t &sec) {
	uint8_t val, n, valHigh, valLow;
	val = GetSpiData(HOUR_ADR);
	if (bitRead(val, 6) == 0) {		// RTCは24時間モード
		hour = bcdToDec(val & B00111111);
	} else {						// RTCは12時間モード
		hour = h12ToH24(bcdToDec(val & B00011111), bitRead(val, 5));
	}
	
	minute = bcdToDec(GetSpiData(MINUTE_ADR));
	sec = bcdToDec(GetSpiData(SEC_ADR));
}
// 24時間制の時間をRTCに設定
void Ds3234::SetTime(uint8_t hour, uint8_t minute, uint8_t sec) {
	uint8_t val;
	
	if (bitRead(GetSpiData(HOUR_ADR), 6) == 0) {	// RTCは24時間モード
		val = decToBcd(hour);
	} else {										// RTCは12時間モード
		val = decToBcd(h24ToH12(hour));
		if (h24AmPm(hour) > 0) {
			val |= B01100000;
		} else {
			val |= B01000000;
		}
	}
	SetSpiData(HOUR_ADR + 0x80, val);
	
	// Min set
	SetSpiData(MINUTE_ADR + 0x80, decToBcd(minute));
	// Sec set
	SetSpiData(SEC_ADR + 0x80, decToBcd(sec));
}

// 24時間制と12時間制を切り替え
void Ds3234::ToggleH24H12() {
	uint8_t val, n;
	n = GetSpiData(HOUR_ADR);
	if (bitRead(n, 6) == 0) {	// RTCは24時間モード
		val = bcdToDec(h24ToH12(n));
		if (h24AmPm(n) == 0) {
			val |= B01000000;
		} else {
			val |= B01100000;
		}
	} else {					// RTCは12時間モード
		val = decToBcd(h12ToH24(bcdToDec(n & B00011111), bitRead(n, 5)));
	}
	SetSpiData(HOUR_ADR + 0x80, val);
}  
// 引数（12 or 24）を指定して12時間制と24時間制を切り替え
void Ds3234::SetHourMode(int mode) {
	uint8_t val, n;
	n = GetSpiData(HOUR_ADR);
	if (bitRead(n, 6) == 0) {	// RTCは24時間モードを12時間モードへ
		if (mode == 12) {
			val = bcdToDec(h24ToH12(n));
			if (h24AmPm(n) == 0) {
				val |= B01000000;
			} else {
				val |= B01100000;
			}
			SetSpiData(HOUR_ADR + 0x80, val);
		}
	} else {					// RTCは12時間モード
		if (mode == 24) {
			val = decToBcd(h12ToH24(bcdToDec(n & B00011111), bitRead(n, 5)));
			SetSpiData(HOUR_ADR + 0x80, val);
		}
	}
}  

// 24時間モードは２４，12時間モードは１２を返す
int Ds3234::GetHourMode() {
	if (bitRead(GetSpiData(HOUR_ADR), 6) == 0) return 24;
	else return 12;
}
// 秒をゼロに
void Ds3234::ResetSec() {
	SetSpiData(SEC_ADR + 0x80, 0);
}

// 日付の取得
void Ds3234::GetDate(uint8_t &year, uint8_t &month, uint8_t &date) {
	year  = bcdToDec(GetSpiData(YEAR_ADR));
	month = bcdToDec(GetSpiData(MONTH_ADR) & 0x1f);
	date  = bcdToDec(GetSpiData(DATE_ADR));
}
// 日付の設定
void Ds3234::SetDate(uint8_t year, uint8_t month, uint8_t date) {
	SetSpiData(YEAR_ADR + 0x80, decToBcd(year));
	SetSpiData(MONTH_ADR + 0x80, decToBcd(month));
	SetSpiData(DATE_ADR + 0x80, decToBcd(date));
}

int Ds3234::GetTemp() {
	uint8_t val = GetSpiData(TEMP_ADR);
	if (bitRead(val, 7) == 0)
		return val & B01111111;
	else
		return  (val & B01111111) * -1;
}