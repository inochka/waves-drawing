#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "prototypes.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

int main(){

    QUEUE * waves = create_queue();

    detector * d = create_detector(10, 100, (FIELD_SIZE_X - 100) / 2);
    // начнем с таких параметров

    // все в сантиметрах пока :)

    field * f = create_field(d);

    int i;
    sources * ss = create_sources_arr(20);

    for(i = 0; i < 10; i++){
        create_source(f, d, 50 + 80 * i, 100 + 40 * i, 5 * i, ss);
    }

    double r = d->size / d->N / 2;

    for(i = 0; i < d->N; i++){
        create_wave(d->pos_x + OFFSET_X + i * 2 * r + r, OFFSET_Y + r, i, 0, 1, waves, 50.0);
    }

    ALLEGRO_DISPLAY * display = init_window();

    move_time(waves, ss, 0, display, d->N, f, d);

    destroy_queue(waves);

    return 0;
}
