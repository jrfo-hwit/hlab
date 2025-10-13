#include "images.h"

const ext_img_desc_t images[4] = {
    { "humidity", &img_humidity },
    { "temperature", &img_temperature },
    { "hwit", &img_hwit },
    { "dew", &img_dew },
};
