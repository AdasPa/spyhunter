
//WYJEBAĆ 
//WSZYSTKIE 
//IOSTREAMY, 
//RANDOM, 
//FSTREAM


#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <string.h>

//srand(time(NULL));
//rand()%50+250;
extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define CAR_HEIGHT 65
#define CAR_WIDTH 45

#define TEXTBOX_HEIGHT SCREEN_HEIGHT
#define TEXTBOX_WIDTH 128

#define MAX_ROAD_WIDTH 322
#define MAX_POBOCZE_WIDTH 100

#define NUMBER_OF_BULLETS 6
#define BULLET_LENGTH 25
#define BULLET_WIDTH 5

#define START_CAR_NUMBER 5
#define UNLIMITED_CARS_TIME 10 //seconds

#define ROAD_SPEED 200
#define ENEMY_SPEED	50
#define NPC_SPEED 300

//spawn frequency
#define ENEMY_SPAWN 2
#define NPC_SPAWN 3
#define POWERUP_SPAWN 10

#define NEW_CAR_POINTS 100 // in points/50
#define FINISH_SCREEN_TIME 10 //seconds


// narysowanie napisu txt na powierzchni screen, zaczynając od punktu (x, y)
// charset to bitmapa 128x128 zawierająca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface *screen, int x, int y, const char *text,
                SDL_Surface *charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while(*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
		};
	};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt środka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface *screen, SDL_Surface *sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
	};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
	if (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT)
	{
		int bpp = surface->format->BytesPerPixel;
		Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
		*(Uint32*)p = color;
	}
	else printf("Próba narysowanai poza ekranem x: %d y: %d\n", x, y);
};


// rysowanie linii o długości l w pionie (gdy dx = 0, dy = 1) 
// bądź poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface *screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for(int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
		};
	};


// rysowanie prostokąta o długości boków l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface *screen, int x, int y, int l, int k,
                   Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for(i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
	};

void Destroy(SDL_Texture *scrtex, SDL_Window* window, SDL_Renderer* renderer)
{
	SDL_DestroyTexture(scrtex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

struct position
{
	double x = (SCREEN_WIDTH - TEXTBOX_WIDTH) / 2;
	double y = SCREEN_HEIGHT / 2;
};


void set_variables(int* t1, int* frames, double* fpsTimer, double* fps, int* quit, double* worldTime, double* distance, double* car_speed_x, double* car_speed_y, double* points, int* road_width, double* xpoints, double* powerup_exist, double* enemy_exist, double* powerup_time, double* npc_exist, int* bullet1_counter, int* bullet2_counter, int* bullet_powerup_counter, int* bullet1_exist, int* bullet2_exist, int* bullet_powerup_exist, double* nopoints_time, int* car_number, double* previous_points, double* no_cars, double* przesuniecie, double* delta_position, int* expected_road_width)
{
	*t1 = SDL_GetTicks();
	*frames = 0;
	*fpsTimer = 0;
	*fps = 0;
	*quit = 0;
	*worldTime = 0;
	*distance = 0;
	*car_speed_x = 0;
	*car_speed_y = 0;
	*points = 0;
	*road_width = 50;
	*expected_road_width = *road_width;
	*xpoints = 1.0;
	*powerup_exist = 0;
	*enemy_exist = 0;
	*npc_exist = 0;
	*powerup_time = 0;
	*bullet1_counter = 0;
	*bullet2_counter = 0;
	*bullet_powerup_counter = 0;
	*nopoints_time = -0.1;
	*car_number = START_CAR_NUMBER;
	*previous_points = 0;
	*no_cars = 0;
	*przesuniecie = 0;
	*delta_position = 0;


	for (int i = 0; i < NUMBER_OF_BULLETS; i++)
	{
		bullet1_exist[i] = 0;
		bullet2_exist[i] = 0;
		bullet_powerup_exist[i] = 0;
	}
}

void set_position(struct position* position)
{
	position->x = (SCREEN_WIDTH - TEXTBOX_WIDTH) / 2;
	position->y = SCREEN_HEIGHT / 2;
}

//int load_graphics(SDL_Surface* screen, SDL_Surface* charset, SDL_Texture* scrtex, SDL_Window* window, SDL_Renderer* renderer, SDL_Surface* car, SDL_Surface* enemy, SDL_Surface* powerup, SDL_Surface* npc, SDL_Surface* bullet1, SDL_Surface* bullet2)
//{
//	charset = SDL_LoadBMP("./cs8x8.bmp");
//	if (charset == NULL) {
//		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(screen);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//	SDL_SetColorKey(charset, true, 0x000000);
//
//	car = SDL_LoadBMP("./auto1.bmp");
//	if (car == NULL) {
//		printf("SDL_LoadBMP(auto1.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	enemy = SDL_LoadBMP("./auto2.bmp");
//	if (enemy == NULL) {
//		printf("SDL_LoadBMP(auto2.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		SDL_FreeSurface(car);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	powerup = SDL_LoadBMP("./powerup.bmp");
//	if (powerup == NULL) {
//		printf("SDL_LoadBMP(powerup.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		SDL_FreeSurface(car);
//		SDL_FreeSurface(enemy);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	npc = SDL_LoadBMP("./auto3.bmp");
//	if (npc == NULL) {
//		printf("SDL_LoadBMP(auto3.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		SDL_FreeSurface(car);
//		SDL_FreeSurface(enemy);
//		SDL_FreeSurface(powerup);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	bullet1 = SDL_LoadBMP("./bullet1.bmp");
//	if (bullet1 == NULL) {
//		printf("SDL_LoadBMP(bullet1.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		SDL_FreeSurface(car);
//		SDL_FreeSurface(enemy);
//		SDL_FreeSurface(powerup);
//		SDL_FreeSurface(npc);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	bullet2 = SDL_LoadBMP("./bullet2.bmp");
//	if (npc == NULL) {
//		printf("SDL_LoadBMP(bullet2.bmp) error: %s\n", SDL_GetError());
//		SDL_FreeSurface(charset);
//		SDL_FreeSurface(screen);
//		SDL_FreeSurface(car);
//		SDL_FreeSurface(enemy);
//		SDL_FreeSurface(powerup);
//		SDL_FreeSurface(npc);
//		SDL_FreeSurface(bullet1);
//		Destroy(scrtex, window, renderer);
//		return 1;
//	};
//
//	return 0;
//}

void save_result(char* name, int score, double time)
{
	FILE* list = fopen("best_results.txt", "a");

	fprintf(list, "%s %d %.2lf\n", name, score, time);
	fclose(list);
}

void draw_game_controls(SDL_Surface* screen, SDL_Surface* charset, int kolor1, int kolor2)
{
	char text[128];
	DrawRectangle(screen, SCREEN_WIDTH - TEXTBOX_WIDTH, 0, TEXTBOX_WIDTH, TEXTBOX_HEIGHT, kolor1, kolor2);



	sprintf(text, "esc-wyjscie");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 192, text, charset);
	sprintf(text, "strzalki-ster");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 172, text, charset);
	sprintf(text, "n-nowa gra");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 152, text, charset);
	sprintf(text, "s-zapisz");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 132, text, charset);
	sprintf(text, "l-wczytaj");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 112, text, charset);
	sprintf(text, "p-pauza/play");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 92, text, charset);
	sprintf(text, "spacja-strzal");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 72, text, charset);
}

