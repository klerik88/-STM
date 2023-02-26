//*****************************************************************************
// File Name    : FB41.c
//	PID = 1.65mS
// Author       : Klerik88
// Created      : 06.02.2023
// Revised      : 06.02.2023
// Version      : 1.1
// Target MCU   : Stm32
//*****************************************************************************
#define FB41_C_
#define REAL float
#define BOOL _Bool
#define TRUE 1
#define FALSE 0
#include "FB41.h"

//*****************************************************************************
/*
VAR_INPUT
  COM_RST               : BOOL ;                    //Полный рестарт
  MAN_ON                : BOOL  := TRUE;            //Ручной режим включить
  PVPER_ON              : BOOL ;                    //Чтение входной переменной с периферии включить
  P_SEL                 : BOOL  := TRUE;            //Пропорциональную составляющую включить
  I_SEL                 : BOOL  := TRUE;            //Интегральную составляющую включить
  INT_HOLD              : BOOL ;                    //Удержание интегральной составляющей
  I_ITL_ON              : BOOL ;                    //Инициализировать интегральную составляющую
  D_SEL                 : BOOL ;                    //Дифференциальную составляющую включить
  CYCLE                 : TIME  := T#1S;            //Время выполнения блока
  SP_INT                : REAL ;                    //Внутреннее задание
  PV_IN                 : REAL ;                    //Входная переменная
  PV_PER                : WORD ;                    //Входная переменная (периферия)
  MAN                   : REAL ;                    //Ручной выход
  GAIN                  : REAL  := 2.000000e+000;   //Коэффициент пропорциональности
  TI                    : TIME  := T#20S;           //Время интегрирования
  TD                    : TIME  := T#10S;           //Время дифференцирования
  TM_LAG                : TIME  := T#2S;            //Время действия дифференциальной составляющей
  DEADB_W               : REAL ;                    //Ширина зоны нечувствительности
  LMN_HLM               : REAL  := 1.000000e+002;   //Верхний предел выходного сигнала
  LMN_LLM               : REAL ;                    //Нижний предел выходного сигнала
  PV_FAC                : REAL  := 1.000000e+000;   //Коэффициент для корректировки входной переменной (умножение)
  PV_OFF                : REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
  LMN_FAC               : REAL  := 1.000000e+000;   //Коэффициент для корректировки выходной переменной (умножение)
  LMN_OFF               : REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
  I_ITLVAL              : REAL ;                    //Начальное значение интегральной составляющей
  DISV                  : REAL ;                    //Возмущающая переменная
END_VAR
VAR_OUTPUT
  LMN                   : REAL ;                    //Выходное значение
  LMN_PER               : WORD ;                    //Выходное значение (переферия)
  QLMN_HLM              : BOOL ;                    //Верхний предел выхода достигнут
  QLMN_LLM              : BOOL ;                    //Нижний предел выхода достигнут
  LMN_P                 : REAL ;                    //Пропорциональная составляющая
  LMN_I                 : REAL ;                    //Интегральная составляющая
  LMN_D                 : REAL ;                    //Дифференциальная составляющая
  PV                    : REAL ;                    //Входная переменная
  ER                    : REAL ;                    //Сигнал рассогласования
END_VAR

*/

//VAR
REAL  sInvAlt         ;//: REAL ;
REAL  sIanteilAlt     ;//: REAL ;
REAL  sRestInt        ;//: REAL ;
REAL  sRestDif        ;//: REAL ;
REAL  sRueck          ;//: REAL ;
REAL  sLmn            ;//: REAL ;
BOOL  sbArwHLmOn      ;//: BOOL ;                    //Выход достиг максимального значения
BOOL  sbArwLLmOn      ;//: BOOL ;                    //Выход достиг минимального значения
BOOL  sbILimOn = TRUE ;//: BOOL  := TRUE;
//END_VAR

//VAR_TEMP
REAL  Hvar            ;//: REAL ;                    //Hilfsvariable
REAL  rCycle          ;//: REAL ;                    //Abtastzeit in real
REAL  Diff            ;//: REAL ;                    //Изменение значения (Anderungswert)
REAL  Istwert         ;//: REAL ;                    //Istwert
REAL  ErKp            ;//: REAL ;                    //Вспомогательная переменная
REAL  rTi             ;//: REAL ;                    //Время интегрирования (Integrationszeit) in real
REAL  rTd             ;//: REAL ;                    //Differentiationszeit in real
REAL  rTmLag          ;//: REAL ;                    //Время задержки (Verzцgerungszeit) in real
REAL  Panteil         ;//: REAL ;                    //P-Anteil
REAL  Ianteil         ;//: REAL ;                    //I-Anteil
REAL  Danteil         ;//: REAL ;                    //D-Anteil
REAL  Verstaerk       ;//: REAL ;                    //Усиление (Verstдrkung)
REAL  RueckDiff       ;//: REAL ;                    //Разница в значении обратной связи (Differenz des Rьckkopplungswertes)
REAL  RueckAlt        ;//: REAL ;                    //давность, старый (Alter) Обратная связь свободный (Rьckkopplungswert)
REAL  dLmn            ;//: REAL ;                    //Заданное значение (Stellwert)
REAL  gf              ;//: REAL ;                    //Hilfwert
REAL  rVal            ;//: REAL ;                    //Real Hilfsvariable
//END_VAR

