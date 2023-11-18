#pragma once

// Originaly GLFW key codes

/* Printable keys */
#define BV_KEY_SPACE              32
#define BV_KEY_APOSTROPHE         39  /* ' */
#define BV_KEY_COMMA              44  /* , */
#define BV_KEY_MINUS              45  /* - */
#define BV_KEY_PERIOD             46  /* . */
#define BV_KEY_SLASH              47  /* / */
#define BV_KEY_0                  48
#define BV_KEY_1                  49
#define BV_KEY_2                  50
#define BV_KEY_3                  51
#define BV_KEY_4                  52
#define BV_KEY_5                  53
#define BV_KEY_6                  54
#define BV_KEY_7                  55
#define BV_KEY_8                  56
#define BV_KEY_9                  57
#define BV_KEY_SEMICOLON          59  /* ; */
#define BV_KEY_EQUAL              61  /* = */
#define BV_KEY_A                  65
#define BV_KEY_B                  66
#define BV_KEY_C                  67
#define BV_KEY_D                  68
#define BV_KEY_E                  69
#define BV_KEY_F                  70
#define BV_KEY_G                  71
#define BV_KEY_H                  72
#define BV_KEY_I                  73
#define BV_KEY_J                  74
#define BV_KEY_K                  75
#define BV_KEY_L                  76
#define BV_KEY_M                  77
#define BV_KEY_N                  78
#define BV_KEY_O                  79
#define BV_KEY_P                  80
#define BV_KEY_Q                  81
#define BV_KEY_R                  82
#define BV_KEY_S                  83
#define BV_KEY_T                  84
#define BV_KEY_U                  85
#define BV_KEY_V                  86
#define BV_KEY_W                  87
#define BV_KEY_X                  88
#define BV_KEY_Y                  89
#define BV_KEY_Z                  90
#define BV_KEY_LEFT_BRACKET       91  /* [ */
#define BV_KEY_BACKSLASH          92  /* \ */
#define BV_KEY_RIGHT_BRACKET      93  /* ] */
#define BV_KEY_GRAVE_ACCENT       96  /* ` */
#define BV_KEY_WORLD_1            161 /* non-US #1 */
#define BV_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define BV_KEY_ESCAPE             256
#define BV_KEY_ENTER              257
#define BV_KEY_TAB                258
#define BV_KEY_BACKSPACE          259
#define BV_KEY_INSERT             260
#define BV_KEY_DELETE             261
#define BV_KEY_RIGHT              262
#define BV_KEY_LEFT               263
#define BV_KEY_DOWN               264
#define BV_KEY_UP                 265
#define BV_KEY_PAGE_UP            266
#define BV_KEY_PAGE_DOWN          267
#define BV_KEY_HOME               268
#define BV_KEY_END                269
#define BV_KEY_CAPS_LOCK          280
#define BV_KEY_SCROLL_LOCK        281
#define BV_KEY_NUM_LOCK           282
#define BV_KEY_PRINT_SCREEN       283
#define BV_KEY_PAUSE              284
#define BV_KEY_F1                 290
#define BV_KEY_F2                 291
#define BV_KEY_F3                 292
#define BV_KEY_F4                 293
#define BV_KEY_F5                 294
#define BV_KEY_F6                 295
#define BV_KEY_F7                 296
#define BV_KEY_F8                 297
#define BV_KEY_F9                 298
#define BV_KEY_F10                299
#define BV_KEY_F11                300
#define BV_KEY_F12                301
#define BV_KEY_F13                302
#define BV_KEY_F14                303
#define BV_KEY_F15                304
#define BV_KEY_F16                305
#define BV_KEY_F17                306
#define BV_KEY_F18                307
#define BV_KEY_F19                308
#define BV_KEY_F20                309
#define BV_KEY_F21                310
#define BV_KEY_F22                311
#define BV_KEY_F23                312
#define BV_KEY_F24                313
#define BV_KEY_F25                314
#define BV_KEY_KP_0               320
#define BV_KEY_KP_1               321
#define BV_KEY_KP_2               322
#define BV_KEY_KP_3               323
#define BV_KEY_KP_4               324
#define BV_KEY_KP_5               325
#define BV_KEY_KP_6               326
#define BV_KEY_KP_7               327
#define BV_KEY_KP_8               328
#define BV_KEY_KP_9               329
#define BV_KEY_KP_DECIMAL         330
#define BV_KEY_KP_DIVIDE          331
#define BV_KEY_KP_MULTIPLY        332
#define BV_KEY_KP_SUBTRACT        333
#define BV_KEY_KP_ADD             334
#define BV_KEY_KP_ENTER           335
#define BV_KEY_KP_EQUAL           336
#define BV_KEY_LEFT_SHIFT         340
#define BV_KEY_LEFT_CONTROL       341
#define BV_KEY_LEFT_ALT           342
#define BV_KEY_LEFT_SUPER         343
#define BV_KEY_RIGHT_SHIFT        344
#define BV_KEY_RIGHT_CONTROL      345
#define BV_KEY_RIGHT_ALT          346
#define BV_KEY_RIGHT_SUPER        347
#define BV_KEY_MENU               348

#define BV_KEY_LAST               BV_KEY_MENU

// Mouse buttons
#define BV_MOUSE_BUTTON_1         0
#define BV_MOUSE_BUTTON_2         1
#define BV_MOUSE_BUTTON_3         2
#define BV_MOUSE_BUTTON_4         3
#define BV_MOUSE_BUTTON_5         4
#define BV_MOUSE_BUTTON_6         5
#define BV_MOUSE_BUTTON_7         6
#define BV_MOUSE_BUTTON_8         7
#define BV_MOUSE_BUTTON_LAST      BV_MOUSE_BUTTON_8
#define BV_MOUSE_BUTTON_LEFT      BV_MOUSE_BUTTON_1
#define BV_MOUSE_BUTTON_RIGHT     BV_MOUSE_BUTTON_2
#define BV_MOUSE_BUTTON_MIDDLE    BV_MOUSE_BUTTON_3