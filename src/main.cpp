#include <SPIFFS.h>
#include "M5Dial.h"

const char *fileName[] = {
    "/pocky.jpg",
    "/namae.jpg",
    "/QR_X.jpg",
    "/QR_F.jpg"
};
unsigned long previousMillis = 0;   // 前回の時間保存用
uint16_t fileNum = sizeof(fileName) / sizeof(char*);
int16_t fileSel = 0;
int32_t oldPosition = 0;
enum  {
    MANUAL = 0,
    AUTO
};
uint16_t mode = AUTO;
//===========================================================
//	drawFile() ： Draw jpg File to LCD
//===========================================================
bool drawFile(const char *filename)
{
  bool ret = false;
	int	err_count = 0;

	delay(1000);
	err_count = 0;

	do {
//		ret = M5Dial.Display.drawJpgFile(SPIFFS, filename, 0, 0, 240, 240, 0, 0, JPEG_DIV_MAX);
		ret = M5Dial.Display.drawJpgFile(SPIFFS, filename, 0, 0, 240, 240, 0, 0);
		if (err_count++ > 10) {
    	    Serial.println("File not EXIST!");
			break;
		}
	} while (!ret);

	return ret;
}


// setup関数は起動時に一度だけ実行されます。
// 主に初期化処理を記述します。
void setup() {

    auto cfg = M5.config();       // M5Stack初期設定用の構造体を代入
    M5Dial.begin(cfg, true, false);
    //M5.begin(cfg);                           // M5デバイスの初期化
    SPIFFS.begin();
    drawFile(fileName[0]);
}

// loop関数は起動している間ずっと繰り返し実行されます。
// センサーから値を取得したり、画面を書き換える動作等をおこないます。
void loop() 
{
    unsigned long currentMillis = millis();
    M5Dial.update();
    if (M5Dial.BtnA.wasPressed()) {
        if (mode == AUTO) {
            M5Dial.Speaker.tone(4000, 20);
            delay(20);
            M5Dial.Speaker.tone(4000, 20);
            mode = MANUAL;
        } else {
            M5Dial.Speaker.tone(4000, 20);
            mode = AUTO;
        }
    }

    if (mode == MANUAL) {
        int32_t newPosition = M5Dial.Encoder.read();
        if (abs(newPosition - oldPosition) > 3) {
            M5Dial.Speaker.tone(8000, 20);
            //Serial.printf("pos = %04d", newPosition);
            if (newPosition > oldPosition) {
                fileSel = (++fileSel >= fileNum) ? 0 : fileSel;
            } else {
                fileSel = (--fileSel < 0) ? fileNum - 1 : fileSel;
            }
            oldPosition = newPosition;
            //Serial.printf("pos = %04d\n", fileSel);
            drawFile(fileName[fileSel]);
        }
    } else {
        if (currentMillis - previousMillis >= 10000) {
            fileSel = (fileSel >= fileNum - 1) ?  0 : fileSel + 1;
            drawFile(fileName[fileSel]);
            previousMillis = currentMillis;
        }
    }
    delay(1);
}