/*****************************************************************************************
*  Описание
* param:
*
* return:
*****************************************************************************************/
//BEGIN
void FC41 (void)
{
//IF COM_RST                                          // Полный рестарт
	if (COM_RST == TRUE) {
//THEN
    // Обнуление переменных
    sIanteilAlt = I_ITLVAL;
    LMN = 0.0;                                       //Выходное значение
    QLMN_HLM = 0;                                    //Верхний предел выхода достигнут
    QLMN_LLM = 0;                                    //Нижний предел выхода достигнут
    LMN_P = 0.0;                                     //Пропорциональная составляющая
    LMN_I = 0.0;                                     //Интегральная составляющая
    LMN_D = 0.0;                                     //Дифференциальная составляющая
    LMN_PER = 0;                                     //Выходное значение (периферия)
    PV = 0.0;                                        //Входная переменная
    ER = 0.0;                                        //Сигнал рассогласования
    sInvAlt = 0.0;
    sRestInt = 0.0;
    sRestDif = 0.0;
    sRueck = 0.0;
    sLmn = 0.0;
    sbArwHLmOn = 0;
    sbArwLLmOn = 0;
}
//ELSE
	else {
//  rCycle = DINT_TO_REAL(TIME_TO_DINT(CYCLE)) / 1000.0;            // Время выполнения блока в секундах
    rCycle = CYCLE;            // Время выполнения блока в секундах
//  Istwert = INT_TO_REAL(WORD_TO_INT(PV_PER)) * 100.0 / 27648.0;    // Получили входную переменную от периферии (0-27648 в 0-100)
    Istwert = PV_PER * 100.0 / 27648.0;    // Получили входную переменную от периферии (0-27648 в 0-100)
    Istwert = Istwert * PV_FAC + PV_OFF;                             // Скорректировали входную переменную
//    IF NOT PVPER_ON THEN Istwert = PV_IN; END_IF;                    // Если периферийный вход отключен берем переменную из PV_IN
    if (!PVPER_ON)
    	{Istwert = PV_IN;}                                           // Если периферийный вход отключен берем переменную из PV_IN
    PV = Istwert;                                                  // Входная переменная
    ErKp = SP_INT - PV;                                              // Получили рассогласование между заданием и входом

//    IF     ErKp < (-DEADB_W)     THEN ER = ErKp+DEADB_W;             // Если рассогласование больше зоны нечувствительности
//    ELSIF  ErKp >   DEADB_W      THEN ER = ErKp-DEADB_W;             // уменьшаем рассогласование на величину зоны нечуств.
//    ELSE                              ER = 0.0;                      // иначе принимаем рассогласование равным нулю.
//    END_IF;
    if (ErKp < (-DEADB_W))
    	ER = ErKp + DEADB_W;             // Если рассогласование больше зоны нечувствительности
    else if (ErKp > DEADB_W)
    	ER = ErKp - DEADB_W;               // уменьшаем рассогласование на величину зоны нечуств.
    else
    	ER = 0.0;                      // иначе принимаем рассогласование равным нулю.

    ErKp = ER * GAIN;                                                // Рассогласование, умноженное на кфт пропорциональности
//  rTi = DINT_TO_REAL(TIME_TO_DINT(TI)) / 1000.0;                   // Время интегрирования в секундах
    rTi = TI;                   // Время интегрирования в секундах
//  rTd = DINT_TO_REAL(TIME_TO_DINT(TD)) / 1000.0;                   // Время дифференцирования в секундах
    rTd = TD;                   // Время дифференцирования в секундах
//  rTmLag = DINT_TO_REAL(TIME_TO_DINT(TM_LAG)) / 1000.0;            // Время действия диф.составляющей в секундах
    rTmLag = TM_LAG;            // Время действия диф.составляющей в секундах
    // Проверка на допустимость временных настроек регулятора
//    IF rTi < (rCycle * 0.5)     THEN    rTi = rCycle * 0.5;      END_IF; // Минимальное время интегрирования - Cycle / 2
//    IF rTd < rCycle             THEN    rTd = rCycle;            END_IF; // Минимальное время дифференцирования - Cycle
//    IF rTmLag < rCycle * 0.5    THEN    rTmLag = rCycle * 0.5;   END_IF; // Минимальное время действия диф.сост.- Cycle / 2
    if (rTi < (rCycle * 0.5))
    	rTi = rCycle * 0.5;             // Минимальное время интегрирования - Cycle / 2
    if (rTd < rCycle)
    	rTd = rCycle;                   // Минимальное время дифференцирования - Cycle
    if (rTmLag < rCycle * 0.5)
    	rTmLag = rCycle * 0.5;          // Минимальное время действия диф.сост.- Cycle / 2

    // Вычисление пропорциональной составляющей
//    IF P_SEL
//    THEN
//        Panteil = ErKp;                                              // Если выбрана пропорциональная составляющая
//    ELSE
//        Panteil = 0.0;
//    END_IF;
    if (P_SEL == TRUE)
        Panteil = ErKp;                                              // Если выбрана пропорциональная составляющая
    else
        Panteil = 0.0;

    //---------------------------------------------------------------------------------------------
    // Вычисление интегральной составляющей
    //---------------------------------------------------------------------------------------------
//  IF I_SEL                    //Интегральную составляющую включить
		if (I_SEL == TRUE) {
//  THEN
//      IF I_ITL_ON             //Инициализировать интегральную составляющую
        if (I_ITL_ON == TRUE) {             //Инициализировать интегральную составляющую
//      THEN
            Ianteil = I_ITLVAL;
            sRestInt = 0.0;
        }
//      ELSE
        else {
//          IF MAN_ON           //Ручной режим
            if (MAN_ON == TRUE) {           //Ручной режим
//          THEN
                Ianteil = sLmn - Panteil - DISV;
                sRestInt = 0.0;
            }
//          ELSE                //Автоматический режим
            else {                //Автоматический режим
                Diff = rCycle / rTi *(ErKp + sInvAlt) *  0.5 + sRestInt;
//              IF ((Diff>0.0) AND sbArwHLmOn OR INT_HOLD)OR ((Diff<0.0) AND sbArwLLmOn) THEN Diff = 0.0; END_IF;
                if (( (Diff>0.0) & sbArwHLmOn || INT_HOLD) || ((Diff<0.0) && sbArwLLmOn))
                    Diff = 0.0;
                Ianteil = sIanteilAlt + Diff;
                sRestInt = sIanteilAlt - Ianteil + Diff;
//          END_IF;
            }
//      END_IF;
        }
    }
//  ELSE
    else {
        Ianteil = 0.0;
        sRestInt = 0.0;
    }
//  END_IF;

    //---------------------------------------------------------------------------------------------
    //Формирование дифференциальной составляющей
    //---------------------------------------------------------------------------------------------
    Diff = ErKp;
//  IF (NOT MAN_ON) AND D_SEL
    if ((!MAN_ON) && D_SEL) {
//  THEN
        Verstaerk = rTd / (rCycle * 0.5 + rTmLag);
        Danteil = (Diff - sRueck) * Verstaerk;
        RueckAlt = sRueck;
        RueckDiff = rCycle / rTd * Danteil + sRestDif;
        sRueck = RueckDiff + RueckAlt;
        sRestDif = RueckAlt - sRueck + RueckDiff;
    }
//    ELSE
    else {
        Danteil = 0.0;
        sRestDif = 0.0;
        sRueck = Diff;
    }
//    END_IF;
    //---------------------------------------------------------------------------------------------
    //Формирование выхода
    //---------------------------------------------------------------------------------------------
    dLmn = Panteil + Ianteil + Danteil + DISV;
//  IF MAN_ON           // Если ручной режим
    if (MAN_ON == TRUE) {          // Если ручной режим
//  THEN
        dLmn = MAN;      // Ручной выход
    }
//  ELSE
    else {
//      IF (NOT I_ITL_ON) AND I_SEL
        if ((!I_ITL_ON) && I_SEL) {
//      THEN
//          IF (Ianteil > (LMN_HLM - DISV)) AND (dLmn > LMN_HLM) AND ((dLmn - LMN_D)> LMN_HLM)
            if ((Ianteil > (LMN_HLM - DISV)) && (dLmn > LMN_HLM) && ((dLmn - LMN_D)> LMN_HLM)) {
//          THEN
                rVal = LMN_HLM - DISV;
                gf = dLmn - LMN_HLM;
                rVal = Ianteil - rVal;
//              IF  rVal > gf THEN rVal = gf; END_IF;
                if (rVal > gf)
                    rVal = gf;
                Ianteil = Ianteil - rVal;
            }
//          ELSE
            else {
//              IF (Ianteil < (LMN_LLM - DISV)) AND (dLmn < LMN_LLM) AND ((dLmn - LMN_D) < LMN_LLM)
                if ((Ianteil < (LMN_LLM - DISV)) && (dLmn < LMN_LLM) && ((dLmn - LMN_D) < LMN_LLM)) {
//              THEN
                    rVal = LMN_LLM - DISV;
                    gf = dLmn - LMN_LLM;
                    rVal = Ianteil - rVal;
//                  IF rVal < gf  THEN rVal = gf; END_IF;
                    if (rVal < gf)
                        rVal = gf;
                    Ianteil = Ianteil - rVal;
//              END_IF;
                }
//          END_IF;
            }
//      END_IF;
        }
//  END_IF;
    }

    LMN_P = Panteil;
    LMN_I = Ianteil;
    LMN_D = Danteil;
    sInvAlt = ErKp;
    sIanteilAlt = Ianteil;
    sbArwHLmOn = 0;
    sbArwLLmOn = 0;

//  IF (dLmn >= LMN_HLM)
    if (dLmn >= LMN_HLM) {
//  THEN
        QLMN_HLM = 1;
        QLMN_LLM = 0;
        dLmn = LMN_HLM;
        sbArwHLmOn = 1;
    }
//  ELSE
    else {
        QLMN_HLM = 0;
//      IF (dLmn <= LMN_LLM)
        if (dLmn <= LMN_LLM) {
//        THEN
            QLMN_LLM = 1;
            dLmn = LMN_LLM;
            sbArwLLmOn = 1;
        }
//        ELSE
        else {
            QLMN_LLM = 0;
//      END_IF;
        }
//  END_IF;
    }

    sLmn = dLmn;
    dLmn = sLmn * LMN_FAC + LMN_OFF;
    LMN = dLmn;
    dLmn = LMN * 2.764800e+002;

//  IF dLmn >= 3.251100e+004
    if (dLmn >= 3.251100e+004)
//  THEN
        dLmn = 3.251100e+004;
//    ELSE
    else
//      IF dLmn <= -3.251200e+004
        if (dLmn <= -3.251200e+004)
//      THEN
            dLmn = -3.251200e+004;
//      END_IF;
//  END_IF;

//  LMN_PER = INT_TO_WORD(REAL_TO_INT(dLmn));
    LMN_PER = (int)dLmn;
//END_IF;
	}
//END_FUNCTION_BLOCK
}


