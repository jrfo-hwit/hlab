#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <../lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *graph;
    lv_obj_t *temp_label;
    lv_obj_t *temp_arc;
    lv_obj_t *temp_val;
    lv_obj_t *humd_label;
    lv_obj_t *humd_arc;
    lv_obj_t *humd_val;
    lv_obj_t *dew_val;
    lv_obj_t *temp_img;
    lv_obj_t *humd_img;
    lv_obj_t *hwit_img;
    lv_obj_t *dew_label;
    lv_obj_t *forward;
    lv_obj_t *back;
    lv_obj_t *chart;
    lv_obj_t *temp_label_chart;
    lv_obj_t *hum_label_chart;
    lv_obj_t *hum_led;
    lv_obj_t *temp_led;
    lv_obj_t *scale_change;
    lv_obj_t *min_hum_chart;
    lv_obj_t *min_temp_chart;
    lv_obj_t *max_hum_chart;
    lv_obj_t *max_temp_chart;
    lv_obj_t *mid_temp_chart;
    lv_obj_t *mid_hum_chart;
    lv_obj_t *percentage_to_30min_label;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_GRAPH = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_graph();
void tick_screen_graph();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/