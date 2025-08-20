#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include "../lvgl/lvgl.h" // For LVGL functions and types

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_humidity;
extern const lv_img_dsc_t img_temperature;
extern const lv_img_dsc_t img_hwit;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[3];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/