/*****************************************************************************************
* Описание
* param:
*
* return:
*****************************************************************************************/
void FC41_Init (void)
{
    COM_RST  = TRUE      ;//: BOOL ;                    //Полный рестарт
    MAN_ON   = FALSE     ;//: BOOL  := TRUE;            //Ручной режим включить
    PVPER_ON = FALSE     ;//: BOOL ;                    //Чтение входной переменной с периферии включить
    P_SEL    = TRUE      ;//: BOOL  := TRUE;            //Пропорциональную составляющую включить
    I_SEL    = TRUE      ;//: BOOL  := TRUE;            //Интегральную составляющую включить
    INT_HOLD = FALSE     ;//: BOOL ;                    //Удержание интегральной составляющей
    I_ITL_ON = FALSE     ;//: BOOL ;                    //Инициализировать интегральную составляющую
    D_SEL    = TRUE      ;//: BOOL ;                     //Дифференциальную составляющую включить
    CYCLE    = 1.0       ;//: TIME  := T#1S;            //Время выполнения блока
    SP_INT   = 0.0       ;//: REAL ;                    //Внутреннее задание
    PV_IN    = 0.0       ;//: REAL ;                    //Входная переменная
    PV_PER   = 0         ;//: WORD ;                    //Входная переменная (периферия)
    MAN      = 0.0       ;//: REAL ;                    //Ручной выход
    GAIN     = 0.01      ;//: REAL  := 2.000000e+000;   //Коэффициент пропорциональности
    TI       = 0.0       ;//: TIME  := T#20S;           //Время интегрирования
    TD       = 0.0       ;//: TIME  := T#10S;           //Время дифференцирования
    TM_LAG   = 2.0       ;//: TIME  := T#2S;            //Время действия дифференциальной составляющей
    DEADB_W  = 0.0       ;//: REAL ;                    //Ширина зоны нечувствительности
    LMN_HLM  = 95.0     ;//: REAL  := 1.000000e+002;   //Верхний предел выходного сигнала
    LMN_LLM  = 1.0       ;//: REAL ;                    //Нижний предел выходного сигнала
    PV_FAC   = 1.0       ;//: REAL  := 1.000000e+000;   //Коэффициент для корректировки входной переменной (умножение)
    PV_OFF   = 0.0       ;//: REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
    LMN_FAC  = 10        ;//: REAL  := 1.000000e+000;   //Коэффициент для корректировки выходной переменной (умножение)
    LMN_OFF  = 0.0       ;//: REAL ;                    //Коэффициент для корректировки входной переменной (сложение)
    I_ITLVAL = 0.0       ;//: REAL ;                    //Начальное значение интегральной составляющей
    DISV     = 0.0       ;//: REAL ;                    //Возмущающая переменна
}
