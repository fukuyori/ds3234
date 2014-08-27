#include <WProgram.h>
#include <inttypes.h>
#include <SPI.h>

class Ds3234 {
	private:
    	int SPI_CS;
    	uint8_t decToBcd(uint8_t);
		uint8_t bcdToDec(uint8_t);
		uint8_t bcdH(uint8_t);
		uint8_t bcdL(uint8_t);

		uint8_t h24ToH12(uint8_t);
		uint8_t h24AmPm(uint8_t);
		uint8_t h12ToH24(uint8_t, uint8_t);
		uint8_t GetSpiData(uint8_t);
		void SetSpiData(uint8_t, uint8_t);
	
	public: 
		// 初期化。引数はCSピン
		Ds3234(int);
		// 12時間制で取得・設定 ampm, hour, minute, sec
		void GetTime(byte&, byte&, byte&, byte&);
		void SetTime(byte, byte, byte, byte);
		// 24時間制で取得・設定 hour, minute, sec
		void GetTime(byte&, byte&, byte&);
		void SetTime(byte, byte, byte);
		// RTC内の24時間制、12時間制の変更
		//　引数ない場合は、切り替え
		void ToggleH24H12();
		//　引数（12 or 24）をつけると、12時間モード、24時間モードを指定
		void SetHourMode(int);
		// RTC内部の24時間制、12時間制のどちらで動いているかを取得
		int GetHourMode();
		// 秒を０にする
		void ResetSec();
		// 日付の取得・設定 year, month, date
		void GetDate(uint8_t&, uint8_t&, uint8_t&);
		void SetDate(uint8_t, uint8_t, uint8_t);
		int GetTemp();
};
