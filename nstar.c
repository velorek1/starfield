/* 
*************************************
- SDL StarField animation -
· Coded by <v3l0r3k> based on ideas from 
  http://web.archive.org/web/20160114180422/http://freespace.virgin.net/hugo.elias/graphics/x_stars.htm
· Date: August 2023
*************************************
*/

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>

/*
#define res		2			//0=160x120 1=360x240 2=640x480
#define pixelScale 	4/res
#define SW		160*res			//screen width
#define SH		120*res			//screen height
*/
//#define pixelScale    4
#define SW		640	//half of screen width
#define SH		480	//half of screen height
#define SW2		(SW/2)	//half of screen width
#define SH2		(SH/2)	//half of screen height
#define TRUE		1
#define FALSE		0
#define MAX_KEY 	1000
//#define SDLW          (SW*pixelScale)         //SDL window width
//#define SDLH          (SH*pixelScale)         //SDl window height
#define MAX_STARS	63
//--------------------------------------------------------------------------------
typedef short BOOL;

typedef struct {
	int w, s, a, d;		//move up, down, left, right
	int sl, sr;		//strafe left, right 
	int m;			//move up, down, look up, down
} keys;
keys K;

typedef struct {
	float cos[360];		//Save sin cos in values 0-360 degrees
	float sin[360];
} math;
math M;

float sx[MAX_STARS], sy[MAX_STARS], sz[MAX_STARS];
float szv[MAX_STARS];
SDL_Event ev;
SDL_Renderer *ren1;
SDL_Window *win1;
long KeyState[MAX_KEY];
BOOL Running = TRUE;
BOOL keypressed = FALSE;
BOOL streak = FALSE;
float SPEED = 1.0;
time_t t;
int x = 0, y = 0;
int px[MAX_STARS], py[MAX_STARS];
int px_old[MAX_STARS], py_old[MAX_STARS];
//Function prototypes
float rand_float(float min, float max);
int rand_num(int min, int max);
//SDL
BOOL InitVideo();
void CleanMemory();
BOOL Key(long K);
void HandleKey(long Sym, BOOL Down);
void HandleEvents();
void KeysUp();
void KeysDown();
//GAME
void newGame();
void draw3D();
void pixel(int x, int y, int c, int alpha);
void clearBackground();
void Main_Loop();
void UpdateGame();
void message();

//-> Entry Point

int randm(unsigned x)
{
	return rand() % x;
}

int rand_num(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

int main()
{
	time_t t;
	srand((unsigned) time(&t));
	InitVideo();
	message();
	newGame();
	Main_Loop();
	CleanMemory();
	return 0;
}

BOOL InitVideo()
{
	SDL_Init(SDL_INIT_VIDEO);
	win1 =
	    SDL_CreateWindow("STARS Z-Demo v0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SW, SH,
			     SDL_WINDOW_SHOWN);
	ren1 = SDL_CreateRenderer(win1, -1, 0);
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	//SDL_RenderSetLogicalSize(ren1, 2048, 1024); 
	//SDL_RenderSetScale(ren1,pixelScale-2,pixelScale-2);
	return (ren1 != NULL) && (win1 != NULL);
}

void ToggleFullscreen(SDL_Window * Window)
{
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN;
	BOOL IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
	SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag);
	SDL_ShowCursor(IsFullscreen);
}

void CleanMemory()
{
	SDL_DestroyRenderer(ren1);
	SDL_DestroyWindow(win1);
	SDL_Quit();
}

BOOL Key(long K)
{
	if ((K >= 0) && (K <= MAX_KEY))
		return KeyState[K];
	else
		return FALSE;
}

void KeysUp()
{
	if (Key(SDLK_w)) {
		K.w = 1;
	}
	if (Key(SDLK_s)) {
		K.s = 1;
	}
	if (Key(SDLK_a)) {
		K.a = 1;
	}
	if (Key(SDLK_d)) {
		K.d = 1;
	}
	if (Key(SDLK_m)) {
		K.m = 1;
	}
	if (Key(SDLK_COMMA)) {
		K.sr = 1;
	}
	if (Key(SDLK_PERIOD)) {
		K.sl = 1;
	}
}