void draw_time_fps(SDL_Surface* screen, SDL_Surface* charset, int kolor1, int kolor2, double worldTime, double fps, double points, double distance, int car_number)
{
	char text[128];

	sprintf(text, "PUNKTY");
	DrawString(screen, SCREEN_WIDTH - 124, 16, text, charset);
	sprintf(text, "%d", (int)points*50);
	DrawString(screen, SCREEN_WIDTH - 124, 31, text, charset);
	sprintf(text, "CZAS TRWANIA");
	DrawString(screen, SCREEN_WIDTH - 124, 56, text, charset);
	sprintf(text, "%.1lf s", worldTime);
	DrawString(screen, SCREEN_WIDTH - 124, 71, text, charset);
	sprintf(text, "FPSy");
	DrawString(screen, SCREEN_WIDTH - 124, 96, text, charset);
	sprintf(text, "%.0lf klatek / s", fps);
	DrawString(screen, SCREEN_WIDTH - 124, 111, text, charset);
	sprintf(text, "DYSTANS");
	DrawString(screen, SCREEN_WIDTH - 124, 136, text, charset);
	sprintf(text, "%.0lf m", distance);
	DrawString(screen, SCREEN_WIDTH - 124, 151, text, charset);
	sprintf(text, "SAMOCHODY");
	DrawString(screen, SCREEN_WIDTH - 124, 176, text, charset);
	sprintf(text, "%d smigaczy", car_number);
	DrawString(screen, SCREEN_WIDTH - 124, 191, text, charset);
}

void new_game(double* worldTime, double* distance, double* carSpeed, double* points, struct position* position, int* road_width)
{
	*worldTime = 0;
	*distance = 0;
	*carSpeed = 10;
	*points = 0;
	*road_width = 50;
	 position->x = (SCREEN_WIDTH - TEXTBOX_WIDTH) / 2;
	 position->y = SCREEN_HEIGHT / 2;
}

void finish_game(SDL_Surface* screen, SDL_Surface* charset, double* world_time, double* distance, int* points, int kolor1, int kolor2)
{
	char text[128];
	DrawRectangle(screen, 100, 100, 500, 200, kolor1, kolor2);
	sprintf(text, "esc-wyjscie");
	DrawString(screen, SCREEN_WIDTH - 124, SCREEN_HEIGHT - 192, text, charset);

}

void road_calculations(int* road_width, int* expected_road_width)
{
	if (*road_width > *expected_road_width)	(*road_width)--;
	else if (*road_width < *expected_road_width)	(*road_width)++;
}

