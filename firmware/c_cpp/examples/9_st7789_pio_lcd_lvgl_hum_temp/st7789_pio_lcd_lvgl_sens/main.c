/**
 * @file st7789_lcd_pio.c
 * @brief LVGL demo with ST7789 LCD PIO (Programmable I/O) driver.
 *
 * LVGL v9.4.0 demo program interfacing with the ST7789 LCD display
 * controller using the PIO peripheral. LVGL screen was created using
 * Eez Studio and is designed to work with the ST7789 2 inches display.
 *
 * @author Juliano Oliveira
 * @date 2025-06-10
 * @copyright (c) 2025 Hardware Innovation Technologies. All rights reserved.
 * License: MIT License (see LICENSE file for details)
 */

#include <stdio.h> // For printf

#include "pico/stdlib.h" // For stdio_init_all, sleep_us, sleep_ms
#include "hardware/gpio.h" // For gpio functions
#include "pico/cyw43_arch.h" // For Wi-Fi functions
#include "st7789_lcd_pio.h" // For ST7789 LCD PIO functions
#include "lvgl.h" // For LVGL functions
#include "pico/multicore.h" // For crictical sections
#include "ui/ui.h" // For EEZ studio LVGL generated UI functions

#include "hardware/i2c.h"
#include "aht10.h"

#define BUTTON_A_PIN 5 // Button A pin from the schematic
#define BUTTON_B_PIN 6 // Button B pin from the schematic
#define DEBOUNCE_DELAY_MS 20 // Debounce delay in milliseconds

#define NORMAL_TEMP_LOW_THRESHOLD 20.0f // Normal temperature low threshold in Celsius
#define NORMAL_TEMP_HIGH_THRESHOLD 25.0f // Normal temperature high threshold in Celsius
#define NORMAL_HUMIDITY_LOW_THRESHOLD 30.0f // Normal humidity low threshold in percentage  
#define NORMAL_HUMIDITY_HIGH_THRESHOLD 60.0f // Normal humidity high threshold in percentage
// Last known states and last debounce times
bool last_state_a = true;
bool last_state_b = true;
absolute_time_t last_debounce_time_a;
absolute_time_t last_debounce_time_b;

#define SERIAL_CLK_DIV 1.f // Serial clock divider for PIO, adjust as needed

static critical_section_t crit_sec = {0}; //< Synchronization for safe time reading
static lv_display_t * lcd_disp = NULL; //< Pointer to the LVGL display object

#define DEMO_MODE 1

/**
 * Retrieves the number of milliseconds elapsed since the system booted.
 * NEEDED BY LVGL FOR TICK HANDLING.
 * 
 * @return The number of milliseconds since the system started.
 */
uint32_t get_milliseconds_since_boot()
{
    critical_section_enter_blocking(&crit_sec);
    uint32_t ms = to_ms_since_boot(get_absolute_time());
    critical_section_exit(&crit_sec);
    return ms;
}

/**
 * @brief Callback function to flush buffered data.
 * NEEDED BY LVGL FOR DISPLAY FLUSHING.
 * 
 * @return int Returns 0 on success, or a negative value on error.
 */
void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    lcd_set_window(pio, sm, area->x1, area->x2, area->y1, area->y2);
    /* The most simple case (also the slowest) to send all rendered pixels to the
     * screen one-by-one.  `put_px` is just an example.  It needs to be implemented by you. */
    uint16_t * buf16 = (uint16_t *)px_map; /* Let's say it's a 16 bit (RGB565) display */
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //put_px(x, y, *buf16);
            st7789_lcd_put(pio, sm, *buf16 >> 8);
            st7789_lcd_put(pio, sm, *buf16 & 0xff);
            buf16++;
        }
    }

    /* IMPORTANT!!!
     * Inform LVGL that flushing is complete so buffer can be modified again. */
    lv_display_flush_ready(display);
}

/**
 * @brief Initialize the LVGL port display.
 * NEEDED BY LVGL FOR DISPLAY INITIALIZATION.
 * 
 * This function sets up the display driver and configures the necessary hardware
 * and software resources required for LVGL to render graphics on the target display.
 * It should be called during system initialization before any LVGL drawing operations.
 *
 * @return int Returns 0 on success, or a negative error code on failure.
 */