void KeysDown()
{
	if (Key(SDLK_w) == FALSE) {
		K.w = 0;
	}
	if (Key(SDLK_s) == FALSE) {
		K.s = 0;
	}
	if (Key(SDLK_a) == FALSE) {
		K.a = 0;
	}
	if (Key(SDLK_d) == FALSE) {
		K.d = 0;
	}
	if (Key(SDLK_m) == FALSE) {
		K.m = 0;
	}
	
	if (Key(SDLK_COMMA) == FALSE) {
		K.sr = 0;
	}
	if (Key(SDLK_PERIOD) == FALSE) {
		K.sl = 0;
	}
	
}

void HandleKey(long Sym, BOOL Down)
{
	//if (Sym == SDLK_UP) Sym = SDL_SCANCODE_UP;
	//if (Sym == SDLK_DOWN) Sym = SDL_SCANCODE_DOWN;
	// if (Sym == SDLK_LEFT) Sym = SDL_SCANCODE_LEFT;
	// if (Sym == SDLK_RIGHT) Sym = SDL_SCANCODE_RIGHT;
	// if (Sym == SDLK_SPACE) Sym = SDL_SCANCODE_SPACE;
	if ((Sym >= 0) && (Sym <= MAX_KEY)) {
		KeyState[Sym] = Down;
		if (Sym == SDLK_ESCAPE)
			Running = FALSE;
	}
}

void HandleEvents()
{
	SDL_Event e;
	if (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			Running = FALSE;
		}

		if (e.type == SDL_KEYDOWN) {
			keypressed = TRUE;
			HandleKey(e.key.keysym.sym, TRUE);
			KeysUp();
		}
		if (e.type == SDL_KEYUP) {
			keypressed = FALSE;
			HandleKey(e.key.keysym.sym, FALSE);
			KeysDown();
		}
	}
}

float rand_float(float min, float max)
{
	float scale = rand() / (float) RAND_MAX;	/* [0, 1.0] */
	return min + scale * (max - min);	/* [min, max] */
}

