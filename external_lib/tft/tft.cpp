#include "pico/time.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ST7735_TFT.hpp"
#include "tft.h"
#include "stdlib.h"

ST7735_TFT myTFT;

#define MAX_BIT_DEPTH 4
// #define NUMBER 0
// #define ASCII 1


// Функция преобразования числа в массив его цифр в виде uint или ASCII - символов 
// static void num_into_digits(uint16_t num, uint8_t * digits, uint8_t bit_depth, bool output_type){
// 	for(int i = 0; i < bit_depth; i++){
// 		switch (output_type){
// 		case NUMBER:
// 			digits[bit_depth-1-i] = num % 10;
// 			break;
// 		case ASCII:
// 		//Для преобразования из uint к ascii добавляем постоянное смещение 48 
// 			digits[bit_depth-1-i] = num % 10 + 48;
// 			break;	
// 		}
// 		num /= 10;
// 	}
// 	if(output_type == ASCII){
// 		//Добавляем символ конца строки, если на выходе необходим строковый тип
// 		digits[bit_depth] = '\0';
// 	}
// }

void Setup(void)
{
	stdio_init_all(); // optional for error messages , Initialize chosen serial port, default 38400 baud
	TFT_MILLISEC_DELAY(TEST_DELAY1);
	printf("TFT :: Start\r\n");

	//*************** USER OPTION 0 SPI_SPEED + TYPE ***********
	bool bhardwareSPI = true; // true for hardware spi, false for software

	if (bhardwareSPI == true) { // hw spi
		uint32_t TFT_SCLK_FREQ = 8000; // Spi freq in KiloHertz , 1000 = 1Mhz
		myTFT.TFTInitSPIType(TFT_SCLK_FREQ, spi0);
	}
	else { // sw spi
		uint16_t SWSPICommDelay = 0; // optional SW SPI GPIO delay in uS
		myTFT.TFTInitSPIType(SWSPICommDelay);
	}
	//*********************************************************
	// ******** USER OPTION 1 GPIO *********
	// NOTE if using Hardware SPI clock and data pins will be tied to 
	// the chosen interface eg Spi0 CLK=18 DIN=19)
	int8_t SDIN_TFT = 19;
	int8_t SCLK_TFT = 18;
	int8_t DC_TFT = 14;
	int8_t CS_TFT = 15;
	int8_t RST_TFT = 17;
	myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
	//**********************************************************

	// ****** USER OPTION 2 Screen Setup ****** 
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with screen manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW, TFT_WIDTH, TFT_HEIGHT);
	// ******************************************

	// ******** USER OPTION 3 PCB_TYPE  **************************
	myTFT.TFTInitPCBType(myTFT.TFT_ST7735S_Black); // pass enum,4 choices,see README
	//**********************************************************

	myTFT.TFTsetRotation(ST7735_TFT::TFT_Degrees_90);
	myTFT.TFTfillScreen(ST7735_BLACK);
}

/*!
	@brief print out hello world on TFT
*/
void Test0(void) {

	char teststr1[] = "ZAEBALO";
	char teststr2[] = "WSE";

	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(myTFT.TFTFont_Default);
	myTFT.TFTdrawText(15, 15, teststr1, ST7735_WHITE, ST7735_BLACK, 2);
	myTFT.TFTdrawText(15, 35, teststr2, ST7735_WHITE, ST7735_BLACK, 3);
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	myTFT.TFTfillScreen(ST7735_BLACK);
	TFT_MILLISEC_DELAY(TEST_DELAY1);
}

void Print_Char(char a) {
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(myTFT.TFTFont_Default);
	//myTFT.TFTdrawText(15, 15, teststr1, ST7735_WHITE, ST7735_BLACK, 2);
	myTFT.TFTdrawChar(15, 35, a, ST7735_WHITE, ST7735_BLACK, 3);
}

void Print_Number(int16_t num) {
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(myTFT.TFTFont_Default);

	char str[MAX_BIT_DEPTH];
	snprintf(str, MAX_BIT_DEPTH, "%d", num);
	myTFT.TFTdrawText(15, 35, str, ST7735_WHITE, ST7735_BLACK, 8);
}

void display_print_text(char str[]) {
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(myTFT.TFTFont_Default);
	myTFT.TFTdrawText(15, 15, str, ST7735_WHITE, ST7735_BLACK, 4);
}

/*!
	@brief  Stop testing and shutdown the TFT
*/
void EndTests(void)
{
	myTFT.TFTPowerDown();
	printf("TFT :: Tests Over");
}

