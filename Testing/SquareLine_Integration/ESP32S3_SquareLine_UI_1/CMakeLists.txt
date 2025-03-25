SET(SOURCES ui_Screen1.c
    ui_Screen2.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_kclogored_png.c
    ui_img_button_up_orange_in_logo_colour_png.c
    ui_img_button_up_action_orange_in_logo_colour_png.c
    ui_img_button_down_orange_in_logo_colour_png.c
    ui_img_button_down_action_orange_in_logo_colour_png.c
    ui_img_scale2_png.c
    ui_img_pointer2_png.c)

add_library(ui ${SOURCES})