void draw_road(SDL_Surface* screen, int road_width, int kolor1, int kolor2, double* przesuniecie)
{
	//pobocze
	DrawRectangle(screen, 0, 0, road_width, SCREEN_HEIGHT, kolor1, kolor1);
	DrawRectangle(screen, SCREEN_WIDTH - (TEXTBOX_WIDTH + road_width), 0, road_width, SCREEN_HEIGHT, kolor1, kolor1);

	//pasy
	if (*przesuniecie > 60) DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, 0, 6, *przesuniecie - 59, kolor2, kolor2);
	DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, 0 % SCREEN_HEIGHT + *przesuniecie, 6, 60, kolor2, kolor2);
	DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, 120 % SCREEN_HEIGHT + *przesuniecie, 6, 60, kolor2, kolor2);
	DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, 240 % SCREEN_HEIGHT + *przesuniecie, 6, 60, kolor2, kolor2);
	if (*przesuniecie < 60) DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, 360 % SCREEN_HEIGHT + *przesuniecie, 6, 60, kolor2, kolor2);
	if (*przesuniecie > 60) DrawRectangle(screen, ((SCREEN_WIDTH - TEXTBOX_WIDTH) - 3) / 2, SCREEN_HEIGHT - (120 - *przesuniecie), 6, (120 - *przesuniecie), kolor2, kolor2);
}

void time_calculations(double* delta_time, int* t1, int* t2, double* world_time, double* distance, double* move, double* car_speed_x, double* car_speed_y, double* fps_timer, double* fps, int* frames, double* points, double* xpoints, double* nopoints_time, double* przesuniecie, struct position* car_position, double* delta_position)
{
	double change = car_position->y - *delta_position;
	if (change > 1.0) change = 1.0;
	*delta_position = car_position->y;
	*t2 = SDL_GetTicks();
	*delta_time = (*t2 - *t1) * 0.001;
	*t1 = *t2;
	*world_time += *delta_time;
	*distance += ROAD_SPEED* *delta_time *(1-change);

	*fps_timer += *delta_time;
	if (*fps_timer > 0.5) {
		*fps = *frames * 2;
		*frames = 0;
		*fps_timer -= 0.5;
	};

	*nopoints_time -= *delta_time;
	if (*nopoints_time < 0) *points += (*xpoints-change) * *delta_time;
	*xpoints = 1.0;

	*przesuniecie += ROAD_SPEED * *delta_time;
	if (*przesuniecie > 120)
	{
		*przesuniecie -= 120;
	}
	//printf("dt: %.3lf  zmiana: %.10lf\n", *delta_time, change);
}

void movement_calculations(double* world_time, double* delta_time, struct position* powerup_position, struct position* npc_position, struct position* car_position, struct position* enemy_position, double* delta_position, double* car_speed_x, double* car_speed_y, int road_width)
{
	if (car_position->x + (double)ROAD_SPEED * *car_speed_x * *delta_time > (CAR_WIDTH / 2 + road_width) && *car_speed_x < 0) car_position->x += (double)ROAD_SPEED * *car_speed_x * *delta_time;
	else if (car_position->x + (double)ROAD_SPEED * *car_speed_x * *delta_time < (SCREEN_WIDTH - TEXTBOX_WIDTH - CAR_WIDTH / 2 - road_width) && *car_speed_x > 0) car_position->x += (double)ROAD_SPEED * *car_speed_x * *delta_time;


	if (car_position->y + (double)ROAD_SPEED * *car_speed_y * *delta_time > CAR_HEIGHT && *car_speed_y < 0) car_position->y += (double)ROAD_SPEED * *car_speed_y * *delta_time;
	else if (car_position->y + (double)ROAD_SPEED * *car_speed_y * *delta_time < (SCREEN_HEIGHT - CAR_HEIGHT) && *car_speed_y > 0) car_position->y += (double)ROAD_SPEED * *car_speed_y * *delta_time;

	
	powerup_position->y += *delta_time * ROAD_SPEED;

	npc_position->y += *delta_time * NPC_SPEED;
	if(npc_position->x < (MAX_POBOCZE_WIDTH + MAX_ROAD_WIDTH) && npc_position->x > MAX_POBOCZE_WIDTH) npc_position->x += *delta_time * 50 * sin(*world_time);


	if (enemy_position->x > car_position->x)
	{
		enemy_position->x -= *delta_time * ENEMY_SPEED;
	}
	else
	{
		enemy_position->x += *delta_time * ENEMY_SPEED;
	}

	if (enemy_position->y > car_position->y)
	{
		enemy_position->y -= *delta_time * ENEMY_SPEED;
	}
	else
	{
		enemy_position->y += *delta_time * ENEMY_SPEED;
	}

	//printf("dp: %.5lf\n", *delta_position);
	//printf("car_x: % .4lf  car_x : % .4lf\n", car_position->x, car_position->y);
}

int change_road_width()
{
	srand(time(NULL));
	return rand() % 50 + 50;
}

void pause_game(SDL_Event* event, int* quit, int* t1)
{
	int play = 0;

	while (!play && !(*quit))
	{
		while (SDL_PollEvent(event)) 
		{
			switch (event->type) 
			{
			case SDL_KEYDOWN:
				if (event->key.keysym.sym == SDLK_r)
				{
					play = 1;
					break;
				}
			case SDL_QUIT:
				*quit = 1;
				break;
			}
		}
	}
	*t1 = SDL_GetTicks();
}

