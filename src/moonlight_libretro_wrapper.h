#include <stdio.h>

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

EXTERN void moonlight_libretro_wrapper_init(int width, int height);
EXTERN void moonlight_libretro_wrapper_set_working_dir(char* dir);
EXTERN void moonlight_libretro_wrapper_handle_mouse_move(double x, double y);
EXTERN void moonlight_libretro_wrapper_handle_mouse_button(int button, int action, int modifiers);
EXTERN void moonlight_libretro_wrapper_draw();