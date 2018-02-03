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
int count_delay = 150; // COUNTING SECONDS IF count_delay = 1000
long actual_count_delay = 0;

//--------------------- переменные паяльника -----------------------------
int button = 7; //Пин кнопок
int tin = 0; // Пин Датчика температуры IN Analog через LM358N
int pinpwm = 3;// порт нагревательного элемента(через транзистор)PWM
int tempust = 230; // установленная температура
int tempmin = 200; // минимальная температура
int tempmax = 480; // максимальная температура
int tempreal = 250; // переменная датчика текущей температуры
int temppwmmin = 40; // минимальное значение PWM нагревателя
int temppwmmax = 180; // максимальное значение PWM нагревателя
int temppwmreal = 0; // текущее значение PWM нагревателя
int airreal = 100; // стартовое значение PWM вентилятора(если нужно)
int temperror = -50; // разница температур(установленная - реальная)
int temprazn = 0; // переменная разницы температуры(установленная - текущая)
int increment = 0;

void setup(){
  pinMode(pinpwm,OUTPUT); // Порт нагрузки(паяльника) настраиваем на выход
  analogWrite(pinpwm, temppwmreal); //Выводшим в нагрузку паяльника (выводим 0 - старт с выключеным паяльником- пока не опредилим состояние температуры)
  byte numDigits = 3; //num of segments
  byte digitPins[] = {4, 7, 8};  //left to right
  byte segmentPins[] = {5, 9, 16, 18, 19, 6, 15, 17};  //a to g + dg
  bool resistorsOnSegments = false; // 'false' means resistors 330 Ohm are on digit pins
  byte hardwareConfig = COMMON_CATHODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(70);
  sevseg.blank();
  Serial.begin(9600);
}

void loop() {
  show(increment); // Вывести значение переменной на экран(LED)
//----------------------------------------------------------Вычисление текущей темепературы относительно установенной----------------------------------------------------------
  if (tempreal < tempust ){ // Если температура паяльника ниже установленной температуры то:

    if ((tempust - tempreal) < 16 & (tempust - tempreal) > 6 ){ // Проверяем разницу между у становленной температурой и текущей паяльника, и если разница меньше 10 градусов то 
      temppwmreal = 99;// Понижаем мощность нагрева (шим 0-255мы делаем 99)- таким образом мы убираем инерцию перегрева
    }
    else if ((tempust - tempreal) < 7 & (tempust - tempreal) > 3){
      temppwmreal = 80;// Понижаем мощность нагрева (шим 0-255мы делаем 80)- таким образом мы убираем инерцию перегрева
    }

    else if ((tempust - tempreal) < 4 ){
      temppwmreal = 45;// Понижаем мощность нагрева (шим 0-255мы делаем 45)- таким образом мы убираем инерцию перегрева
    }
    else {
      temppwmreal = 230; // Иначе Подымаем мощность нагрева(шим 0-255мы делаем 230) на максимум для быстрого нагрева до нужной температуры
    }
  }
  else { //Иначе (если температура паяльника равняется или выше установленной) 
    temppwmreal = 0;// Выключаем мощность нагрева (шим 0-255мы делаем 0)- таким образом мы отключаем паяльник
  }

  analogWrite(pinpwm, temppwmreal); //Вывод в шим порт (на транзистор) значение мощности
  
//---------------------------------------------------------- Кнопки -------------------------------------------------------------------------
  if(millis() - actual_count_delay > count_delay){ // это для сегментов 
    actual_count_delay = millis();
  // Здесь мы пишем нашу прогу по считыванию состояния кнопок (это место в счетчк не будет тормозить вывод на сегменты)
    tempreal = analogRead(tin);// считываем текущую температуру
    tempreal=map(tempreal,750,1023,20,500); // нужно вычислить
    tempreal=constrain(tempreal,20,500);
    increment=tempreal;

    if (analogRead(button) == 0){// Если нажата вниз кнопка то понизить температуру на 5
      if( tempust <= tempmin || (tempust-5) <= tempmin ){
        tempust= tempmin;
        increment = tempust;
      }
      else{ 
        tempust-=5;
        increment = tempust;
        
      } 
      show(increment); // Вывести значение переменной на экран(LED)
    }
    else if (analogRead(button) > 500 && analogRead(button) < 524 ){// Если нажата вверх кнопка то повысить температуру на 5
      tempust+=5;
      if( tempust >=tempmax){
        tempust= tempmax;
      }
      increment = tempust;
      show(increment); // Вывести значение переменной на экран(LED)
    }
  }
}

void show(int value) {
    sevseg.setNumber(value);
    //sevseg.setNumber(analogRead(tin));
    sevseg.refreshDisplay();
}
