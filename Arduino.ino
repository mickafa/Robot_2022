#define INA 2  // четыре фазы шагового мотора
#define INB 3
#define INC 4
#define IND 5
#define FORTH 8  // реле хода мотора толкателя
#define REVERSE1 10 // два реле реверсирования направления толкателя
#define REVERSE2 9
#define TSTEP (6) // время выдержки на фазе шагового мотора
#define PIN_TRIG 12  // запуск дальномера
#define PIN_ECHO 11  // получение ответа от дальномера
#define BLACK 13  // получение цвета метки на толкателе 
#define RED 7   // светодиоды индикации
#define GREEN 6

#define ADVANCE (1)   // прямой ход толкателя
#define RETRACT (-1)  // реверсивный ход толкателя

const double sound_speed = 343.; // скорость звука (м/с)

double distance ( );
// измерение дальномером (назкоуровневое)

int space ( int s );
// измерение (высокоуровневое), s - ожидаемое значение (мм). Возвращает 
//фактическое значение от дальномера до стенки (мм), либо 0 если неуспешно

void stp ( int a, int b, int c, int d );
// переключение на одну фазу (низкоуровневое)

void rotate ();
// поворот ротора шагового мотора на один оборот

void drive ( long l );
// проехать вперёд на l мм

// void push ( int w );
// включение-выключение реле мотора толкателя (низкоуровневое)
// w: 0 - выключить, 1 - вперёд, -1 - назад

// void DCmotor ();
// цикл толкателя вперёд и назад

void indi ( int p ) ; 
// индикация одну секунду красным ( p = RED) или зелёным ( p = GREEN) светодиодом


void setup ( ) {
//  Serial.begin ( 9600 );
  pinMode ( INA, OUTPUT );
  pinMode ( INB, OUTPUT );
  pinMode ( INC, OUTPUT );
  pinMode ( IND, OUTPUT );
  pinMode ( FORTH, OUTPUT );
  pinMode ( REVERSE1, OUTPUT );
  pinMode ( REVERSE2, OUTPUT );
  pinMode ( PIN_TRIG, OUTPUT );
  pinMode ( PIN_ECHO, INPUT );
  pinMode ( BLACK, INPUT );
  pinMode ( RED, OUTPUT );
  pinMode ( GREEN, OUTPUT );
///\\\///\\\///\\\///\\\///\\\///\\\///\\\///\\\///\\\///
 indi ( RED ) ;  // время убрать пальцы от робота
 indi ( GREEN ) ;

 motor ( ADVANCE, 300 ) ;  // выдвинем толкатель немного, чтобы резистор R1 остыл

// едем вперёд 558 мм. Передний бампер сравняется с осью кубика
  drive ( 558 ) ;

// измерение, ожидаем получить 1000 мм от дальномера до стенки.
// если измерение успешно, то едем t + 5 - 890 мм, иначе едем 115 мм
  int t = space ( 1000 ) ;
  
  if ( t ) { 
             indi ( GREEN ) ; // измерение успешно
             drive ( t + 5 - 890 ) ;
  }
  else { 
	indi ( RED ) ;  // измерение неуспешно
	drive ( 115 );
 }

// толкаем кубик
// суммарный ограничитель времени прямого хода толкателя 300 + 2000 = 2300 мс
	motor ( ADVANCE , 2000 ) ;
	delay ( 1000 ) ;
// немного отодвинем толкатель назад, чтобы не зацепил за кубик при страгивании телеги   
	motor ( RETRACT , 300 ) ;

	//  DCmotor ( );  старая функция, потом убрать

// едем вперёд 632 - 115 мм. Передний бампер сравняется с осью финишной зоны
  drive ( 632 - 115 );

// измерение, ожидаем получить 258 + 110 мм от дальномера до стенки.
// если измерение успешно, то едем t - 258 + 7 мм, иначе едем 117 мм
  t = space ( 258 + 110 ) ;
  if ( t ) {
	indi ( GREEN ) ;  // измерение успешно
	drive ( t + 7 - 258 ) ;
 }
  else { 
	indi ( RED ) ;  // измерение неуспешно
	drive ( 117 ) ;
 }

// убираем толкатель полностью
// суммарный ограничитель времени реверсивного хода толкателя 300 + 1700 = 2000 мс
	motor ( RETRACT , 1700 ) ;  

// конец программы
  stp ( LOW,  LOW,  LOW,  LOW );   // гасим ходовой шаговый двигатель
  delay ( 2000 ) ;
  indi ( RED ) ;		// стоп-сигнал - движение закончено
}


void loop ( ) {
  ; // пустой цикл ожидания выключения питания
}