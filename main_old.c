#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define _USE_MATH_DEFINES
// подключаем всякие там определенные Константы gcc, пи, например :)

/* ********************* Константы ****************************** */


#define SPEED 1.5
// скорость распространения сигналов

#define ZONE 0.001
// угловой шаг обхода в радианах

#define SOURCE_SIZE 0.01
// размер источника (иначе, точки, которая лучи будет отражать)

/* ********************* Структуры и типы ****************************** */


// детектор
typedef struct _detector{
    int N;
    // число датчиков
    double size;
    // ширина детектора

    double pos_x;
    // координата левого конца детектора, игрек = нулю, по построению (т е меньшая из координат)
} detector;


// исследуемая среда
typedef struct _field{
    double lim_x;
    double lim_y;
    detector * d;
    // луше хранить детектор вместе
} field;


// лучи сигналов
typedef struct _line{
    double b;
    double a;
    double phase;
    // фаза луча, арвносильно (условные единицы), его временная задержка
    // y = - сtg a * x + b, y - от детектора, то есть больше 0 в рассматриваемой области
    // удобнее угол наклона, чем коэффициент, чтобы: 1) относительно равномерно инкрементировать, меньше ошибки, то есть 2) чтобы с бесконечностями не работать, и все лучи простраивать
} line;



// лучше, наверное, волну хранить в источнике, а не источник в волне, логичнее, по крайней мере.
// чтобы потом много источников было удобно перебирать

// но тогда, полуается, в волне - только массив, поэтому проще вообще такую структуру не создавать, то есть все хранить в источнике? да! это экономно! ПИ, что сказать :)

// источники света
typedef struct _source{
    double pos_x;
    double pos_y;
    double t;
    // время срабытывания

    field * f;
    // поле, к которому относится источник. Тоже мб весьма себе полезно и логично оставить его и здесь.
    line ** lines;
    // лучи в волне, испущенной источником
    // хранить по кускам лучше, чем в одном блоке. Не всегда в памяти один большой блок найдется

} source;


//

/* ********************* Функции ****************************** */


detector * detector_create(int N, double size, double pos_x){

    detector * det = (detector *)malloc(sizeof(detector));

    det->N = N;
    det->size = size;

    det->pos_x = pos_x;

    return det;
}

int detector_destroy(detector ** d){
    // Указатель на указатель, чтобы мы могли изменить тот, что в мэине, а не его копию
    // поэтому еще одно разыменование нужно
    free(*d);
    return 0;
}


field * field_create(double lim_x, double lim_y, detector * d){

    field * f = (field *)malloc(sizeof(field));
    f->lim_y = lim_y;
    f->lim_x = lim_x;
    f->d = d;


    return f;
}

int field_destroy(field ** f){
    free((*f)->d);
    free(*f);
    return 0;
}



line * line_create(double start_x, double start_y, double a, double init_phase){

    line * l = (line *)malloc(sizeof(line));

    // !!! a отсчитывается от OY! то есть и меньше 0 мб!!!

    l->a = a;

    l->b = start_x / tan(a) + start_y;
    // геометрия. Уравнение прямой по двум точкам

    l->phase = init_phase;

    return l;

}

int line_destroy(line ** l){
    free(*l);
    return 0;
    // само поле, понятно, не нужно удалять, к счастью :)
}


int make_wave(source ** s, detector * d){

    // то есть источник уже должен быть "немного" да инициализирован :)

    if(s == NULL || *s == NULL || (*s)->pos_x == 0 || (*s)->pos_y == 0)
        return -1;

    (*s)->lines = (line **)malloc(d->N * sizeof(line *));

    if((*s)->lines == NULL)
        return -1;


    // смотри идею ниже, для экономии ресурсов - по 1 лучу в каждый датчик
    // поэтому число лучей = число датчиков в детекторе.

    int i;
    // просто по циклу генерируем световые лучи во всех направлениях из фиксированной точки. Плюс достаточно брать a > 0 только, понятно, так как все, что пошло "вниз" (от детектора) нас не сильно интересует

    // угол инкрементирования
    // ИДЕЯ! для экономии ресурсов достаточно моделировать только те лучи, которые попадают в источник, плюс по одному лучу в детектор! А это неплохой вопрос по выбору по оптике :)))
    // а если еще всякие там отражения учесть...
    // тогда и число лучей в волен будет тоже оптимальным :)


    double a_end = atan( ( (d->pos_x) - ((*s)->pos_x) + (d->size) ) / ((*s)->pos_y) );
    // специально располагаем в таком порядке, чтобы числа были поменьше
    //
    double a_start = atan( (d->pos_x - (*s)->pos_x) / ((*s)->pos_y) );

    // Но! инкрементировать лечше по другому углу, чтобы он целиком в области определения арктангенса лежал! или... а мб просто арккотангенс взять? нет, лучше арктангенс! там числа в среднем блиде к 1 будут => точность выше.

    double step = (a_end - a_start) / d->N;


    // так! надо еще подгадать, чтобы луч примерно в центр каждого датчика попадал.
    // Поэтому где-то полшага прибавим
    double a = a_start + step / 2;

    for(i = 0; i < d->N; i++){
        a_start += step * i;
        (*s)->lines[i] = line_create((*s)->pos_x, (*s)->pos_y, (*s)->t);
        if((*s)->lines[i] == NULL)
            return -1;
        // если нет памяти
    }

    return 0;
    // ага! то есть по заданному источнику создали волну, которую он испустит в заданный момент времени
}


