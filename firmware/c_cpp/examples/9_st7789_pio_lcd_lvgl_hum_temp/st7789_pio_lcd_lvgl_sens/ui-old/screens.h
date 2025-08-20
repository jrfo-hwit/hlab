#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include "../lvgl/lvgl.h" // For LVGL functions and types

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *temp_label;
    lv_obj_t *temp_arc;
    lv_obj_t *temp_val;
    lv_obj_t *humd_label;
    lv_obj_t *humd_arc;
    lv_obj_t *humd_val;
    lv_obj_t *dew_label;
    lv_obj_t *dew_val;
    lv_obj_t *temp_img;
    lv_obj_t *humd_img;
    lv_obj_t *hwit_img;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/