#ifndef FB41_H_
#define FB41_H_

#ifdef   FB41_C_
#define  PID_EXT
#else
#define  PID_EXT extern
#endif
#define REAL float
#define TIME float
#define WORD unsigned int 
#define BOOL _Bool
#define TRUE 1
#define FALSE 0
/*
*********************************************************************************************************
*                                             СТРУКТУРА ПИД
*********************************************************************************************************
*/

//VAR_INPUT
PID_EXT  BOOL  COM_RST  ;//: BOOL ;                    //Полный рестарт
PID_EXT  BOOL  MAN_ON   ;//: BOOL  := TRUE;            //Ручной режим включить
PID_EXT  BOOL  PVPER_ON ;//: BOOL ;                    //Чтение входной переменной с периферии включить
PID_EXT  BOOL  P_SEL    ;//: BOOL  := TRUE;            //Пропорциональную составляющую включить
PID_EXT  BOOL  I_SEL    ;//: BOOL  := TRUE;            //Интегральную составляющую включить
PID_EXT  BOOL  INT_HOLD ;//: BOOL ;                    //Удержание интегральной составляющей
PID_EXT  BOOL  I_ITL_ON ;//: BOOL ;                    //Инициализировать интегральную составляющую
PID_EXT  BOOL  D_SEL    ;//: BOOL ;                    //Дифференциальную составляющую включить
PID_EXT  TIME  CYCLE    ;//: TIME  := T#1S;            //Время выполнения блока
PID_EXT  REAL  SP_INT   ;//: REAL ;                    //Внутреннее задание
PID_EXT  REAL  PV_IN    ;//: REAL ;                    //Входная переменная
PID_EXT  WORD  PV_PER   ;//: WORD ;                    //Входная переменная (периферия)
PID_EXT  REAL  MAN      ;//: REAL ;                    //Ручной выход
PID_EXT  REAL  GAIN     ;//: REAL  := 2.000000e+000;   //Коэффициент пропорциональности
PID_EXT  TIME  TI       ;//: TIME  := T#20S;           //Время интегрирования
PID_EXT  TIME  TD       ;//: TIME  := T#10S;           //Время дифференцирования
PID_EXT  TIME  TM_LAG   ;//: TIME  := T#2S;            //Время действия дифференциальной составляющей
PID_EXT  REAL  DEADB_W  ;//: REAL ;                    //Ширина зоны нечувствительности
PID_EXT  REAL  LMN_HLM  ;//: REAL  := 1.000000e+002;   //Верхний предел выходного сигнала
PID_EXT  REAL  LMN_LLM  ;//: REAL ;                    //Нижний предел выходного сигнала
PID_EXT  REAL  PV_FAC   ;//: REAL  := 1.000000e+000;   //Коэффициент для корректировки входной переменной (умножение)
PID_EXT  REAL  PV_OFF   ;//: REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
PID_EXT  REAL  LMN_FAC  ;//: REAL  := 1.000000e+000;   //Коэффициент для корректировки выходной переменной (умножение)
PID_EXT  REAL  LMN_OFF  ;//: REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
PID_EXT  REAL  I_ITLVAL ;//: REAL ;                    //Начальное значение интегральной составляющей
PID_EXT  REAL  DISV     ;//: REAL ;                    //Возмущающая переменная
//END_VAR
//VAR_OUTPUT
PID_EXT  REAL LMN       ;//: REAL ;                    //Выходное значение
PID_EXT  WORD LMN_PER   ;//: WORD ;                    //Выходное значение (переферия)
PID_EXT  BOOL QLMN_HLM  ;//: BOOL ;                    //Верхний предел выхода достигнут
PID_EXT  BOOL QLMN_LLM  ;//: BOOL ;                    //Нижний предел выхода достигнут
PID_EXT  REAL LMN_P     ;//: REAL ;                    //Пропорциональная составляющая
PID_EXT  REAL LMN_I     ;//: REAL ;                    //Интегральная составляющая
PID_EXT  REAL LMN_D     ;//: REAL ;                    //Дифференциальная составляющая
PID_EXT  REAL PV        ;//: REAL ;                    //Входная переменная
PID_EXT  REAL ER        ;//: REAL ;                    //Сигнал рассогласования
//END_VAR


void  FC41 (void);                 /* Расчет ПИД                               */
void  FC41_Init (void);            /* Инициализвция ПИД                               */


#endif