//Drawing
void pixel(int x, int y, int c, int alpha)	//draw a pixel at x/y with rgb
{
	int rgb[3];
	SDL_Point *newpoint;

	if (c == 0) {
		rgb[0] = 0;
		rgb[1] = 0;
		rgb[2] = 0;
	}			//Black     
	if (c == 1) {
		rgb[0] = 255;
		rgb[1] = 255;
		rgb[2] = 255;
	}			//White       
	newpoint = (SDL_Point *) malloc(sizeof (SDL_Point));	//new pointer
	newpoint->x = x;
	newpoint->y = y;	// convertPointY(y);i
	SDL_SetRenderDrawBlendMode(ren1, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(ren1, rgb[0], rgb[1], rgb[2], alpha);
	SDL_RenderDrawPoints(ren1, newpoint, 1);
	free(newpoint);		// free memory
}

void draw_line(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, int r,
	       int g, int b, int a)
{
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void clearBackground()
{
	int x, y;
	for (y = 0; y < SH; y++) {
		for (x = 0; x < SW; x++) {
			pixel(x, y, 0, 255);
		}		//clear background color
	}
	//SDL_RenderPresent(ren1);
}

void initStars()
{

	for (int i = 0; i < MAX_STARS; i++) {
		sx[i] = rand_num(-500, 500);
		sy[i] = rand_num(-500, 500);
		sz[i] = rand_num(100, 1000);
		px[i] = 0;
		py[i] = 0;
		szv[i] = rand_float(0.5, 5);
		//printf("X:%f /t Y:%f /t Z:%f /t px: %d /t py: %d\n", sx[i], sy[i],sz[i],px[i],py[i]);
	}
}

void newGame()
{
	SPEED = 0.1;
	initStars();
}

void message(){
   // Blue message
    
    // Reset color to default
    printf("\033[0m");
    
    // Red message
    printf("\033[1;31m[nstar]\033[0m coded by\033[90m velorek\n");
    printf("\033[1;97mKeys:\n");
    printf("\033[0m");
    printf("\033[1;94mw \033[0m increase speed\n");
    printf("\033[1;94md \033[0m decrease speed\n");
    printf("\033[1;94ma \033[0m toggle streak\n");
    printf("\033[1;94mm \033[0m toggle fullscreen\n");
    printf("\033[1;94mESC \033[0m quit\n");
    
    // Reset color to default
    printf("\033[0m");

}

void UpdateGame()
{
	if (K.w == 1) {
		SPEED++;
		//printf("%f\n", SPEED);
		K.w == 0;
	}
	if (K.s == 1) {
		SPEED--;
		//printf("%f\n", SPEED);
		K.s == 0;
	}
	if (K.m == 1) {
		ToggleFullscreen(win1);
		K.m == 0;
	}
	if (K.a == 1) {
		if (streak)
			streak = FALSE;
		else
			streak = TRUE;
		K.a = 0;
	}

}

void draw3D()
{
	int b;
	int xd = 0, yd = 0;
	float length = 0;
	clearBackground();
	for (int i = 0; i < MAX_STARS; i++) {
		// pixel(SW2,SH2,0,255);
		//if (i>0) pixel(px[i-1]-1,py[i-1]-1,1);
		sz[i] = (sz[i] - szv[i]) - SPEED;
		//if (sz[i]!=0){
		//px[i] = (sx[i]/(sz[i]))*100 + (SW2);
		//py[i] = (sy[i]/(sz[i]))*100 + (SH2);
		x = (sx[i] / (sz[i])) * 100 + (SW2);
		y = (sy[i] / (sz[i])) * 100 + (SH2);
		//b = ( (255/5) * szv[i]) * (1000 / sz[i]);
		b = 255 - (sz[i] * (255. / 1000.));
		//b = 255-(sz[i]);
		pixel(px[i], py[i], 1, b);

		//}
		if ((sz[i] < 1) || (px[i] < 0 && px[i] > SW)
		    && (py[i] < 0 && py[i] > SH)) {
			sx[i] = rand_num(-500, 500);
			sy[i] = rand_num(-500, 500);
			sz[i] = rand_num(100, 1000);
			//printf("%d:%d\n",sran,signus);
			szv[i] = rand_float(0.5, 5);
			//printf("X:%f /t Y:%f /t Z:%f /t px: %d /t py: %d  ; szv:%f\n", sx[i], sy[i],sz[i],px[i],py[i],szv[i]);
		}

		// b = 255-(sz[i]);
		//b = 255-(sz[i]*(255./1000.));
		//calculate the length of the streak
		xd = x - px[i];
		yd = y - py[i];
		length = sqrt(xd * xd + yd * yd);
		if (!(streak)) {
			pixel(px[i], py[i], 1, b);
			pixel(px[i] + 1, py[i], 1, b);
			pixel(px[i] + 1, py[i] + 1, 1, b);
		}
		pixel(px[i], py[i] + 1, 1, b);
		//draw streak
		//draw_line(ren1,x,y,px[i],py[i],255,255,255,255);
		if (sz[i] > 1 && (px[i] > 1 && px[i] < SW)
		    && (py[i] > 1 && py[i] < SH)
		    && (x > 1 && x < SW) && (y > 1 && y < SH) && length < 50
		    && streak) {
			b = (5000 * szv[i]) / sz[i];
			//if (length > 1) b = (b/length);
			draw_line(ren1, x, y, px[i], py[i], 255, 255, 255, b);
			//draw_line(ren1,px[i],py[i],x,y,255,255,255,100);

			//printf("%d:%d:%d:%d:%f:%d\n", x, y, px[i], py[i],
			   //    length, b);
		}
		px_old[i] = px[i];
		py_old[i] = py[i];
		px[i] = x;
		py[i] = y;

	}
	SDL_RenderPresent(ren1);
}

void Main_Loop()
{
/* Update + HandleEvents - Draw */
	unsigned int LastTime, CurrentTime;

	LastTime = SDL_GetTicks();
	while (Running == TRUE) {
		CurrentTime = SDL_GetTicks();
		if (CurrentTime - LastTime > 1000)
			LastTime = CurrentTime - 60;
		while (CurrentTime - LastTime > 1000 / 30) {
			UpdateGame();
			LastTime = LastTime + 30;
		}
		HandleEvents();
		draw3D();
	}
}

/* ---------------------------------------------- */
/* END 						  */
/* ---------------------------------------------- */
