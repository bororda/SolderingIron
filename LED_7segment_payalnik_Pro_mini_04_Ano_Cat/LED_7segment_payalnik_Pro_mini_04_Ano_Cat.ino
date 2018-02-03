/*
// Пины подключения индикаторов
ANODES(CATHODES):
D1 - a
D2 - b
D4 - c
D6 - d
D7 - e
D5 - f
D3 - g
D0 - dp

    a1
  ********
  *       *
f *        * b2
5   *g3*
  ********
  *       *
e *       * c4
7   *d6*
******** # dp0

CATHODES(ANODES):
D16 - cathode 3
D15 - cathode 2
D12 - cathode 1
*/
#include <SevSeg.h>
SevSeg sevseg;

//--------------------- variables setup -----------------------------
#define buttonDown = 2; //Пин кнопок
#define buttonUp = 3;
#define tin = 0; // Пин Датчика температуры IN Analog через LM358N
#define pinpwm = 10;// порт нагревательного элемента(через транзистор)PWM
#define numDigits = 3; //num of segments
volatile int tempSet = 230; // установленная температура
volatile byte flag = false; //флаг для управления отображаемой температуры (tempReal или tempSet)
int tempMin = 200; // минимальная температура
int tempMax = 480; // максимальная температура
int tempReal = 20; // переменная датчика текущей температуры
int temppwmreal = 0; // текущее значение PWM нагревателя
int tempToDisplay = 0;
long time = 0;//переменная для хранения времени изменения температуры
byte digitPins[] = {4, 7, 8};  //left to right
byte segmentPins[] = {5, 9, 16, 18, 19, 6, 15, 17};  //a to g + dg
bool resistorsOnSegments = false; // 'false' means resistors 330 Ohm are on digit pins
byte hardwareConfig = COMMON_CATHODE;

void setup(){
	pinMode(pinpwm, OUTPUT); // Порт нагрузки(паяльника) настраиваем на выход
	pinMode(buttonDown, INPUT_PULLDOWN);
	pinMode(buttonUp, INPUT_PULLDOWN);
	attachInterrupt(digitalPinToInterrupt(), RISING);
	attachInterrupt(digitalPinToInterrupt(), RISING);
	sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
	sevseg.setBrightness(70);
	sevseg.blank();
	Serial.begin(9600);
}

void loop(){
	show(); // Вывести значение переменной на экран(LED)
//----------------------------------------------------------Вычисление текущей темепературы относительно установенной----------------------------------------------------------
	if (tempReal < tempSet ){ // Если температура паяльника ниже установленной температуры то:
		if ((tempSet - tempReal) < 16 & (tempSet - tempReal) > 6 ) temppwmreal = 150; // Проверяем разницу между у становленной температурой и текущей паяльника, и если разница меньше 10 градусов, то понижаем мощность нагрева, убираем инерцию перегрева (шим 0-255)
		else if ((tempSet - tempReal) < 7 & (tempSet - tempReal) > 3) temppwmreal = 120;// Понижаем мощность нагрева, убираем инерцию перегрева
		else if ((tempSet - tempReal) < 4 ) temppwmreal = 90;// Понижаем мощность нагрева, убираем инерцию перегрева
		else temppwmreal = 230; // Иначе Подымаем мощность нагрева на максимум для быстрого нагрева до нужной температуры
	}
	else temppwmreal = 0;//Иначе (если температура паяльника равняется или выше установленной) Выключаем мощность нагрева, отключаем паяльник
	analogWrite(pinpwm, temppwmreal); //Вывод в шим порт (на транзистор) значение мощности

	show(); // Вывести значение переменной на экран(LED)

	tempReal = analogRead(tin);// считываем текущую температуру
	tempReal=map(tempReal,750,1023,20,500); // нужно вычислить
	tempReal=constrain(tempReal,20,500);

	if(flag true){
		flag = false;
		time=millis();
	}
	if(millis()-time<=2000){
		tempToDisplay=tempSet;
	}
	else tempToDisplay=tempReal;
}
void show(){
	sevseg.setNumber(tempToDisplay);
	sevseg.refreshDisplay();
}
void tmpDown(){
	if((tempSet-5) >= tempMin){ //будущая температура не ниже минимальной
		tempSet-=5;
		flag = true;
	}
}
void tmpUp(){
	if((tempSet+5) <= tempMax){ // будущая температура не выше максимальной
		tempSet+=5;
		flag = true;
	}
}