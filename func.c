#ifndef __FUNC_C__
#define __FUNC_C__


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "prototypes.h"



/* ********************* Функции ****************************** */

// функция проверки координат на корректность
int are_coord_corr(double x, double y){
    return 1;
}
// вренее, проверяем, находятся ли данные точки в зоне досягаемости волны в течение одного временного шага
double check_is_near(double x1, double y1, double x2, double y2, double r, double resolution){
    // просто находим расстояние между заданными точками
    // часто будем использовать, поэтому лучше в отдельную функцию вынести

    // ага! вот здесь проблема была! по такой функции, волна будет постоянно отражаться от одного источника!

    if(!are_coord_corr(x1, y1) || !are_coord_corr(x2, y2))
        return -1;

    double d = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    // нужно по модулю сомтреть, распиши формулы. То есть случая
    if(d >= r * r && d <= (r + resolution) * (r + resolution))
        return 1;
    else if (d < r *r && d > (r - resolution) * (r - resolution))
        return 1;
    else
        return 0;
    // то есть, формально говоря, мы по сферическому сектору смотрим

}


detector * create_detector(int N, double size, double pos_x){

    if(!are_coord_corr(pos_x, 0)){
        return NULL;
    }

    detector * det = (detector *)malloc(sizeof(detector));

    det->N = N;
    det->size = size;

    det->pos_x = pos_x;

    return det;
}

int destroy_detector(detector ** d){
    // Указатель на указатель, чтобы мы могли изменить тот, что в мэине, а не его копию
    // поэтому еще одно разыменование нужно
    free(*d);
    return 0;
}


field * create_field(detector * d){

    field * f = (field *)malloc(sizeof(field));
    f->d = d;


    return f;
}

int destroy_field(field ** f){
    free((*f)->d);
    free(*f);
    return 0;
}


sources * create_sources_arr(int size){
    // передаем предполагаемое количество источников
    sources * ss = (sources *)malloc(sizeof(sources));
    if(!ss){

    }

    if(size <= 0)
        size = 10;
    // стартовое количество. Чтобы с реаллоком не баловаться

    ss->n = 0;
    ss->SOURCES = (source **)malloc(size * sizeof(source *));

    return ss;

}
// важно понимать логику. Мы создали очередь волн, так как их нужно периодически пересчитывать, нарасчивать и т. д. Источники же далеко не такая динамическая штука (то есть не будут так активно меняться в ходе программы), поэтому для них сгодиться и обычный массив (динамический, понятно : )


source * create_source(field * f, detector * d, double pos_x, double pos_y, double t, sources * ss){
    // быстрее передавать в функцию указатели, чем структуры
    source * s = (source *)malloc(sizeof(source));

    if(!are_coord_corr(pos_x, pos_y)){
        return NULL;
    }

    // пока попытаемся обойтись без перечислимого типа :)

    s->pos_y = pos_y;
    s->pos_x = pos_x;

    s->f = f;

    ss->SOURCES[ss->n] = s;
    ss->n++;
    // чтобы не добавлялось все в одну ячейку

    return s;
}

int destroy_source(source ** s){

    if(s == NULL || *s == NULL)
        return 0;

    free(*s);

    return 0;
    // само поле, понятно, не нужно удалять, к счастью :)
}


wave * create_wave(double center_x, double center_y, double t0, double n_x, double n_y, QUEUE * ws, double ampl){

    // вообще, так как нам все волны придется просчитывать, андо создать огромный их массив, по которому мы потом будем бегать для нахождения времен прихода к каждому из датчиков всех волн
    // то есть источник уже должен быть "немного" да инициализирован :)

    // последний аргумент - начальная амплитуда данной волны

    if(are_coord_corr(center_x, center_y) == 0)
        return NULL;

    wave * w = (wave *)malloc(sizeof(wave));

    w->center_x = center_x;
    w->center_y = center_y;
    w->t0 = t0;

    w->r = SPEED * TIME_STEP / 2;
    // в момент создания волна - в точке испускания. НО! ставим начальный радиус равный его приращению за временнойшаг, чтобы не было нулевого значения в формуле для учета затухания.
    // да и вообще, понятно, модель точечного истоничка не особо применима на расстояниях порядка радиуса. Едиственгое что, можно приближение сферически симметричного источника рассмотреть. ага! тогда даже половинку приращения надо ставить, типа "эфф" (см далее!) радиус источника. И "начальная" амплитуда отраженной волны - на поверхности этого сферического источника.

    w->ampl = ampl;

    w->n_x = n_x;
    w->n_y = n_y;

    // и осталось просто внести нашу новую волну в очередь

    enqueue(ws, w);

    return w;

}

void reflect_wave(QUEUE * ws, wave * w, source * s, double t){
    // отражаем волну от точечного источника
    // потом эту функцию много где вызывать будем
    // волна не "гибнет", а возникает новая, понятно, которая на неt накладывается
    // вынесли в отдельную функцию, чтобы потом было проще что-нибудь новое добавлять

    create_wave(s->pos_x, s->pos_y, t, w->center_x - s->pos_x, w->center_y - s->pos_y, ws, REFLECTION_COEF * w->ampl);
    // вот здесь видно, чем хорошо координатное представление "ветора направления" волны ;)

    // напоминаем, ws - массив, в котором содержатся все волны
    // хм:) сложна! потом сюда мб добавить изменение амплитуды при отражении
    // и графику, конечно

}

// функция удаления волны. Будем использовать, если волна вышла за пределы поля (считаем стенки поглощающими), либо (впоследствии, если необходимо), при учете затухания, чтобы бесконечно много волн не плодить  :)
void destroy_wave(wave * w){
    // пока что все  очень просто, но для хорошей структуры программы лучше вынести это все в отдельную функцию
    // понятно, из очерели она ее не удаляет
    free(w);
}


#endif
