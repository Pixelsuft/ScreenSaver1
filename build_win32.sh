cc main.c -o screensaver.exe $(sdl2-config --cflags --libs) -DSDL_MAIN_HANDLED -DIS_WINDOWS -DWIN_32_LEAN_AND_MEAN -mwindows