void create_powerup(double* powerup_exist, struct position* position)
{
	position->x = rand() % 300 + 111; //322 to minimalna szerokosc drogi
	position->y = rand() % (SCREEN_HEIGHT - 100) + 50;
	*powerup_exist = 0.1;
}

void is_powerup_taken(struct position* powerup_position, struct position* car_position, double* powerup_exist, double* powerup_time, double* delta_time)
{
	if (*powerup_exist && powerup_position->y > SCREEN_HEIGHT)
	{
		*powerup_exist = 0;
	}
	else
	{
		if (*powerup_exist && fabs(car_position->x - powerup_position->x) < CAR_WIDTH && fabs(car_position->y - powerup_position->y) < CAR_HEIGHT)
		{
			*powerup_exist = 0;
			*powerup_time = 5;
		}
	}

	*powerup_time -= *delta_time;
}

void create_npc(double* npc_exist, struct position* position, struct position* car_position)
{
	srand(time(NULL));
	position->x = rand() % 300 + 111; //322 to minimalna szerokosc drogi
	position->y = 0;

	while (fabs(position->x - car_position->x) < 50 || fabs(position->y - car_position->y) < 50)
	{
		position->x = rand() % 300 + 111; //322 to minimalna szerokosc drogi
		position->y = 0;
	}
	*npc_exist = 0.1;
}

void create_enemy(double* enemy_exist, struct position* position, struct position* car_position)
{
	srand(time(NULL));
	position->x = rand() % 300 + 111; //322 to minimalna szerokosc drogi
	position->y = rand() % (SCREEN_HEIGHT - 100) + 50;

	while (fabs(position->x - car_position->x) < 100 || fabs(position->y - car_position->y) < 100)
	{
		position->x = rand() % 300 + 111; //322 to minimalna szerokosc drogi, 100 to maksymalna szerokość pobocza
		position->y = rand() % (SCREEN_HEIGHT - 100) + 50;
	}
	*enemy_exist = 0.1;
}

void is_npc_out_of_screen(struct position* npc_position, double* npc_exist)
{
	if (*npc_exist && npc_position->y > SCREEN_HEIGHT)
	{
		*npc_exist = 0;
	}
}

