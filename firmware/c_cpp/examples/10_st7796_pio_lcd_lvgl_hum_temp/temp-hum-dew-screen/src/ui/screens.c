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
    lv_obj_set_size(obj, 480, 320);
    {
        lv_obj_t *parent_obj = obj;
        {
            // temp_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_label = obj;
            lv_obj_set_pos(obj, 21, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Temperature");
        }
        {
            // temp_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.temp_arc = obj;
            lv_obj_set_pos(obj, 26, 48);
            lv_obj_set_size(obj, 86, 94);
            lv_arc_set_value(obj, 25);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff500000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xffff0000), LV_PART_INDICATOR | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffff0000), LV_PART_KNOB | LV_STATE_DEFAULT);
        }
        {
            // temp_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_val = obj;
            lv_obj_set_pos(obj, 24, 263);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // humd_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humd_label = obj;
            lv_obj_set_pos(obj, 378, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Humidity");
        }
        {
            // humd_arc
            lv_obj_t *obj = lv_arc_create(parent_obj);
            objects.humd_arc = obj;
            lv_obj_set_pos(obj, 369, 48);
            lv_obj_set_size(obj, 86, 94);
            lv_arc_set_value(obj, 25);
            lv_obj_set_style_arc_color(obj, lv_color_hex(0xff2196f3), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        }
        {
            // humd_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.humd_val = obj;
            lv_obj_set_pos(obj, 367, 263);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // dew_val
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dew_val = obj;
            lv_obj_set_pos(obj, 196, 263);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_32, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "99.9C");
        }
        {
            // temp_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.temp_img = obj;
            lv_obj_set_pos(obj, 31, 184);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_temperature);
        }
        {
            // humd_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.humd_img = obj;
            lv_obj_set_pos(obj, 380, 184);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_humidity);
        }
        {
            // hwit_img
            lv_obj_t *obj = lv_image_create(parent_obj);
            objects.hwit_img = obj;
            lv_obj_set_pos(obj, 190, 58);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_hwit);
        }
        {
            lv_obj_t *obj = lv_image_create(parent_obj);
            lv_obj_set_pos(obj, 208, 184);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_image_set_src(obj, &img_dew);
        }
        {
            // dew_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.dew_label = obj;
            lv_obj_set_pos(obj, 203, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Dew Point");
        }
        {
            // forward
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.forward = obj;
            lv_obj_set_pos(obj, 444, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "B >>");
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_graph() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.graph = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 480, 320);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(obj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // back
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.back = obj;
            lv_obj_set_pos(obj, 7, 7);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "<< A");
        }
        {
            // chart
            lv_obj_t *obj = lv_chart_create(parent_obj);
            objects.chart = obj;
            lv_obj_set_pos(obj, 10, 37);
            lv_obj_set_size(obj, 394, 240);
        }
        {
            // temp_label_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.temp_label_chart = obj;
            lv_obj_set_pos(obj, 56, 290);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Temperature");
        }
        {
            // hum_label_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.hum_label_chart = obj;
            lv_obj_set_pos(obj, 340, 290);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff0080ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "Humidity");
        }
        {
            // hum_led
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.hum_led = obj;
            lv_obj_set_pos(obj, 299, 290);
            lv_obj_set_size(obj, 16, 16);
            lv_led_set_color(obj, lv_color_hex(0xff0080ff));
            lv_led_set_brightness(obj, 255);
        }
        {
            // temp_led
            lv_obj_t *obj = lv_led_create(parent_obj);
            objects.temp_led = obj;
            lv_obj_set_pos(obj, 14, 290);
            lv_obj_set_size(obj, 16, 16);
            lv_led_set_color(obj, lv_color_hex(0xffff0000));
            lv_led_set_brightness(obj, 255);
        }
        {
            // scale_change
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.scale_change = obj;
            lv_obj_set_pos(obj, 350, 7);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "B = Scale Change");
        }
        {
            // min_hum_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.min_hum_chart = obj;
            lv_obj_set_pos(obj, 450, 261);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff0080ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "10%");
        }
        {
            // min_temp_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.min_temp_chart = obj;
            lv_obj_set_pos(obj, 416, 261);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "0°C");
        }
        {
            // max_hum_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.max_hum_chart = obj;
            lv_obj_set_pos(obj, 446, 37);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff0080ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "90%");
        }
        {
            // max_temp_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.max_temp_chart = obj;
            lv_obj_set_pos(obj, 408, 37);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "50°C");
        }
        {
            // mid_temp_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.mid_temp_chart = obj;
            lv_obj_set_pos(obj, 409, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "25°C");
        }
        {
            // mid_hum_chart
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.mid_hum_chart = obj;
            lv_obj_set_pos(obj, 447, 152);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xff0080ff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(obj, "50%");
        }
        {
            // percentage_to_30min_label
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.percentage_to_30min_label = obj;
            lv_obj_set_pos(obj, 189, 7);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "100%");
        }
    }
    
    tick_screen_graph();
}

void tick_screen_graph() {
}



typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_graph,
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
    create_screen_graph();
}