double * count_time(source * s, detector * d){
    double * t = (double *)malloc(d->N * sizeof(double));
    // вернем указатель на масив времен
    // блин! так нам и лучи сами, де-факто, просчитывать не надо...
    int i;

    if(t == NULL)
        return NULL;

    double delta_x;

    for(i = 0; i < d->N; i++){
        delta_x = d->pos_x - s->pos_x + d->size / d->N * i;
        t[i] = s->t + sqrt(delta_x * delta_x + s->pos_y * s->pos_y) / SPEED;
        // первое слагаемое - время срабатывания источника
    }

    return t;
}


source * source_create(field * f, detector * d, double pos_x, double pos_y, double t){
    // быстрее передавать в функцию указатели, чем структуры
    source * s = (source *)malloc(sizeof(source));

    if(pos_y > f->lim_y || pos_x > f->lim_x){
        fprintf(stderr, "TOO BIG COORD!\n");
        exit(1);
    }

    // пока попытаемся обойтись без перечислимого типа :)

    s->pos_y = pos_y;
    s->pos_x = pos_x;

    s->t = t;
    s->f = f;
    // устанавливаем расчетное время испукания и поле, к которому относится источник

    make_wave(&s, d);
    // заполняем массив лучей в структуре источника
    // передаем ССЫЛКУ на источник, так как хотим его изменить

    return s;
}

int source_destroy(source ** s){

    if(s == NULL || *s == NULL)
        return 0;

    int i;

    for(i = 0; i < (*s)->f->d->N; i++){
        line_destroy(&(*s)->lines[i]);
        // как обычно, передаем ссылку
        // если нет памяти
    }

    free((*s)->lines);
    free(*s);

    return 0;
    // само поле, понятно, не нужно удалять, к счастью :)
}


int print_times(source * s, const char * source_name, detector * d){

    char * name = (char *)malloc(10 * sizeof(char));

    if(name == NULL)
        return -1;

    sprintf(name, "logs/log_s_%s.txt", source_name);

    FILE * f = fopen(name, "a");

    fprintf(f, "START\n");

    double * t = count_time(s, d);

    int i = 0;

    for(i = 0; i < d->N; i++){
        fprintf(f, "%i\t%lf\n", i, t[i]);
    }

    fprintf(f, "END\n");

    fclose(f);

    return 0;
}

double ** compute_multiple_sources(source ** sources, int num, detector * d){

    // создадим маиссив массивов, в каждом из которых - сигналы, пришедшие на данный датчик детектора
    // num - число источников. С динамическим массивом сизеоф на пашет

    int i = 0;
    int j = 0;
    double delta_x = 0;
    int N = d->N;
    double between = d->size / N;
    source * s;

    double ** data = (double **)malloc(N * sizeof(double *));

    if(data == NULL)
        printf("NO MEMORY for 'data'!\n");

    for(i = 0; i < N; i++){
        data[i] = (double *)malloc(num * sizeof(double));
        for(j = 0; j < num; j++){
            s = sources[j];
            delta_x = d->pos_x - s->pos_x + between * i;
            data[i][j] = s->t + sqrt(delta_x * delta_x + s->pos_y * s->pos_y) / SPEED;
            // первое слагаемое - время срабатывания источника
        }

    }


    return data;
}


int print_multiple_times(source ** sources, int num, detector * d, const char * filename){

    char name[100];

   // if(name == NULL)
        //return -1;

    sprintf(name, "logs/log_%s.txt", filename);

    FILE * f = fopen(name, "a");

    if(f == NULL){
        printf("Cannot open the file!\n");
    }

    fprintf(f, "START\n");

    double ** data = compute_multiple_sources(sources, num, d);

    int i, j;

    for(j = 0; j < d->N; j++){
        // выводим по каждому детектору
        fprintf(f, "DETECTOR\t%i\n", j);

        for(i = 0; i < num; i++){
            // выводим по каждому детектору по данному источнику
            fprintf(f, "%i\t%lf\n", i, data[j][i]);
            // то есть в данной секции (своя для каждого датчика) выводятся данные со всех источников
            // первая цифра - номер источника, вторая - время прибытия
        }
    }

    fprintf(f, "END\n");

    fclose(f);

    return 0;
}

void reflect_line(line ** l){
    if(rho < SOURCE_SIZE){

    }
}


int main(){

    detector * d = detector_create(30, 10, 7.5);
    // начнем с таких параметров

    // все в сантиметрах пока :)

    field * f = field_create(30, 30, d);

    //source * s = source_create(f, d, 1, 10, 0);

    //print_times(s, "4", d);

    int i;
    source ** sources = (source **)malloc(10 * sizeof(source *));

    for(i = 0; i < 10; i++){
        sources[i] = source_create(f, d, 10, 1 + 2 * i, 5 * i);
    }

    print_multiple_times(sources, 10, d, "mult_4");

    print_times(sources[2], "test", d);

    for(i = 0; i < 10; i++){
        source_destroy(&sources[i]);
    }

    field_destroy(&f);
    detector_destroy(&d);

    // добавь возможность просчета набора точечных источников и построй получающиеся от них графики на одном бланке

    return 0;
}