void shoot(SDL_Surface* screen, struct position* car_position, double* powerup_time, SDL_Surface* bullet1, int* bullet1_exist, struct position* bullet1_position, int bullet1_counter, int* bullet_powerup_exist, struct position* bullet_powerup_position, int bullet_powerup_counter)
{
	if (*powerup_time <= 0)
	{
		for (int i = 0; i < NUMBER_OF_BULLETS; i++)
		{
			if (!bullet1_exist[i])
			{
				bullet1_exist[i] = 1;
				bullet1_position[i].x = car_position->x;
				bullet1_position[i].y = car_position->y - BULLET_LENGTH;
				bullet1_counter++;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < NUMBER_OF_BULLETS; i++)
		{
			if (!bullet1_exist[i])
			{
				bullet1_exist[i] = 1;
				bullet1_position[i].x = car_position->x - 15;
				bullet1_position[i].y = car_position->y - BULLET_LENGTH;
				bullet1_counter++;
				bullet_powerup_exist[i] = 1;
				bullet_powerup_position[i].x = car_position->x + 15;
				bullet_powerup_position[i].y = car_position->y - BULLET_LENGTH;
				bullet_powerup_counter++;
				break;
			}
		}
	}
}

void draw_bullets(SDL_Surface* screen, SDL_Surface* bullet, int* bullet_exist, struct position* bullet_position, double delta_time)
{
	for (int i = 0; i < NUMBER_OF_BULLETS; i++)
	{
		if (bullet_exist[i])
		{
			DrawSurface(screen, bullet, bullet_position[i].x, bullet_position[i].y);
			
			bullet_position[i].y -= delta_time * 200;
	

			if (bullet_position[i].y < 0)
			{
				bullet_exist[i] = 0;
			}
		}
	}
}

void is_enemy_shot(struct position* bullet1_position, struct position* bullet_powerup_position, struct position* enemy_position, double* enemy_exist, int* bullet1_exist, int* bullet_powerup_exist, double* points, double* nopoints_time)
{
	for (int i = 0; i < NUMBER_OF_BULLETS; i++)
	{
		if (bullet1_exist[i] && enemy_exist)
		{
			if (fabs(bullet1_position[i].x - enemy_position->x) < CAR_WIDTH/2 && fabs(bullet1_position[i].y - enemy_position->y) < CAR_HEIGHT / 2)
			{
				enemy_position->x = 0;
				enemy_position->y = 0;
				bullet1_exist[i] = 0;
				*enemy_exist = 0;
				if (*nopoints_time < 0) *points += 4;
				break;
			}
			if (fabs(bullet_powerup_position[i].x - enemy_position->x) < CAR_WIDTH / 2 && fabs(bullet_powerup_position[i].y - enemy_position->y) < CAR_HEIGHT / 2)
			{
				enemy_position->x = 0;
				enemy_position->y = 0;
				bullet_powerup_exist[i] = 0;
				*enemy_exist = 0;
				if (*nopoints_time < 0) *points += 4;
				break;
			}
		}
	}
}

void is_npc_shot(struct position* bullet1_position, struct position* bullet_powerup_position, struct position* npc_position, double* npc_exist, int* bullet1_exist, int* bullet_powerup_exist, double* points, double* nopoints_time)
{
	for (int i = 0; i < NUMBER_OF_BULLETS; i++)
	{
		if (bullet1_exist[i] && npc_exist)
		{
			if (fabs(bullet1_position[i].x - npc_position->x) < CAR_WIDTH / 2 && fabs(bullet1_position[i].y - npc_position->y) < CAR_HEIGHT / 2)
			{
				npc_position->x = 0;
				npc_position->y = 0;
				bullet1_exist[i] = 0;
				*npc_exist = 0;
				*nopoints_time = 4.0;
				break;
			}
			if (fabs(bullet_powerup_position[i].x - npc_position->x) < CAR_WIDTH / 2 && fabs(bullet_powerup_position[i].y - npc_position->y) < CAR_HEIGHT / 2)
			{
				npc_position->x = 0;
				npc_position->y = 0;
				bullet_powerup_exist[i] = 0;
				*npc_exist = 0;
				*nopoints_time = 4.0;
				break;
			}
		}
	}
}

void is_crash(struct position* car_position, struct position* npc_position, struct position* enemy_position, double* enemy_exist, double* npc_exist, int* car_number, double* world_time)
{
	if (*npc_exist && *enemy_exist)
	{
		if (fabs(enemy_position->x - npc_position->x) < CAR_WIDTH && fabs(enemy_position->y - npc_position->y) < CAR_HEIGHT)
		{
			*enemy_exist = 0;
			*npc_exist = 0;
			npc_position->x = 0;
			npc_position->y = 0;
			enemy_position->x = 0;
			enemy_position->y = 0;
		}
	}
	if (*enemy_exist)
	{
		if (fabs(enemy_position->x - car_position->x) < CAR_WIDTH && fabs(enemy_position->y - car_position->y) < CAR_HEIGHT)
		{
			*enemy_exist = 0;
			set_position(car_position);
			if (*world_time > UNLIMITED_CARS_TIME) (*car_number)--;
			enemy_position->x = 0;
			enemy_position->y = 0;
		}
	}
	if (*npc_exist)
	{
		if (fabs(npc_position->x - car_position->x) < CAR_WIDTH && fabs(npc_position->y - car_position->y) < CAR_HEIGHT)
		{
			*npc_exist = 0;
			set_position(car_position);
			if (*world_time > UNLIMITED_CARS_TIME) (*car_number)--;
			npc_position->x = 0;
			npc_position->y = 0;
		}
	}
}

void add_car(int* car_number, double* points, double* previous_points)
{
	//printf("points: %.3lf   prev_points: %.3lf\n", *points, *previous_points);

	if ((int)*points % NEW_CAR_POINTS == 0 && (int)*previous_points % NEW_CAR_POINTS != 0) (*car_number)++;
	*previous_points = *points;
}

void draw_no_cars(SDL_Surface* screen, SDL_Surface* charset, SDL_Texture* scrtex, SDL_Renderer* renderer, int kolor1, int kolor2, int czarny, double* no_cars, int* car_number, double* delta_time, double* points, double* world_time, SDL_Event* event, int* quit)
{
	SDL_FillRect(screen, NULL, czarny);
	*no_cars = FINISH_SCREEN_TIME;
	char name[32] = "";
	int size = 0;
	char txt[128];
	
	SDL_StartTextInput();

	while (*no_cars > 0)
	{
		
		DrawRectangle(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4, 16, 3 * (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4, 256, kolor1, kolor2);

		sprintf(txt, "KONIEC GRY. ZABRAKLO SAMOCHODOW");
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 64, txt, charset);
		sprintf(txt, "NOWA GRA ROZPOCZNIE SIE ZA %.1lf s", *no_cars);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 96, txt, charset);
		sprintf(txt, "TWOJ WYNIK: %d ", (int)(*points)*50);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 128, txt, charset);
		sprintf(txt, "TWOJ CZAS: %.2lf ", *world_time);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 160, txt, charset);
		sprintf(txt, "WPISZ SWOJ NICK: %s ", name);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 192, txt, charset);

		while (SDL_PollEvent(event))
		{
			switch (event->type)
			{
				case SDL_TEXTINPUT:
					name[size] = *(event->text.text);
					size++;
			}
		}
		printf("%s\n", name);

		*no_cars -= *delta_time;
		if (*quit == 1) break;

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

	}

	SDL_StopTextInput();

	if (size == 0)
	{
		name[0] = 'a';
		name[1] = 'n';
		name[2] = 'o';
		name[3] = 'n';
		name[4] = 'y';
		name[5] = 'm';
		name[6] = 'u';
		name[7] = 's';

		size = 8;
	}

	save_result(name, 50 * (int)(*points), *world_time);
}

