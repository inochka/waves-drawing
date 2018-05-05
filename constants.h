#ifndef __CONSTANTS_H__

#define __CONSTANTS_H__

#define _USE_MATH_DEFINES
// подключаем всякие там определенные Константы gcc, пи, например :)


/* ********************* Константы ****************************** */

#define SPEED 150
// скорость распространения сигналов

#define PI 3.14159265
// число пи :)

#define SOURCE_SIZE 10
// размер источника (иначе, точки, которая лучи будет отражать)
// важно, чтобв он был согласован со скоростью и шагом по времени, потому что абсолютное попадание маловероятно, важен диапазон
// так и сделаем. а это просто останется в пикселях для графики

#define FIELD_SIZE_X 900
#define FIELD_SIZE_Y 600
// лучше всего в одном месте эти величины хранить, чтобыпросто было потом поменять (в 1 месте)
// в см

#define TIME_STEP 0.01
// задаем шаг во времени в расчетах. Модельное время, как правило, никак не совпадает с рY_еальным
// мб лучше размер источника (препятствия) сделать динамическим, то есть, так как источник точечный (в теории), то и "эффективный" размер его полностью определяется шагом во времени и скоростью волн

#define REDRAW_TIME 0.001
// время, через которое будем прорисовывать

#define OFFSET_X 25
#define OFFSET_Y 25
// отступы от края окна в графике

#define ATTENUATION 0.001
// коэффициент затухания. Без него все слишком нереалистично смотриться

#define REFLECTION_COEF 0.9
// коэффициент отражения от источника

#define MIN_AMPL 0.00001
// минимальная амплитуда волн, которые все еще будем рассматривать

#define MAX_AMPL 50.0
// максимальная ампитуда волн, которую будем вообще задавать. Нужна для красивой отрисовки затухания.

#endif
