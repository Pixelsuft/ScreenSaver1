#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#ifdef IS_WINDOWS
#include <Windows.h>
#endif
#ifdef SDL_main
#undef main
#else
#define SDL_main main
#endif
#ifndef DISPLAY
#define DISPLAY   0
#endif
#ifndef VSYNC
#define VSYNC SDL_TRUE
#endif
#ifndef RENDERER
#define RENDERER  -1
#endif
#ifndef MIN_SPEED
#define MIN_SPEED 0.5f
#endif
#ifndef MAX_SPEED
#define MAX_SPEED 5.0f
#endif


void fatal(const char* error_name) {
  printf("%s: %s\n", error_name, SDL_GetError());
  SDL_Quit();
  exit(1);
}

float random_float(float a, float b) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = b - a;
  float r = random * diff;
  return a + r;
}

int random_int(int a, int b) {
  float random = ((float)rand()) / (float)RAND_MAX;
  float diff = (float)b - (float)a;
  float r = random * diff;
  return a + (int)r;
}

void calc_speed(int* a, int* b, float* c, float d) {
  for (int i = 0; i < 3; i++) {
    c[i] = ((float)b[i] - (float)a[i]) / d;
  }
}

int SDL_main(int c, char* argv[]) {
  if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) != 0) {
    fatal("Failed to init");
  }

  SDL_DisplayMode dm;
  if (SDL_GetDesktopDisplayMode(DISPLAY, &dm) != 0) {
    fatal("Failed to get screen size");
  }
  SDL_Window* window = SDL_CreateWindow(
    "ScreenSaver",
    0,
    0,
    dm.w,
    dm.h,
#ifdef IS_WINDOWS
    SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_GRABBED
#else
    SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_FULLSCREEN
#endif
  );
  if (window == NULL) {
    fatal("Failed to create window");
  }
  SDL_ShowCursor(SDL_FALSE);
#ifdef IS_WINDOWS
  SDL_SysWMinfo wm_info;
  SDL_VERSION(&wm_info.version);
  SDL_GetWindowWMInfo(window, &wm_info);
  HWND hwnd = wm_info.info.win.window;
  if (hwnd == NULL) {
    SetForegroundWindow(hwnd);
  }
#endif
  SDL_Renderer* renderer = SDL_CreateRenderer(
    window,
    RENDERER,
#if VSYNC == SDL_TRUE
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
#else
    SDL_RENDERER_ACCELERATED
#endif
  );
  if (renderer == NULL) {
    fatal("Failed to create renderer");
  }
  srand(SDL_GetTicks());

  int from_color[3] = { 0, 0, 0 };
  int to_color[3] = { random_int(0, 255), random_int(0, 255), random_int(0, 255) };
  float current_color[3] = { 0.0f, 0.0f, 0.0f };
  float speed = random_float(MIN_SPEED, MAX_SPEED);
  float color_speed[3] = { 0.0f, 0.0f, 0.0f };
  float current_timer = 0.0f;
  calc_speed(from_color, to_color, color_speed, speed);
  SDL_bool running = SDL_TRUE;
  Uint64 last_tick = SDL_GetTicks64();

  while (running == SDL_TRUE) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (
        event.type == SDL_QUIT ||
        event.type == SDL_MOUSEBUTTONDOWN ||
        event.type == SDL_KEYUP
      ) {
        running = SDL_FALSE;
      }
    }
    Uint64 now = SDL_GetTicks64();
    float delta = ((float)(now - last_tick)) / 1000.0f;
    last_tick = now;

    current_timer += delta;
    current_color[0] += delta * color_speed[0];
    current_color[1] += delta * color_speed[1];
    current_color[2] += delta * color_speed[2];
    if (current_timer >= speed) {
      current_timer = 0.0f;
      for (int i = 0; i < 3; i++) {
        current_color[i] = (float)to_color[i];
        from_color[i] = to_color[i];
        to_color[i] = random_int(0, 255);
      }
      speed = random_float(MIN_SPEED, MAX_SPEED);
      calc_speed(from_color, to_color, color_speed, speed);
    }

    SDL_SetRenderDrawColor(
      renderer,
      (Uint8)current_color[0],
      (Uint8)current_color[1],
      (Uint8)current_color[2],
      255
    );
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
