#pragma comment(lib, "winmm")

#include "stdafx.h"
#include "svga/svga.h"
#undef SV_BM_ANY
#define IS_PRESSED(x) (GetAsyncKeyState(x)/*&0x8000*/)

//If this variable sets to true - game will quit
static bool game_quited = false;

//sv_width and sv_height variables are width and height of screen
extern unsigned int sv_width, sv_height;

//This is default fullscreen shadow buffer. You can use it if you want to.
static unsigned* shadow_buf = NULL;

// these variables provide access to joystick and joystick buttons
// In this version joystick is simulated on Arrows and Z X buttons
// [0]-X axis (-501 - left; 501 - right)
// [1]-Y axis (-501 - left; 501 - right)
int gAxis[2];
//0 - not pressed; 1 - pressed
int gButtons[6];

//This function update full screen from scrptr. The array should be at least sv_height*scrpitch bytes size;
void w32_update_screen(void* scrptr, unsigned scrpitch) {
    if (!SV_update_rect_before(0, 0, sv_width, sv_height)) return;
    //if(!SV_update_rect_before(0,0,0,0))return;
    if (!SV_lock(0, 0, sv_width, sv_height)) return;
    //assert(scrpitch<=sv_pitch);
    for (int h = 0; h < sv_height; ++h) memcpy((char*)locked_scr + h * sv_pitch, (char*)scrptr + h * scrpitch, sv_width * 4);
    //win32_ConvertBitmap(scrptr,scrpitch,locked_scr,sv_pitch);
    SV_unlock();

}void win32_UpdateJoystick() {
    if (IS_PRESSED(VK_UP))
        gAxis[1] = -501;
    else if (IS_PRESSED(VK_DOWN))
        gAxis[1] = 501;
    else gAxis[1] = 0;

    if (IS_PRESSED(VK_LEFT))
        gAxis[0] = -501;
    else if (IS_PRESSED(VK_RIGHT))
        gAxis[0] = 501;
    else gAxis[0] = 0;
    gButtons[0] = ((IS_PRESSED(0x5A)) ? 1 : 0);
    gButtons[1] = ((IS_PRESSED(0x58)) ? 1 : 0);
    gButtons[2] = ((IS_PRESSED(0x43)) ? 1 : 0);
    gButtons[3] = ((IS_PRESSED(VK_SPACE)) ? 1 : 0);
    gButtons[4] = ((IS_PRESSED(0x51)) ? 1 : 0);

}//These functions called from another thread, when a button is pressed or released
void win32_key_down(unsigned k) { if (k == VK_F1) game_quited = true; }
void win32_key_up(unsigned) {}

void init_game() { shadow_buf = new unsigned[sv_width * sv_height]; }

void close_game() {
    if (shadow_buf) delete shadow_buf;
    shadow_buf = NULL;

}//draw the game to screen
void draw_game() {
    if (!shadow_buf) return;
    memset(shadow_buf, 0, sv_width * sv_height * 4);

    //here you should draw anything you want in to shadow buffer. (0 0) is left top corner
    w32_update_screen(shadow_buf, sv_width * 4);

}//act the game. dt - is time passed from previous act
void act_game(float dt) {}

int get_current_time() { return timeGetTime(); }

void game_thread(void* data) {
    //TODO: game init code
    init_game();
    int ticks1 = get_current_time();
    while (!game_quited) {
        //TODO: game infinite loop code
        draw_game();

        win32_UpdateJoystick();

        int ticks2 = get_current_time();

        act_game((ticks2 - ticks1) / 1000.0f);
        ticks1 = ticks2;
        //TODO: game main code
    };//TODO: game quit code
    close_game();
    exit(0);

}//Game thread
void main_Start() { _beginthread(game_thread, 0, NULL); }
void main_End() { SV_done(); }

void win32_init_window(int w, int h, HWND hw) {}
void win32_close_window() {}

void win32InitWindow(HWND hw) {
    sv_back_memory = SV_BM_SYSTEM;
    if (!SV_findmode(640, 480, 32, hw, 1)) SV_done();
}