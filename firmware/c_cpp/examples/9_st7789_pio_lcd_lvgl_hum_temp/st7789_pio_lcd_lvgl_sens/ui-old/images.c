#include "images.h"

const ext_img_desc_t images[3] = {
    { "humidity", &img_humidity },
    { "temperature", &img_temperature },
    { "hwit", &img_hwit },
};