void lv_port_display_init(void)
{
    lcd_init(pio, sm, st7789_init_seq); // initialize the LCD before display driver

    /* Create the LVGL display object and the ST7789 LCD display driver */
    lcd_disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    //lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_0);   // jrfo - isso não tá funcionando 

    uint32_t buf_size = SCREEN_WIDTH * SCREEN_HEIGHT / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    buf1 = (uint8_t*)lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = (uint8_t*)lv_malloc(buf_size);
    if(buf2 == NULL) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

// Function to get the current screen ID
enum ScreensEnum get_current_screen_id(){   
    if (objects.main && lv_obj_get_screen(objects.main) == lv_scr_act()) {
        return SCREEN_ID_MAIN;
    } else if (objects.graph && lv_obj_get_screen(objects.graph) == lv_scr_act()) {
        return SCREEN_ID_GRAPH;
    }
    return SCREEN_ID_MAIN; // Default to main screen if none matches
}

enum scaleGraph {
    SCALE_MINUTE = 1,
    SCALE_HOUR = 2,
    SCALE_DAY = 3,
};

#include <float.h>  // for FLT_MAX and FLT_MIN

float array_max(const float *arr, size_t len) {
    if (len == 0) return -FLT_MAX; // or handle error
    float max_val = arr[0];
    for (size_t i = 1; i < len; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }
    return max_val;
}

float array_min(const float *arr, size_t len) {
    if (len == 0) return FLT_MAX; // or handle error
    float min_val = arr[0];
    for (size_t i = 1; i < len; i++) {
        if (arr[i] < min_val) {
            min_val = arr[i];
        }
    }
    return min_val;
}

float array_avg(const float *arr, size_t len) {
    if (len == 0) return 0.0f;  // handle empty array case
    float sum = 0.0f;
    for (size_t i = 0; i < len; i++) {
        sum += arr[i];
    }
    return sum / (float)len;
}

#define CHART_POINT_COUNT 48

/**
 * @brief Entry point of the program.
 *
 * This function serves as the main entry point for program execution.
 * It initializes necessary resources, executes the core logic, and
 * returns an exit status to the operating system.
 */
int main() {
    stdio_init_all(); // Initialize standard I/O for debugging
    
    // Initialize Button A and Button B pins
    gpio_init(BUTTON_A_PIN); // Initialize Button A pin
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN); // Set as input
    gpio_pull_up(BUTTON_A_PIN);  // Enable pull-up resistor
    gpio_init(BUTTON_B_PIN); // Initialize Button B pin
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN); // Set as input
    gpio_pull_up(BUTTON_B_PIN); // Enable pull-up resistor
    // Initialize button debounce timers
    last_debounce_time_a = get_absolute_time(); // Initialize last debounce time for Button A
    last_debounce_time_b = get_absolute_time(); // Initialize last debounce time for Button B

    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    // Initialize the PIO state machine for ST7789 LCD
    uint offset = pio_add_program(pio, &st7789_lcd_program); // Add the ST7789 LCD PIO program to the PIO state machine
    st7789_lcd_program_init(pio, sm, offset, PIN_DIN, PIN_CLK, SERIAL_CLK_DIV); // Initialize the PIO state machine for ST7789 LCD

    // Initialize GPIO pins for ST7789 LCD
    gpio_init(PIN_CS); // Chip Select pin
    gpio_init(PIN_DC); // Data/Command pin
    gpio_init(PIN_RESET); // Reset pin
    gpio_init(PIN_BL); // Backlight pin
    gpio_set_dir(PIN_CS, GPIO_OUT); // Set Chip Select pin as output
    gpio_set_dir(PIN_DC, GPIO_OUT); // Set Data/Command pin as output
    gpio_set_dir(PIN_RESET, GPIO_OUT); // Set Reset pin as output
    gpio_set_dir(PIN_BL, GPIO_OUT); // Set Backlight pin as output

    // Set initial states for GPIO pins
    gpio_put(PIN_CS, 1); // Set Chip Select pin high
    gpio_put(PIN_RESET, 1); // Set Reset pin high
    gpio_put(PIN_BL, 1); // Set Backlight pin high (turn on backlight)

    // Initialize LVGL library
    lv_init(); // Initialize LVGL library
    lv_tick_set_cb(get_milliseconds_since_boot); // Set the tick callback for LVGL
    lv_port_display_init(); // Initialize the LVGL port display
    lv_display_set_flush_cb(lcd_disp, my_flush_cb); // Set the flush callback for LVGL display

    printf("Hello, world! ST7789 LCD over PIO!\n"); // Print a message to the console

    ui_init(); // Initialize the UI created with Eez Studio

    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    float temperature;
    float humidity;
    float dew;

    lv_arc_set_range(objects.temp_arc, -50, 100);
    lv_arc_set_range(objects.humd_arc, 0, 100);
    
    lv_chart_set_type(objects.chart, LV_CHART_TYPE_LINE);
    lv_chart_series_t * ser1 = lv_chart_add_series(objects.chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(objects.chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_Y);
    lv_chart_set_point_count(objects.chart, CHART_POINT_COUNT); // Set the number of points in the chart
    lv_chart_set_range(objects.chart, LV_CHART_AXIS_PRIMARY_Y, -50, 100); // Set range for temperature
    lv_chart_set_range(objects.chart, LV_CHART_AXIS_SECONDARY_Y, 0, 100); // Set range for humidity

    // Initialize chart with default values
    int i;
    for (i = 0; i < CHART_POINT_COUNT; i++) {
        lv_chart_set_next_value(objects.chart, ser1, 25); // Initialize temperature series with 25C
        lv_chart_set_next_value(objects.chart, ser2, 50); // Initialize humidity series with 50%
    }
    // Circular buffers for temperature and humidity
    static float temp_buffer[CHART_POINT_COUNT]; 
    static float humd_buffer[CHART_POINT_COUNT];
    // Initialize with a default value
    for (i=0; i<CHART_POINT_COUNT; i++) {
        temp_buffer[i] = 25.0f;
        humd_buffer[i] = 50.0f;
    }

    float temp_max, temp_min, humd_max, humd_min, temp_avg, humd_avg;
    // Calculate max, min, and average values
    temp_max = array_max(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
    temp_min = array_min(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
    temp_avg = array_avg(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
    humd_max = array_max(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));
    humd_min = array_min(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));
    humd_avg = array_avg(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));
    // Update chart ranges based on min and max values
    lv_chart_set_range(objects.chart, LV_CHART_AXIS_PRIMARY_Y, temp_min-1, temp_max+1); // Set range for temperature
    lv_chart_set_range(objects.chart, LV_CHART_AXIS_SECONDARY_Y, humd_min-1, humd_max+1); // Set range for humidity
    // Update labels with min, max, and average values
    char temp_str[10], humd_str[10],dew_str[10];
    sprintf(temp_str, "%0.0fC", temp_min-1);
    lv_label_set_text(objects.min_temp_chart, temp_str);
    sprintf(temp_str, "%0.0fC", temp_max+1);
    lv_label_set_text(objects.max_temp_chart, temp_str);
    sprintf(temp_str, "%0.0fC", temp_avg);
    lv_label_set_text(objects.mid_temp_chart, temp_str);
    sprintf(humd_str, "%0.0f%%", humd_min-1);
    lv_label_set_text(objects.min_hum_chart, humd_str);
    sprintf(humd_str, "%0.0f%%", humd_max+1);
    lv_label_set_text(objects.max_hum_chart, humd_str);
    sprintf(humd_str, "%0.0f%%", humd_avg);
    lv_label_set_text(objects.mid_hum_chart, humd_str);

    while(1) {
        temperature = GetTemperature();
        humidity= GetHumidity();
        dew = GetDewPoint();

        // Handle button presses and update UI
        bool btna, current_state_a = gpio_get(BUTTON_A_PIN); // Button A state
        bool btnb, current_state_b = gpio_get(BUTTON_B_PIN); // Button B state
        

        sprintf(temp_str, "%0.1fC", temperature);
        sprintf(humd_str, "%0.1f%%", humidity);
        sprintf(dew_str, "%0.1fC", dew);
        lv_label_set_text(objects.temp_val, temp_str);
        lv_label_set_text(objects.humd_val, humd_str);
        lv_label_set_text(objects.dew_val, dew_str);
        
        lv_arc_set_value(objects.temp_arc, temperature);
        lv_arc_set_value(objects.humd_arc, humidity);

        // define absolute time variable and last button pressed time variable
        absolute_time_t now = get_absolute_time(), last_btn_pressed_time;
        
        // Handle Button A
        if (current_state_a != last_state_a) // Check if Button A state has changed
        {
            if (absolute_time_diff_us(last_debounce_time_a, now) > DEBOUNCE_DELAY_MS * 1000) // Check if debounce time has passed (us)
            {
                last_debounce_time_a = now; // Update last debounce time for Button A
                last_state_a = current_state_a; // Update last state for Button A

                if (!current_state_a) // Button A pressed (LOW)
                {
                    btna=true; // Set btna to true if Button A is pressed
                } 
                else 
                {
                    btna=false; // Set btna to false if Button A is released
                }
            }
        }

        // Handle Button B
        if (current_state_b != last_state_b) 
        {
            if (absolute_time_diff_us(last_debounce_time_b, now) > DEBOUNCE_DELAY_MS * 1000) 
            {
                last_debounce_time_b = now;
                last_state_b = current_state_b;

                if (!current_state_b) 
                {
                    btnb=true;
                } 
                else 
                {
                    btnb=false;
                }
            }
        }

        if (temperature < NORMAL_TEMP_LOW_THRESHOLD) {
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0x104050), LV_PART_MAIN ); // Set blue color for low temperature
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0x2196f3), LV_PART_INDICATOR ); // Set blue color for low temperature
            lv_obj_set_style_bg_color(objects.temp_arc, lv_color_hex(0x2196f3), LV_PART_KNOB); // Set blue color for low temperature
        } else if (temperature > NORMAL_TEMP_HIGH_THRESHOLD) {
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0x500000), LV_PART_MAIN); // Set red color for high temperature
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0xFF0000), LV_PART_INDICATOR); // Set red color for high temperature
            lv_obj_set_style_bg_color(objects.temp_arc, lv_color_hex(0xFF0000), LV_PART_KNOB); // Set red color for high temperature
        } else {
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0x005000), LV_PART_MAIN); // Set green color for normal temperature
            lv_obj_set_style_arc_color(objects.temp_arc, lv_color_hex(0x00FF00), LV_PART_INDICATOR); // Set green color for normal temperature
            lv_obj_set_style_bg_color(objects.temp_arc, lv_color_hex(0x00FF00), LV_PART_KNOB); // Set green color for normal temperature
        }

        if (humidity < NORMAL_HUMIDITY_LOW_THRESHOLD) {
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0x104050), LV_PART_MAIN); // Set blue color for low humidity
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0x2196f3), LV_PART_INDICATOR); // Set blue color for low humidity
            lv_obj_set_style_bg_color(objects.humd_arc, lv_color_hex(0x2196f3), LV_PART_KNOB); // Set blue color for low humidity

        } else if (humidity > NORMAL_HUMIDITY_HIGH_THRESHOLD) {
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0x500000), LV_PART_MAIN); // Set red color for high humidity
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0xFF0000), LV_PART_INDICATOR); // Set red color for high humidity
            lv_obj_set_style_bg_color(objects.humd_arc, lv_color_hex(0xFF0000), LV_PART_KNOB); // Set red color for high humidity
        } else {
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0x005000), LV_PART_MAIN); // Set green color for normal humidity
            lv_obj_set_style_arc_color(objects.humd_arc, lv_color_hex(0x00FF00), LV_PART_INDICATOR); // Set green color for normal humidity
            lv_obj_set_style_bg_color(objects.humd_arc, lv_color_hex(0x00FF00), LV_PART_KNOB); // Set green color for normal humidity
        }

        // Update the UI screens based on button presses
        if (btna == true || btnb == true) 
        {
            enum ScreensEnum current_screen = get_current_screen_id(); // Get the active screen from the LVGL display

            if (btna == true && btnb == false && (current_screen == SCREEN_ID_GRAPH)) // If only Button A is pressed @ SCREEN_ID_GRAPH
            {
                loadScreen(SCREEN_ID_MAIN); // Load the main screen
            }
            if (btna == false && btnb == true && (current_screen == SCREEN_ID_GRAPH)) // If only Button B is pressed @ SCREEN_ID_GRAPH
            {
                // change the scale of the graph
            }
            if(btna==false && btnb == true && (current_screen == SCREEN_ID_MAIN)) // If only Button B is pressed @ SCREEN_ID_MAIN
            {
                loadScreen(SCREEN_ID_GRAPH); // Load the graph screen
            }
            if (now - last_btn_pressed_time > 100000) // If more than 100 ms has passed since the last button press 100,000 us = 100ms
            { 
                last_btn_pressed_time = now; // Update the last button pressed time
            }
        }

        enum scaleGraph scale_graph = SCALE_MINUTE; // Scale graph to fit temperature and humidity values 0=minute, 1=hour, 2=day
        static int buffer_index = 0, percentage_to_30min=0;
        static absolute_time_t last_sample_time = 0, last_sample_time2 = 0;
        
        #ifdef DEMO_MODE
            static absolute_time_t last_sample_time3 = 0;
        #endif

        // Sample every 30 minutes (30min * 60sec * 1,000,000us = 1,800,000,000us)
        if (absolute_time_diff_us(last_sample_time, now) >= (30*60*1000000)) { // 30min * 60sec * 1sec = 1,000,000 us
            temp_buffer[buffer_index] = temperature;
            humd_buffer[buffer_index] = humidity;
            buffer_index = (buffer_index + 1) % CHART_POINT_COUNT;
            last_sample_time = now;

            // Calculate max, min, and average values
            temp_max = array_max(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
            temp_min = array_min(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
            temp_avg = array_avg(temp_buffer, sizeof(temp_buffer)/sizeof(temp_buffer[0]));
            humd_max = array_max(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));
            humd_min = array_min(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));
            humd_avg = array_avg(humd_buffer, sizeof(humd_buffer)/sizeof(humd_buffer[0]));

            // Update chart ranges based on min and max values
            lv_chart_set_range(objects.chart, LV_CHART_AXIS_PRIMARY_Y, temp_min-1, temp_max+1); // Set range for temperature
            lv_chart_set_range(objects.chart, LV_CHART_AXIS_SECONDARY_Y, humd_min-1, humd_max+1); // Set range for humidity
            
            // Update labels with min, max, and average values
            sprintf(temp_str, "%0.0fC", temp_min-1);
            lv_label_set_text(objects.min_temp_chart, temp_str);
            sprintf(temp_str, "%0.0fC", temp_max+1);
            lv_label_set_text(objects.max_temp_chart, temp_str);
            sprintf(temp_str, "%0.0fC", temp_avg);
            lv_label_set_text(objects.mid_temp_chart, temp_str);
            sprintf(humd_str, "%0.0f%%", humd_min-1);
            lv_label_set_text(objects.min_hum_chart, humd_str);
            sprintf(humd_str, "%0.0f%%", humd_max+1);
            lv_label_set_text(objects.max_hum_chart, humd_str);
            sprintf(humd_str, "%0.0f%%", humd_avg);
            lv_label_set_text(objects.mid_hum_chart, humd_str);

            lv_label_set_text(objects.scale_change, "24h@1p/30m");

            // Update the chart with new values
            lv_chart_set_next_value(objects.chart, ser1, temperature);
            lv_chart_set_next_value(objects.chart, ser2, humidity);
            percentage_to_30min=0;
        }
        else if(absolute_time_diff_us(last_sample_time2, now) >= (30*60*1000000/100)) // Update every 18 seconds for smoother chart update
        {
            percentage_to_30min++;
            sprintf(humd_str, "%d%%", percentage_to_30min);
            lv_label_set_text(objects.percentage_to_30min_label, humd_str);
            last_sample_time2 = now;
        }

        #ifdef DEMO_MODE
            // In demo mode, switch screens every 7 seconds
            enum ScreensEnum current_screen_demo = get_current_screen_id(); // Get the active screen from the LVGL display
            if (absolute_time_diff_us(last_sample_time3, now) >= 7000000) { // 7 seconds
                if(current_screen_demo == SCREEN_ID_GRAPH)
                {
                    loadScreen(SCREEN_ID_MAIN); // Load the main screen
                }
                else
                {
                    loadScreen(SCREEN_ID_GRAPH); // Load the graph screen
                }
                last_sample_time3 = now;
            }   
        #endif
        lv_task_handler(); // Handle LVGL tasks
        sleep_ms(10); // Sleep to allow other tasks to run
    }

    // Cleanup
    lv_display_delete(lcd_disp);
    lv_free(buf1);
    lv_free(buf2);
    cyw43_arch_deinit();
    
    return 0;
}