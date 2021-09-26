#include <LiquidCrystal.h>            //  Подключаем библиотеку LiquidCrystal для работы с LCD дисплеем
LiquidCrystal lcd(16,17,23,25,27,29); //  Объявляем объект библиотеки, указывая выводы дисплея (RS,E,D4,D5,D6,D7)

#define CLK 31
#define DT 33
#define SW 35
#include "GyverEncoder.h"  //  Подключаем библиотеку для работы с енкодером
Encoder enc1(CLK, DT, SW);
int value = 0;
boolean Edirection; //Напрвалення енкодера

#include <ArduinoJson.h>  //  Подключаем библиотеку для работы с json
#include <SD.h>
#include <SPI.h>
const int PIN_CHIP_SELECT = 53; // Ініціалізація бібліотек 
String input; // Строка в якій буде зберігатись файл json
String inputJson; // Строка в якій буде зберігатись файл json
String inputData; // Строка в якій буде зберігатись файл jsonData
const char *filename = "/Test.txt";  // Назва файлу в якому лежить json
const char *FileDataName = "/Data.txt";  // Назва файлу в якому лежить json з змінними

#include <Servo.h> //  Подключаем библиотеку для работы с серво
Servo Lservo;
int pos;//Позиція серво

#define ONE_WIRE_BUS 4  //Пін на якій підключено ds18b20
#include <OneWire.h>  //  Подключаем библиотеку для работы по одному піну
#include <DallasTemperature.h>  //  Подключаем библиотеку для работы с ds18b20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress *sensorsUnique;
int countSensors;
int temperature[10];

#define B 3950 // B-коэффициент
#define SERIAL_R 4700 // сопротивление последовательного резистора
#define THERMISTOR_R 100000 // номинальное сопротивления термистора, 100 кОм
#define NOMINAL_T 25 // номинальная температура (при которой TR = 100 кОм)
const byte tempPin = A13;
int termis;

#include "SparkFunHTU21D.h"   // Подключаем библиотеку SparkFunHTU21D
HTU21D myHumidity;  // Создаем объект для работы с датчиком
int temp;  //Змінна для температури
int humd;  //Змінна для вологості

#include <buildTime.h>
#include <microDS3231.h>
MicroDS3231 rtc;

long last_time;
boolean time_flag;
boolean Water;
#define BWater 3 // Пін датчика наявності води
#define BStop 41 // Пін кнопки СТОП
int YMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; //Вказано скільки днів в кожному місяці 
boolean mode = 0;//Змінна для запису прочитаного з json
int Month = 0;//Змінна для запису прочитаного з json
int Day = 0;//Змінна для запису прочитаного з json
int Hours = 0;//Змінна для запису прочитаного з json
int Min = 0;//Змінна для запису прочитаного з json
int IDay = 0;//Змінна для запису днів які пройшли після запуску інкубації (звертатись після обрахунку)
int IHours = 0;//Змінна для запису годин які пройшли після запуску інкубації (звертатись після обрахунку)
int IMin = 0;//Змінна для запису хвилин які пройшли після запуску інкубації (звертатись після обрахунку)

int menu = 0; //Змінна яка використовується в показі меню
int error;    //Змінна яка використовується в показі меню