void move_inside_tab(char names[][32], double* time, int* points, char *imie, int wynik, double czas, int index)
{
	for (int i = 4; i > index; i--)
	{
		for (int j = 0; j < 32; j++)
		{
			names[i][j] = names[i - 1][j];
		}

		time[i] = time[i-1];
		points[i] = points[i-1];
	}

	for (int j = 0; j < 32; j++)
	{
		names[index][j] = imie[j];
	}
	
	time[index] = czas;
	points[index] = wynik;

}


void sort_results(char names[][32], double* time, int* points, int type)
{
	FILE* list = fopen("best_results.txt", "r");
	char znak = 'a';
	int licznik = 0;

	char imie[32] = "";
	int wynik = 0;
	double czas = 0;

	while ((znak = fgetc(list)) != EOF)
	{
		if (znak == '\n') licznik++;
	}
	printf("\n%d\n", licznik);

	
	rewind(list);
	for (int i = 0; i < licznik; i++)
	{
		fscanf(list, "%s %d %lf", imie, &wynik, &czas);

		for(int j = 0; j < 5; j++)
		{
			if (type == 0)
			{
				if (wynik > points[j])
				{
					move_inside_tab(names, time, points, imie, wynik, czas, j);
					printf("%s %d %.2lf\n", imie, wynik, czas);
					break;
				}
			}
			else
			{
				if (czas > time[j])
				{
					move_inside_tab(names, time, points, imie, wynik, czas, j);
					printf("essa %s %d %.2lf\n", imie, wynik, czas);
					break;
				}
			}
		}
	}
	licznik = 0;
	printf("jestem");

	fclose(list);
}

void draw_highscore(SDL_Surface* screen, SDL_Surface* charset, SDL_Texture* scrtex, SDL_Renderer* renderer, int kolor1, int kolor2, SDL_Event* event, int* quit, int* t1, int type, int czarny, double delta_time) //type == 0 -> sort by points       type == 1 -> sort by time 
{
	SDL_FillRect(screen, NULL, czarny);
	char txt[128];
	int show = 0;

	char names[5][32];
	double time[5];
	int points[5];
	
	printf("1");
	for (int i = 0; i < 5; i++)
	{
		points[i] = 0;
		time[i] = 0;
		for (int j = 0; j < 32; j++)
		{
			names[i][j] = ' ';
		}
	}

	sort_results(names, time, points, type);	


	while (!show)
	{

		DrawRectangle(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4, 16, 3 * (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4, 256, kolor1, kolor2);

		sprintf(txt, "OTO NAJLEPSZE WYNIKI:");
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 32, txt, charset);
		

		sprintf(txt, "1. %s     %.1lf s     %d punktow", names[0], time[0], points[0]);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 64, txt, charset);
		printf("1. %s     %.1lf s     %d punktow\n", names[0], time[0], points[0]);

		sprintf(txt, "2. %s     %.1lf s     %d punktow", names[1], time[1], points[1]);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 96, txt, charset);
		printf("2. %s     %.1lf s     %d punktow\n", names[1], time[1], points[1]);

		sprintf(txt, "3. %s    %.1lf s     %d punktow", names[2], time[2], points[2]);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 128, txt, charset);
		printf("3. %s     %.1lf s     %d punktow\n", names[2], time[2], points[2]);

		sprintf(txt, "4. %s    %.1lf s     %d punktow", names[3], time[3], points[3]);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 160, txt, charset);
		printf("4. %s     %.1lf s     %d punktow\n", names[3], time[3], points[3]);

		sprintf(txt, "5. %s    %.1lf s     %d punktow", names[4], time[4], points[4]);
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 192, txt, charset);
		printf("5. %s     %.1lf s     %d punktow\n", names[4], time[4], points[4]);

		sprintf(txt, "ABY WZNOWIC GRE WCISNIJ PRZYCISK r");
		DrawString(screen, (SCREEN_WIDTH - TEXTBOX_WIDTH) / 4 + 16, 256, txt, charset);

		while (SDL_PollEvent(event))
		{
			switch (event->type)
			{
			case SDL_QUIT:
				*quit = 1;
				break;
			case SDL_KEYDOWN:
				if (event->key.keysym.sym == SDLK_ESCAPE) { *quit = 1; show = 1;}
				else if (event->key.keysym.sym == SDLK_r) show = 1;
			}
		}

		if (*quit == 1) break;

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		*t1 = SDL_GetTicks();
	}
}





