//  denshi meishi 
//  Ver 1.0 2023.10.30 K.Ohe
#include <FS.h>
#include <SPIFFS.h>
#include "M5Dial.h"

#define MAX_FILES 100

String fileName[MAX_FILES];

unsigned long previousMillis = 0;   // 前回の時間保存用
//uint16_t fileNum = sizeof(fileName) / sizeof(char*);
uint16_t fileNum = 0;
int16_t fileSel  = 0;
int32_t oldPosition = 0;
enum  {
    MANUAL = 0,
    AUTO
};
uint16_t mode = AUTO;

//===========================================================
//	getFileNames() ： get File Name From SPIFFS
//===========================================================
int getFileNames()
{
    int count = 0;
    File root = SPIFFS.open("/");
    File fName = root.openNextFile();
    while (fName) {
        String file = fName.name();
        if (file.endsWith(".jpg") || file.endsWith(".jpeg")) {
            fileName[count] = "/" + file;
            count++;
        if (count >= MAX_FILES) break;
        }
        fName = root.openNextFile();
    }
    for (int i = 0; i < count; i++) {
        Serial.println(fileName[i]);
    }
    return count;
}

//===========================================================
//	drawFile() ： Draw jpg File to LCD
//===========================================================
bool drawFile(String filename)
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

//===========================================================
//	setup() ： Arduino Setup
//===========================================================
void setup() {
    Serial.begin(115200);
    auto cfg = M5.config();       // M5Stack初期設定用の構造体を代入
    M5Dial.begin(cfg, true, false);
    //M5.begin(cfg);                           // M5デバイスの初期化
    SPIFFS.begin();
    fileNum = getFileNames();
    if (fileNum == 0) {
        Serial.println("jpeg file not found");
        while(true);
    }
    drawFile(fileName[0]);
}

//===========================================================
//	loop() ： Arduino main loop
//===========================================================
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

