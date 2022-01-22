#include <Arduino.h>

#include <MD_AD9833.h>
#include <SPI.h>
#include <menu.h>
#include <menuIO/serialOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialIn.h>
#include <Adafruit_ST7735.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#include <TimerOne.h>
#include <ClickEncoder.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>

// Pins for SPI comm with the AD9833 IC
#define FSYNC PIN_SPI_SS	///< SPI Load pin number (FSYNC in AD9833 usage)
#define DATA  PIN_SPI_MOSI	///< SPI Data pin number
// #define PIN_SPI_MISO  (12)
#define CLK   PIN_SPI_SCK	///< SPI Clock pin number

MD_AD9833	AD(FSYNC);  // Hardware SPI
// MD_AD9833	AD(DATA, CLK, FSYNC); // Arbitrary SPI pins


using namespace Menu;

#define LEDPIN LED_BUILTIN
// #define MAX_DEPTH 1

//ST7735 1.8TFT 128x160
#define TFT_SCLK 5
#define TFT_MOSI 6
#define TFT_DC   7
#define TFT_RST  8
#define TFT_CS	 9

// clickencoder pins
#define encA    2
#define encB    3
#define encBtn  4


// Adafruit_ST7735 gfx(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ST7735 gfx(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK,
                                 TFT_RST);

result doAlert(eventMask e, prompt &item);

int test=55;

int ledCtrl=LOW;

result myLedOn() {
  ledCtrl=HIGH;
  return proceed;
}
result myLedOff() {
  ledCtrl=LOW;
  return proceed;
}

TOGGLE(ledCtrl,setLed,"Led: ",doNothing,noEvent,noStyle//,doExit,enterEvent,noStyle
  ,VALUE("On",HIGH,doNothing,noEvent)
  ,VALUE("Off",LOW,doNothing,noEvent)
);

int selTest=0;
SELECT(selTest,selMenu,"Select",doNothing,noEvent,noStyle
  ,VALUE("Zero",0,doNothing,noEvent)
  ,VALUE("One",1,doNothing,noEvent)
  ,VALUE("Two",2,doNothing,noEvent)
);

int chooseTest=-1;
CHOOSE(chooseTest,chooseMenu,"Choose",doNothing,noEvent,noStyle
  ,VALUE("First",1,doNothing,noEvent)
  ,VALUE("Second",2,doNothing,noEvent)
  ,VALUE("Third",3,doNothing,noEvent)
  ,VALUE("Last",-1,doNothing,noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt:public prompt {
public:
  altPrompt(constMEM promptShadow& p):prompt(p) {}
  Used printTo(navRoot &root,bool sel,menuOut& out, idx_t idx,idx_t len,idx_t) override {
    return out.printRaw(F("special prompt!"),len);;
  }
};

MENU(subMenu,"Sub-Menu",doNothing,noEvent,noStyle
  ,altOP(altPrompt,"",doNothing,noEvent)
  ,OP("Op",doNothing,noEvent)
  ,EXIT("<Back")
);

const char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
const char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
char buf1[]="0x11";

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
//   ,OP("Op1",doNothing,noEvent)
//   ,OP("Op2",doNothing,noEvent)
  // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
  ,SUBMENU(subMenu)
  ,SUBMENU(setLed)
//   ,OP("LED On",myLedOn,enterEvent)
//   ,OP("LED Off",myLedOff,enterEvent)
  ,SUBMENU(selMenu)
  ,SUBMENU(chooseMenu)
  //,OP("Alert test",doAlert,enterEvent)
  ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
  ,EXIT("<Back")
);

// define menu colors --------------------------------------------------------
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
//monochromatic color table

#define ST7735_GRAY RGB565(128,128,128)

const colorDef<uint16_t> colors[6] MEMMODE={
  {{(uint16_t)ST7735_BLACK,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_BLACK, (uint16_t)ST7735_BLUE,  (uint16_t)ST7735_BLUE}},//bgColor
  {{(uint16_t)ST7735_GRAY, (uint16_t)ST7735_GRAY},  {(uint16_t)ST7735_WHITE, (uint16_t)ST7735_WHITE, (uint16_t)ST7735_WHITE}},//fgColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_YELLOW,(uint16_t)ST7735_YELLOW,(uint16_t)ST7735_RED}},//valColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_BLACK}, {(uint16_t)ST7735_WHITE, (uint16_t)ST7735_YELLOW,(uint16_t)ST7735_YELLOW}},//unitColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_GRAY},  {(uint16_t)ST7735_BLACK, (uint16_t)ST7735_BLUE,  (uint16_t)ST7735_WHITE}},//cursorColor
  {{(uint16_t)ST7735_WHITE,(uint16_t)ST7735_YELLOW},{(uint16_t)ST7735_BLUE,  (uint16_t)ST7735_RED,   (uint16_t)ST7735_RED}},//titleColor
};

serialIn serial(Serial);

// ClickEncoder clickEncoder(encA,encB,encBtn);
ClickEncoder clickEncoder(encA,encB,encBtn, 4, LOW);

ClickEncoderStream encStream(clickEncoder,1);
MENU_INPUTS(in,&encStream,&serial);
void timerIsr() {clickEncoder.service();}

#define MAX_DEPTH 4
#define textScale 1
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(gfx,colors,6*textScale,9*textScale,{0,0,14,8},{14,0,14,8})
  ,SERIAL_OUT(Serial)
);

NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

//when menu is suspended
result idle(menuOut& o,idleEvent e) {
  if (e==idling) {
    o.println(F("suspended..."));
    o.println(F("press [select]"));
    o.println(F("to continue"));
  }
  return proceed;
}

//config myOptions('*','-',defaultNavCodes,false);

void setup(void)
{
  Serial.begin(9600);
  Serial.write("Serial initialised");
  AD.begin();

  //options=&myOptions;//can customize options
  pinMode(LEDPIN,OUTPUT);
  while(!Serial);
  Serial.println("menu 4.x test");
  Serial.flush();
  nav.idleTask=idle;//point a function to be used when menu is suspended
  mainMenu[1].disable();
  nav.inputBurst = 1;
  nav.canExit = false;
  //outGfx.usePreview=true;//reserve one panel for preview?
  //nav.showTitle=false;//show menu title?

  //pinMode(encBtn, INPUT_PULLUP);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  SPI.begin();
  gfx.initR(INITR_GREENTAB);
  gfx.setRotation(3);
  gfx.setTextSize(textScale);//test scalling
  gfx.setTextWrap(false);
  gfx.fillScreen(ST7735_BLACK);
  gfx.setTextColor(ST7735_RED,ST7735_BLACK);
  gfx.println("Menu 4.x test on GFX");
  delay(1000);

}


void loop(void)
{
	static uint16_t lastv = 0;
	uint16_t v = 1234; // analogRead(A0);
//	Serial.println(v);

	if (abs(v-lastv) > 20)
	{
		AD.setFrequency(MD_AD9833::CHAN_0, 1000 + v);
		lastv = v;
	}

  nav.poll();//this device only draws when needed
  digitalWrite(LEDPIN, ledCtrl);
  delay(100);//simulate a delay when other tasks are done

}
