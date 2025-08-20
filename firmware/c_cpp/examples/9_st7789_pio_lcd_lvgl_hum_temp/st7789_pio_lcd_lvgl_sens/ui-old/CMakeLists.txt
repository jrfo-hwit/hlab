SET(SOURCES images.c
    screens.c
    styles.c
    ui.c
    ui_image_humidity.c
    ui_image_temperature.c
    ui_image_hwit.c
    )
    
add_library(ui ${SOURCES})