byte LH[8] = {  //Іконка лотка поверненого горизонтально
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01010,
  0b00000,
  0b00000
};
byte LL[8] = {  //Іконка лотка поверненого в ліво
  0b00000,
  0b00000,
  0b00000,
  0b11000,
  0b01110,
  0b00011,
  0b00000,
  0b00000
};
byte LR[8] = {  //Іконка лотка поверненого в право
  0b00000,
  0b00000,
  0b00000,
  0b00011,
  0b01110,
  0b11000,
  0b00000,
  0b00000
};
byte Gr[8] = {  //Іконка градуса
  0b01000,
  0b10100,
  0b01000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte Setting[8] = {  //Іконка настройок
  0b00000,
  0b00000,
  0b10101,
  0b01110,
  0b11011,
  0b01110,
  0b10101,
  0b00000
};
byte WaterOff[8] = {  //Іконка "Вода закінчується"
  0b00000,
  0b00000,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000
};
byte WaterOn[8] = {  //Іконка "Вода є"
  0b00000,
  0b00000,
  0b10001,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000
};


void jsonFile(const char *filename,const char *fileinput) {  // Функція запису json в input
  input = "";
  File file = SD.open(filename);    // Відкрити файл
  if (!file) {     //При помилці читання файлу 
    Serial.println(F("Помилка читання файлу"));  // Вивести в серіал "Помилка читання файлу"
    return;
  }

  while (file.available()) {  // Якщо файл відкритий
    String inputdop = String((char)file.read());   // Читати по кусочку
    input = input + inputdop;   // Склеїти прочитані кусочки і записати в input
  }
  if(fileinput=="Data"){
    inputData = input;
  }
  //Serial.print(input);
  Serial.println();
  file.close();   // Закрити файл
}

void DataWrite(const char *FileDataName,boolean i) {  // Функція запису json в файл Data
  SD.remove("/Data.txt");
  File file = SD.open(FileDataName, FILE_WRITE);    // Відкрити файл
  if (!file) {     //При помилці читання файлу 
    Serial.println(F("Помилка читання файлу WRITE"));  // Вивести в серіал "Помилка читання файлу"
    return;
  }
  if(i == 1){
    file.println("{\"mode\":" + String(i) + ",\"Month\":" + String(rtc.getMonth()) + ",\"Day\":" + String(rtc.getDate()) + ",\"Hours\":" + String(rtc.getHours()) + ",\"Min\":" + String(rtc.getMinutes()) + "}");
    file.close();   // Закрити файл
  }else{
    file.println("{\"mode\":" + String(i) + ",\"Month\":0,\"Day\":0,\"Hours\":0,\"Min\":0}");
    file.close();   // Закрити файл
  }
    
}

void DataJSON(const char *FileDataName) {    // Функція читання jsonData
  DynamicJsonBuffer jsonBuffer;
  jsonFile(FileDataName,"Data");// Перейти до функції запису json в inputData
  JsonObject& rootData = jsonBuffer.parseObject(inputData);
  delay(50);
  mode = rootData[String("mode")];
  Month = rootData[String("Month")];
  Day = rootData[String("Day")];
  Hours = rootData[String("Hours")];
  Min = rootData[String("Min")];
}

void Time (){   //Функція обрахунку часу
  if((rtc.getMonth()-Month)<0){
    IDay = (YMonth[Month-1]-Day) + rtc.getDate();
  }else{
    IDay = rtc.getDate() - Day;
  }
  if((rtc.getHours()-Hours)<0){
    IDay--;
    IHours = (24-Hours)+rtc.getHours();
  }else{
    IHours = rtc.getHours()-Hours;
  }
  if((rtc.getMinutes()-Min)<0){
    IHours--;
    IMin = (60-Min)+rtc.getMinutes();
  }else{
    IMin = rtc.getMinutes()-Min;
  }
  Serial.println();
  Serial.print("IDay ");
  Serial.println(IDay);
  Serial.print("IHours ");
  Serial.println(IHours);
  Serial.print("IMin ");
  Serial.println(IMin);
}

void DravMenu(int menu,int number,boolean mode,boolean Edirection){  //Функція показу меню
  switch (menu) {
    case 1:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Stop incubation?");
      lcd.setCursor(6, 2);
      lcd.print("NO   YES");
  
      if (Edirection == 0){
      lcd.setCursor(5, 2);
      lcd.print(">");
      }else{
      lcd.setCursor(10, 2);
      lcd.print(">");
      }
      break;
    default:
      lcd.clear();
      if (error == 0){
        if (mode==1){
        lcd.setCursor(2, 0);
        lcd.print("Incubation");
        lcd.setCursor(14, 0);
        lcd.print("Day:");
        lcd.setCursor(18, 0);
        lcd.print(IDay);
        }else{
        lcd.setCursor(2, 0);
        lcd.print("START");}
        } if (error == 1){
        lcd.setCursor(2, 0);
        lcd.print("Check all sensors");
      }
      if (pos == 0){
        lcd.setCursor(2, 1);
        lcd.write(byte(0));
      }else if(pos == -1){
        lcd.setCursor(2, 1);
        lcd.write(1);
      }else if(pos == 1){
        lcd.setCursor(2, 1);
        lcd.write(2);
      }

      lcd.setCursor(4, 1);
      lcd.print("TBed:");
      lcd.print(termis);
      lcd.write(3);
      
      lcd.setCursor(14, 1);
      lcd.print("T :");
      lcd.setCursor(15, 1);       
      lcd.write(byte(0));
      lcd.setCursor(17, 1);
      if(error == 0){lcd.print(String(temperature[2]));
      }else{lcd.print("--");}
      lcd.write(3);
      


      lcd.setCursor(2, 2);
      lcd.print("TAir:");      
      lcd.print(temp);
      lcd.write(3);
      lcd.setCursor(11, 2);
      lcd.print("HAir:");      
      lcd.print(humd);
      lcd.print("%");




      
  lcd.setCursor(0, number);
  lcd.print(">");

  
  lcd.setCursor(6, 3);
  lcd.print(rtc.getDate());
  lcd.print(".");
  lcd.print(rtc.getMonth());
  lcd.print(".");
  lcd.print(rtc.getYear()-2000);
  lcd.setCursor(15, 3);
  lcd.print(rtc.getHours());
  lcd.print(":");
  lcd.print(rtc.getMinutes());

  if (Water == 1){
    lcd.setCursor(4, 3);
  lcd.write(5);
  }else{
    lcd.setCursor(4, 3);
  lcd.write(6);
  }
  
  lcd.setCursor(2, 3);
  lcd.write(4);
  break;
  }

}

void servo(int angle) {
  if(angle==89)pos = 0;
  if(angle==106)pos = -1;
  if(angle==69)pos = 1;
  Lservo.write(angle);
}

void beginAll() {
    Serial.begin(9600);  //швидкість Serial
    lcd.begin(20, 4);  //  Инициируем работу с LCD дисплеем, указывая количество (столбцов, строк)
    enc1.setType(TYPE1);  //Инициализация енкодера
    sensors.begin();  //Инициализация DS18B20
    myHumidity.begin();  // Инициализация HTU21D
    DynamicJsonBuffer jsonBuffer;
    while (!SD.begin()) {      // При помилці ініціалізувати SD бібліотеку
    Serial.println(F("Помилка ініціалізації SD бібліотеки"));  // Вивести в серіал "Помилка ініціалізації SD бібліотеки"
    delay(100);
  }
}

void ds18b20() {
  countSensors = sensors.getDeviceCount();
  countSensors = 3;
  sensorsUnique = new DeviceAddress[countSensors];
  for (int i = 0; i < countSensors; i++) {
    sensors.getAddress(sensorsUnique[i], i);
  }
  for (int i = 0; i < countSensors; i++) {
    sensors.setResolution(sensorsUnique[i], 12);
  }
  sensors.requestTemperatures();
  for (int i = 0; i < countSensors; i++) {
    temperature[i] = sensors.getTempCByIndex(i);
  }

    if(temperature[0]<=-10 || temperature[1]<=-10 || temperature[2]<=-10){error=1;
  }else{error=0;}
}

void termistor() {
  int t = analogRead( tempPin );
  float tr = 1023.0 / t - 1;
  tr = SERIAL_R / tr;

  float steinhart;
  steinhart = tr / THERMISTOR_R; // (R/Ro)
  steinhart = log(steinhart); // ln(R/Ro)
  steinhart /= B; // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_T + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart; // Invert
  steinhart -= 273.15; 
  termis = int (steinhart);
}



void setup() {
  beginAll();  //Функція ініціалізації всього
  Time ();  //Функція обрахунку часу (працює після читання jsonData)
  //  rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);  //Розкоментувати для задання часу вручну (потім закоментувати і перезаписати код)
  lcd.createChar(0, LH);  //Ініціалізація іконки
  lcd.createChar(1, LL);  //Ініціалізація іконки
  lcd.createChar(2, LR);  //Ініціалізація іконки
  lcd.createChar(3, Gr);  //Ініціалізація іконки
  lcd.createChar(4, Setting);  //Ініціалізація іконки
  lcd.createChar(5, WaterOff);  //Ініціалізація іконки
  lcd.createChar(6, WaterOn);  //Ініціалізація іконки

  Lservo.attach(5);  // Пін серво
  pinMode(BStop, INPUT_PULLUP);  //Встановити пін як вихід з підтяжкою до +
  pinMode(BWater, INPUT_PULLUP);  //Встановити пін як вихід з підтяжкою до +
  pinMode( tempPin, INPUT );  //Встановити пін як вихід
  DataJSON(FileDataName);  // Функція читання jsonData

  DravMenu(menu,value,mode,Edirection);  //Функція показу меню
}

void loop() {
  enc1.tick();
  Water = digitalRead(BWater);
  termistor();
  if (millis() - last_time > 1000){
    DravMenu(menu,value,mode,Edirection);
    last_time = millis();
    if (time_flag == 1) {
     // ds18b20();
    }
    time_flag = !time_flag;
  }
  humd = myHumidity.readHumidity();    // Считываем влажность
  temp = myHumidity.readTemperature(); // Считываем температуру
  
  if (enc1.isRight()) {value++;      // если был поворот направо, увеличиваем на 1
  Edirection = 0;}
  if (enc1.isLeft()) {value--;
  Edirection = 1;}
   if (enc1.isTurn()) {       // если был совершён поворот (индикатор поворота в любую сторону)
   // DravMenu(menu,value,mode,Edirection);
  } 

  if (enc1.isPress()){
    if (menu == 0&&mode == 0&&value==0 ){
      DataWrite(FileDataName,1);
      inputData = "";
      DataJSON(FileDataName);
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 0){
      menu = 0;
      value = 0;  
      DravMenu(menu,value,mode,Edirection);
    }
    if (menu == 1&&Edirection == 1){
      menu = 0;
      value = 0;
      DataWrite(FileDataName,0);
      inputData = "";
      DataJSON(FileDataName);
      DravMenu(menu,value,mode,Edirection);
    }
  }
  //DravMenu(menu,value,mode,Edirection);
  if (digitalRead(BStop)==0&&mode==1){
    menu = 1;
    Edirection = 0;
    DravMenu(menu,value,mode,Edirection);
    Serial.println("stop");
  }
}