// main
#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char **argv) {
	int t1, t2, quit, frames, rc,  road_width, expected_road_width, bullet1_exist[NUMBER_OF_BULLETS], bullet2_exist[NUMBER_OF_BULLETS], bullet_powerup_exist[NUMBER_OF_BULLETS], bullet1_counter, bullet2_counter, bullet_powerup_counter, car_number;
	double delta_time, world_time, fps_timer, fps, distance, car_speed_x, car_speed_y, move, points, xpoints, powerup_exist, enemy_exist, npc_exist, powerup_time, nopoints_time, previous_points, no_cars, przesuniecie, delta_position;
	SDL_Event event;
	SDL_Surface *screen, *charset;
	SDL_Surface *car, *enemy, *powerup, *npc, *bullet1, *bullet2;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	struct position car_position, powerup_position, enemy_position, npc_position, bullet1_position[NUMBER_OF_BULLETS], bullet2_position[NUMBER_OF_BULLETS], bullet_powerup_position[NUMBER_OF_BULLETS];

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pełnoekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Adam Pacek 193318");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	

	// wczytanie obrazków
	charset = SDL_LoadBMP("./cs8x8.bmp");
	if(charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(screen);
		Destroy(scrtex, window, renderer);
		return 1;
		};
	SDL_SetColorKey(charset, true, 0x000000);

	car = SDL_LoadBMP("./auto1.bmp");
	if(car == NULL) {
		printf("SDL_LoadBMP(auto1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		Destroy(scrtex, window, renderer);
		return 1;
		};

	enemy = SDL_LoadBMP("./auto2.bmp");
	if (enemy == NULL) {
		printf("SDL_LoadBMP(auto2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_FreeSurface(car);
		Destroy(scrtex, window, renderer);
		return 1;
	};

	powerup = SDL_LoadBMP("./powerup.bmp");
	if (powerup == NULL) {
		printf("SDL_LoadBMP(powerup.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_FreeSurface(car);
		SDL_FreeSurface(enemy);
		Destroy(scrtex, window, renderer);
		return 1;
	};

	npc = SDL_LoadBMP("./auto3.bmp");
	if (npc == NULL) {
		printf("SDL_LoadBMP(auto3.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_FreeSurface(car);
		SDL_FreeSurface(enemy);
		SDL_FreeSurface(powerup);
		Destroy(scrtex, window, renderer);
		return 1;
	};

	bullet1 = SDL_LoadBMP("./bullet1.bmp");
	if (bullet1 == NULL) {
		printf("SDL_LoadBMP(bullet1.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_FreeSurface(car);
		SDL_FreeSurface(enemy);
		SDL_FreeSurface(powerup);
		SDL_FreeSurface(npc);
		Destroy(scrtex, window, renderer);
		return 1;
	};

	bullet2 = SDL_LoadBMP("./bullet2.bmp");
	if (bullet2 == NULL) {
		printf("SDL_LoadBMP(bullet2.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(charset);
		SDL_FreeSurface(screen);
		SDL_FreeSurface(car);
		SDL_FreeSurface(enemy);
		SDL_FreeSurface(powerup);
		SDL_FreeSurface(npc);
		SDL_FreeSurface(bullet1);
		Destroy(scrtex, window, renderer);
		return 1;
	};




	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);
	int bialy = SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF);
	int counter = 0;

	set_variables(&t1, &frames, &fps_timer, &fps, &quit, &world_time, &distance, &car_speed_x, &car_speed_y, &points, &road_width, &xpoints, &powerup_exist, &enemy_exist, &powerup_time, &npc_exist, &bullet1_counter, &bullet2_counter, &bullet_powerup_counter, bullet1_exist, bullet2_exist, bullet_powerup_exist, &nopoints_time, &car_number, &previous_points, &no_cars, &przesuniecie, &delta_position, &expected_road_width);

	while (!quit) {

		time_calculations(&delta_time, &t1, &t2, &world_time, &distance, &move, &car_speed_x, &car_speed_y, &fps_timer, &fps, &frames, &points, &xpoints, &nopoints_time, &przesuniecie, &car_position, &delta_position);
		movement_calculations(&world_time, &delta_time, &powerup_position, &npc_position, &car_position, &enemy_position, &delta_position, &car_speed_x, &car_speed_y, road_width);

		SDL_FillRect(screen, NULL, czarny);

		//rysowanie drogi
		if (((int)world_time) % 5 == 0 && (int)(world_time - delta_time) % 5 != 0) expected_road_width = change_road_width();
		road_calculations(&road_width, &expected_road_width);
		draw_road(screen, road_width, zielony, bialy, &przesuniecie);

		//rysowanie powerupa
		if (!powerup_exist && ((int)world_time) % POWERUP_SPAWN == 0 && (int)(world_time - delta_time) % POWERUP_SPAWN != 0) create_powerup(&powerup_exist, &powerup_position);
		if (powerup_exist)
		{
			DrawSurface(screen, powerup, (int)powerup_position.x, (int)powerup_position.y);
		}

		//rysowanie bulletów
		draw_bullets(screen, bullet1, bullet1_exist, bullet1_position, delta_time);
		draw_bullets(screen, bullet1, bullet_powerup_exist, bullet_powerup_position, delta_time);

		//rysowanie npc
		if (!npc_exist && ((int)world_time) % NPC_SPAWN == 0 && (int)(world_time - delta_time) % NPC_SPAWN != 0) create_npc(&npc_exist, &npc_position, &car_position);
		if (npc_exist)
		{
			DrawSurface(screen, npc, (int)npc_position.x, (int)npc_position.y);
		}

		//rysowanie enemy
		if (!enemy_exist && ((int)world_time) % ENEMY_SPAWN == 0 && (int)(world_time - delta_time) % ENEMY_SPAWN != 0) create_enemy(&enemy_exist, &enemy_position, &car_position);
		if (enemy_exist)
		{
			DrawSurface(screen, enemy, (int)enemy_position.x, (int)enemy_position.y);
		}

		//rysowanie samochodu 
		DrawSurface(screen, car, (int)car_position.x, (int)car_position.y);

		//rysowanie opisów
		draw_game_controls(screen, charset, czerwony, niebieski);
		draw_time_fps(screen, charset, czerwony, niebieski, world_time, fps, points, distance, car_number);

		


		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
//		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);


		//dodawanie samochodów
		add_car(&car_number, &points, &previous_points);
		


		//pokrywajace sie wspolrzedne
		is_crash(&car_position, &npc_position, &enemy_position, &enemy_exist, &npc_exist, &car_number, &world_time);

		is_powerup_taken(&powerup_position, &car_position, &powerup_exist, &powerup_time, &delta_time);
		//printf("powerup_time: %.1lf powerup_exist: %.1lf\n", powerup_time, powerup_exist);
		is_npc_out_of_screen(&npc_position, &npc_exist);
		is_enemy_shot(bullet1_position, bullet_powerup_position, &enemy_position, &enemy_exist, bullet1_exist, bullet_powerup_exist, &points, &nopoints_time);
		is_npc_shot(bullet1_position, bullet_powerup_position, &npc_position, &npc_exist, bullet1_exist, bullet_powerup_exist, &points, &nopoints_time);

		//koniec
		if(car_number<=0)
		{
			draw_no_cars(screen, charset, scrtex, renderer, zielony, czerwony, czarny, &no_cars, &car_number, &delta_time, &points, &world_time, &event, &quit);
			set_variables(&t1, &frames, &fps_timer, &fps, &quit, &world_time, &distance, &car_speed_x, &car_speed_y, &points, &road_width, &xpoints, &powerup_exist, &enemy_exist, &powerup_time, &npc_exist, &bullet1_counter, &bullet2_counter, &bullet_powerup_counter, bullet1_exist, bullet2_exist, bullet_powerup_exist, &nopoints_time, &car_number, &previous_points, &no_cars, &przesuniecie, &delta_position, &expected_road_width);
			set_position(&car_position);
		}
		
		// obsługa zdarzeń
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					else if (event.key.keysym.sym == SDLK_UP && car_position.y > CAR_HEIGHT) car_speed_y = -1.0;
					else if (event.key.keysym.sym == SDLK_DOWN && car_position.y < SCREEN_HEIGHT - CAR_HEIGHT) car_speed_y = 1.0;
					else if (event.key.keysym.sym == SDLK_RIGHT && car_position.x < SCREEN_WIDTH - TEXTBOX_WIDTH - CAR_WIDTH / 2 - road_width) car_speed_x = 1.0;
					else if (event.key.keysym.sym == SDLK_LEFT && car_position.x > CAR_WIDTH / 2 + road_width) car_speed_x = -1.0;
					else if (event.key.keysym.sym == SDLK_n) { set_variables(&t1, &frames, &fps_timer, &fps, &quit, &world_time, &distance, &car_speed_x, &car_speed_y, &points, &road_width, &xpoints, &powerup_exist, &enemy_exist, &powerup_time, &npc_exist, &bullet1_counter, &bullet2_counter, &bullet_powerup_counter, bullet1_exist, bullet2_exist, bullet_powerup_exist, &nopoints_time, &car_number, &previous_points, &no_cars, &przesuniecie, &delta_position, &expected_road_width); set_position(&car_position);}
					else if (event.key.keysym.sym == SDLK_r) pause_game(&event, &quit, &t1);
					else if (event.key.keysym.sym == SDLK_p) draw_highscore(screen, charset, scrtex, renderer, niebieski, zielony, &event, &quit, &t1, 0, czarny, delta_time);
					else if (event.key.keysym.sym == SDLK_t) draw_highscore(screen, charset, scrtex, renderer, niebieski, zielony, &event, &quit, &t1, 1, czarny, delta_time);
					else if (event.key.keysym.sym == SDLK_SPACE) shoot(screen, &car_position, &powerup_time, bullet1, bullet1_exist, bullet1_position, bullet1_counter, bullet_powerup_exist, bullet_powerup_position, bullet_powerup_counter);
					break;
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_UP) car_speed_y = 0;
					else if (event.key.keysym.sym == SDLK_DOWN) car_speed_y = 0;
					else if (event.key.keysym.sym == SDLK_RIGHT) car_speed_x = 0;
					else if (event.key.keysym.sym == SDLK_LEFT) car_speed_x = 0;
				};
			};
		frames++;
		};

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_FreeSurface(car);
	SDL_FreeSurface(enemy);
	SDL_FreeSurface(powerup);
	SDL_FreeSurface(npc);
	SDL_FreeSurface(bullet1);
	SDL_FreeSurface(bullet2);
	Destroy(scrtex, window, renderer);

	return 0;
	};
