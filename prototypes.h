#ifndef __PROTOTYPES_H__

#define __PROTOTYPES_H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

/* ************************************ Прототипы функций *********************************************/

// my_lib.c

QUEUE * create_queue();
int destroy_queue(QUEUE * q);
int enqueue(QUEUE * q, queueT a);
queueT dequeue(QUEUE * q);
void clear_queue(QUEUE * q);
void print_queue(QUEUE * q);
int is_empty_queue(QUEUE * q);



// func.c

int are_coord_corr(double x, double y);
double check_is_near(double x1, double y1, double x2, double y2, double r, double resolution);

detector * create_detector(int N, double size, double pos_x);
int destroy_detector(detector ** d);

field * create_field(detector * d);
int destroy_field(field ** f);


sources * create_sources_arr(int size);
source * create_source(field * f, detector * d, double pos_x, double pos_y, double t, sources * ss);
int destroy_source(source ** s);


//waves * init_waves(int N);
wave * create_wave(double center_x, double center_y, double t0, double n_x, double n_y, QUEUE * ws, double ampl);
void reflect_wave(QUEUE * ws, wave * w, source * s, double t);
void destroy_wave(wave * w);



// graph.c

ALLEGRO_DISPLAY * init_window();
int draw_wave(ALLEGRO_DISPLAY * display, wave * w);
int move_time(QUEUE * ws, sources * ss, double t, ALLEGRO_DISPLAY * display, int num, field * f, detector * d);
int draw_static_elements(ALLEGRO_DISPLAY * display, field * f, detector * d, sources * ss);

#endif
