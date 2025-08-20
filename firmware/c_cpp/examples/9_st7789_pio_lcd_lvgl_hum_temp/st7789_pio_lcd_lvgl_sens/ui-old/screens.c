#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;
uint32_t active_theme_index = 0;

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // temp_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_label = obj;
            lv_obj_set_pos(obj, 22, 19);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Temperature");
        }
        {
            // temp_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.temp_arc = obj;
            lv_obj_set_pos(obj, 27, 49);
            lv_obj_set_size(obj, 86, 94);
            lv_arc_set_value(obj, 25);
        }
        {
            // temp_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_val = obj;
            lv_obj_set_pos(obj, 52, 89);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // humd_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humd_label = obj;
            lv_obj_set_pos(obj, 218, 19);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Humidity");
        }
        {
            // humd_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.humd_arc = obj;
            lv_obj_set_pos(obj, 209, 50);
            lv_obj_set_size(obj, 86, 94);
            lv_arc_set_value(obj, 25);
        }
        {
            // humd_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humd_val = obj;
            lv_obj_set_pos(obj, 234, 89);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // dew_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dew_label = obj;
            lv_obj_set_pos(obj, 122, 65);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Dew Point");
        }
        {
            // dew_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dew_val = obj;
            lv_obj_set_pos(obj, 141, 89);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // temp_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.temp_img = obj;
            lv_obj_set_pos(obj, 38, 153);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_temperature);
        }
        {
            // humd_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.humd_img = obj;
            lv_obj_set_pos(obj, 222, 153);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_humidity);
        }
        {
            // hwit_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.hwit_img = obj;
            lv_obj_set_pos(obj, 110, 149);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_hwit);
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
