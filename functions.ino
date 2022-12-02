void indi ( int p ) { // индикация одну секунду красным или зелёным светодиодом
  digitalWrite ( RED, LOW );
  digitalWrite ( GREEN, LOW );
  digitalWrite ( p, HIGH );
  delay ( 1000 );
  digitalWrite ( p, LOW );
}

double distance ( ) { // низкоуровневое измерение дальномером
  long duration ; // время эха
  digitalWrite ( PIN_TRIG, LOW );
  delayMicroseconds ( 5 ) ;
  
  digitalWrite ( PIN_TRIG, HIGH) ; // включаем излучатель дальномера 
  delayMicroseconds ( 10 ) ;  // выдержка на работу излучателя
  digitalWrite ( PIN_TRIG, LOW ) ;  // выключаем излучатель

  duration = pulseIn ( PIN_ECHO, HIGH ) ; // получаем длительность эха (мкс)
  return (duration / 2. / 1000.) * sound_speed; // возвращаем расстояние (мм)
}

int space ( int s ){  // s - ожидаемое расстояние (мм)
  long summ = 0 ;   // суммарное намеренное расстояние десятью замерами
  int cnt = 0 ;     // количество достоверных измерений

  for ( int i = 0; i < 10; i++ ) { // делаем 10 измерений
    int l;
    l = distance ( ) ;
    if ( (( l > 0.85*s ) && ( l < 1.15*s )) ) { 
// если отличие результата от ожидаемого не превышает +/- 15%, то измерение достоверное
      cnt++;
      summ += l;
    }
  }

  if ( cnt ) { 
// если хотя бы одно измерение получилось достоверным, то возвращаем среднее всех успешных измерений
// индикация зелёная
   		indi ( GREEN ) ;
                return (int) ((double)summ / (double)cnt); // ответ в (мм)
  }
  else {
// если не было ни одного успешного измерения (отказ дальномера), то возвращаем ответ 0 (мм)
// индикация красная 
	indi ( RED ) ;
  	return 0;
  }
}

void stp ( int a, int b, int c, int d ) {
// проворот ротора шагового мотора на одну фазу и выдержка TSTEP 
	digitalWrite ( INA, a );
  	digitalWrite ( INB, b );
	digitalWrite ( INC, c );
	digitalWrite ( IND, d );
  	delay ( TSTEP );
}


void rotate ( ) {
// один оборот ротора шагового мотора последовательным переключением фаз
  stp ( HIGH,  LOW,  LOW,  LOW );
  stp ( HIGH, HIGH,  LOW,  LOW );
  stp (  LOW, HIGH,  LOW,  LOW );
  stp (  LOW, HIGH, HIGH,  LOW );
  stp (  LOW,  LOW, HIGH,  LOW );
  stp (  LOW,  LOW, HIGH, HIGH );
  stp (  LOW,  LOW,  LOW, HIGH );
  stp ( HIGH,  LOW,  LOW, HIGH );
}


void drive ( long l ) { // l - сколько проехать в мм
  l *= 5000;          // 5000/1175 - пересчёт из мм в обороты шагового мотора
  l /= 1175;          // с учётом передаточного отношения в редукторе и шкивах
  while ( l-- ) rotate ( ); // l теперь теперь это кол-во оборотов ротора, едем их 
}

void push ( int w ){ // толкатель: 1 - вкл передний ход, 0 - остановить, -1 - вкл ход на реверс
// выключаем все три реле
  digitalWrite ( FORTH, LOW ) ;
  digitalWrite ( REVERSE1, LOW ) ;
  digitalWrite ( REVERSE2, LOW ) ;

	switch ( w ) {
		case -1: // задний ход, включение реле реверса
			digitalWrite ( REVERSE1, HIGH ) ;
			digitalWrite ( REVERSE2, HIGH ) ;
		case  1: // включение ходового реле 
			digitalWrite ( FORTH, HIGH ) ;
		case 0: ; // 0 - нет действий (все реле выключены, толкатель остановлен)
       }
}


// подать толкатель в направление direction на время не более ms миллисекунд (давать больше, чем 200 )
void motor ( int direct, unsigned long ms ) { 
	int error_code = 0 ;
	unsigned long tim ;        // ограничитель времени хода
	
  	tim = millis ( ) + ms ; // устанавливаем целевое время окончания хода 

	if ( direct == RETRACT ) {   // если убирать толкатель, то вкл реле реверса
		digitalWrite ( REVERSE1, HIGH ) ;
		digitalWrite ( REVERSE2, HIGH ) ;
	}
	digitalWrite ( FORTH, HIGH ) ;   // пуск мотора толкателя
	delay ( 200 ) ; 		// едем принудительно 200 мс чтобы гарантированно сойти с чёрной метки

  	// продолжаем ехать пока видим белый цвет, а также ограничитель по времени не превышен
  	// digitalRead ( BLACK ) даёт 0 когда цвет белый, 1 когда чёрный
  	while ( error_code == 0 ) {
		if ( digitalRead ( BLACK ) == 1 ) error_code = GREEN ;
		if ( millis ( ) > tim ) error_code = RED ;
  	}
	 
	digitalWrite ( FORTH, LOW ) ;  // остановка мотора толкателя
	digitalWrite ( REVERSE1, LOW ) ;  // обесточиваем реле реверса
	digitalWrite ( REVERSE2, LOW ) ;

	indi ( error_code ) ; // моргнём красным светодиодом если остановка произошла по времени, 
				// зелёным - если по достижении чёрной метки

}