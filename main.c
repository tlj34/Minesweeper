#include <SDL2/SDL.h>
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#undef main
#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MOVE_CURSOR(x, y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD) {(x), (y)}) /*移动光标*/

const char chmap[11] = " 12345678@";
char *mp0, *mp1;
int width, height, mines, mode, record[3], d[8], current, flagsnum;
SDL_Window *window;
SDL_Renderer *render;
SDL_Surface *picA, *picB, *picC, *picD;

int adjoin(int p1, int p2) { //判断两点是否临接
	if (p1 < 0 || p2 < 0 || p1 >= width * height || p2 >= width * height) return 0;
	return (abs((p1 % width) - (p2 % width)) < 2 && abs(p1 / width - p2 / width) < 2);
}

int print_map() { //打印地图
	SDL_Texture *texA = SDL_CreateTextureFromSurface(render, picA);
	SDL_Texture *texB = SDL_CreateTextureFromSurface(render, picB);
	SDL_Rect scr, dst;
	flagsnum = 0;
	int cnt = 0;
	
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int a = i * width + j;
			dst = (SDL_Rect) {j * 20, i * 20 + 30, 20, 20};
			if (isdigit(mp1[a])) {
				scr = (SDL_Rect) {(mp1[a] - '1') * 16, 0, 15, 15};
				SDL_RenderCopy(render, texB, &scr, &dst);
			} else if (mp1[a] == '*') {
				scr = (SDL_Rect) {0, 0, 15, 15};
				SDL_RenderCopy(render, texA, &scr, &dst);
			} else if (mp1[a] == ' ') {
				scr = (SDL_Rect) {16, 0, 15, 15};
				SDL_RenderCopy(render, texA, &scr, &dst);
			} else if (mp1[a] == '!') {
				scr = (SDL_Rect) {32, 0, 15, 15};
				SDL_RenderCopy(render, texA, &scr, &dst);
			}

			if (mp1[a] == '!') ++flagsnum;
			if (mp1[a] == '*' || mp1[a] == '!') ++cnt;
			else if (mp1[a] == '@') {
				for (int k = 0; k < height; ++k) {
					for (int l = 0; l < width; ++l) {
						int b = k * width + l;
						dst = (SDL_Rect) {l * 20, k * 20 + 30, 20, 20};
						if (mp0[b] == 9) {
							if (mp1[b] == '!') scr = (SDL_Rect) {32, 0, 15, 15};
							else if (mp1[b] == '@') scr = (SDL_Rect) {96, 0, 15, 15};
							else scr = (SDL_Rect) {80, 0, 15, 15};
							SDL_RenderCopy(render, texA, &scr, &dst);
						} else if (mp1[b] == '!') {
							scr = (SDL_Rect) {112, 0, 15, 15};
							SDL_RenderCopy(render, texA, &scr, &dst);
						} else if (isdigit(mp1[b])) {
							scr = (SDL_Rect) {(mp1[b] - '1') * 16, 0, 15, 15};
							SDL_RenderCopy(render, texB, &scr, &dst);
						} else if (mp1[b] == '*') {
							scr = (SDL_Rect) {0, 0, 15, 15};
							SDL_RenderCopy(render, texA, &scr, &dst);
						} else if (mp1[b] == ' ') {
							scr = (SDL_Rect) {16, 0, 15, 15};
							SDL_RenderCopy(render, texA, &scr, &dst);
						}
					}
				}
				
				SDL_RenderPresent(render);
				SDL_DestroyTexture(texA);
				SDL_DestroyTexture(texB);
				return -1;
			}
		}
	}
	
	SDL_RenderPresent(render);
	SDL_DestroyTexture(texA);
	SDL_DestroyTexture(texB);

	return cnt == mines;
}

void dig(int n) { //挖掘
	if (mp1[n] != '*') return;
	mp1[n] = chmap[(int)mp0[n]];
	if (mp1[n] == ' ') for (int i = 0; i < 8; ++i) if (adjoin(n, n + d[i])) dig(n + d[i]);
}

void game() { //游戏主体
	bool step1, cheats;
	time_t startTime;
	char code[6] = "xyzzy"; //作弊码
	int tmp;
	SDL_Event event;
	SDL_Rect scr, dst;
	
start:
	step1 = 1, cheats = 0, tmp = 0;
	SDL_Texture *texC = SDL_CreateTextureFromSurface(render, picC);
	SDL_Texture *texD = SDL_CreateTextureFromSurface(render, picD);
	memset(mp0, 0, width * height), memset(mp1, '*', width * height);
	SDL_SetRenderDrawColor(render, 0xa0, 0xa0, 0xa0, 0xff);
	SDL_RenderClear(render);
	system("cls");

	while (1) {
		SDL_PollEvent(&event);
		int flag = print_map();
		int Time = step1 ? 0 : time(0) - startTime;
		int surplus = mines - flagsnum;
		if (surplus >= 0) {
			scr = (SDL_Rect) {surplus / 100 % 10 * 13, 0, 12, 22};
			dst = (SDL_Rect) {0, 0, 14, 24};
			SDL_RenderCopy(render, texC, &scr, &dst);
			scr = (SDL_Rect) {surplus / 10 % 10 * 13, 0, 12, 22};
			dst = (SDL_Rect) {14, 0, 14, 24};
			SDL_RenderCopy(render, texC, &scr, &dst);
			scr = (SDL_Rect) {surplus % 10 * 13, 0, 12, 22};
			dst = (SDL_Rect) {28, 0, 14, 24};
			SDL_RenderCopy(render, texC, &scr, &dst);
		}
		scr = (SDL_Rect) {Time / 100 % 10 * 13, 0, 12, 22};
		dst = (SDL_Rect) {width * 20 - 42, 0, 14, 24};
		SDL_RenderCopy(render, texC, &scr, &dst);
		scr = (SDL_Rect) {Time / 10 % 10 * 13, 0, 12, 22};
		dst = (SDL_Rect) {width * 20 - 28, 0, 14, 24};
		SDL_RenderCopy(render, texC, &scr, &dst);
		scr = (SDL_Rect) {Time % 10 * 13, 0, 12, 22};
		dst = (SDL_Rect) {width * 20 - 14, 0, 14, 24};
		SDL_RenderCopy(render, texC, &scr, &dst);
		scr = (SDL_Rect) {0, 0, 24, 24};
		dst = (SDL_Rect) {width * 10 - 12, 0, 24, 24};
		SDL_RenderCopy(render, texD, &scr, &dst);
		
		SDL_RenderPresent(render);
		if (SDL_QUIT == event.type) {
			SDL_DestroyTexture(texC);
			return;
		} else if (SDL_MOUSEBUTTONDOWN == event.type) {
			if (SDL_BUTTON_LEFT == event.button.button) {
				if (event.button.y > 30) {
					current = (event.button.y - 30) / 20 * width + event.button.x / 20;
					if (mp1[current] == '!') continue;
					if (step1) {
						for (int i = 0; i < mines; ++i) {
							int a = rand() % (width * height);
							if (mp0[a] < 9 && a ^ current) {
								mp0[a] = 9;
								for (int i = 0; i < 8; ++i)
									if (adjoin(a, a + d[i]) && mp0[a + d[i]] < 9)
										++mp0[a + d[i]];
							} else --i;
						}
						step1 = 0;
						startTime = time(0);
					}
					dig(current);
				} else if (event.button.x > width * 10 - 12 && event.button.x < width * 10 + 12) goto start;
			} else if (SDL_BUTTON_RIGHT == event.button.button && event.button.y > 30) {
				current = (event.button.y - 30) / 20 * width + event.button.x / 20;
				if (mp1[current] == '*') mp1[current] = '!';
				else if (mp1[current] == '!') mp1[current] = '*';
			}
		} else if (SDL_MOUSEBUTTONUP == event.type && SDL_BUTTON_RIGHT == event.button.button) {
			if (isdigit(mp1[current])) {
				int cnt = 0;
				for (int i = 0; i < 8; ++i)
					if (adjoin(current, current + d[i]) && mp1[current + d[i]] == '!') ++cnt;
				if (cnt == mp1[current] - '0')
					for (int i = 0; i < 8; ++i)
						if (adjoin(current, current + d[i])) dig(current + d[i]);
			}
		} else if (SDL_KEYDOWN == event.type) { //核对作弊码
			if (tolower(event.key.keysym.sym) == code[tmp]) cheats = (cheats + (tmp == 4)) % 2, tmp += tmp < 4 ? 1 : -4;
			else tmp = 0;
		}

		//处理作弊模式
		MOVE_CURSOR(0, 0);
		if (cheats && mp0[current] == 9) putchar('.');
		else if (tmp) putchar('*');
		else putchar(' ');

		if (flag == 1) {
			scr = (SDL_Rect) {77, 0, 24, 24};
			dst = (SDL_Rect) {width * 10 - 12, 0, 24, 24};
			SDL_RenderCopy(render, texD, &scr, &dst);
			SDL_RenderPresent(render);
			printf("You win!\n");
			if (mode < 3) record[mode] = MIN(record[mode], time(0) - startTime);
			goto end;
		} else if (flag == -1) {
			scr = (SDL_Rect) {102, 0, 24, 24};
			dst = (SDL_Rect) {width * 10 - 12, 0, 24, 24};
			SDL_RenderCopy(render, texD, &scr, &dst);
			SDL_RenderPresent(render);
			printf("Game over!\n");
			goto end;
		}
		
		current = (current + width * height) % (width * height);
		SDL_RenderPresent(render);
	}

end:
	SDL_PollEvent(&event);
	if (SDL_MOUSEBUTTONDOWN == event.type) {
		if (SDL_BUTTON_LEFT == event.button.button)
			if (event.button.x > width * 10 - 12 && event.button.y < width * 10 + 12)
				goto start;
	} else if (SDL_QUIT == event.type) {
		SDL_DestroyTexture(texC);
		return;
	}
	goto end;
}

int main() {
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &(CONSOLE_CURSOR_INFO){1, 0}); //隐藏光标
	srand((unsigned int)time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	
	//获取图像
	picA = SDL_LoadBMP("./pics/A.bmp");
	picB = SDL_LoadBMP("./pics/B.bmp");
	picC = SDL_LoadBMP("./pics/C.bmp");
	picD = SDL_LoadBMP("./pics/D.bmp");

	//获取记录
	FILE *fp = fopen("record.txt", "r");
	if (fp) fscanf(fp, "%d\n%d\n%d", &record[0], &record[1], &record[2]);
	else record[0] = record[1] = record[2] = 999;
	
	while (1) {
		//打印游戏菜单
		MOVE_CURSOR(0, 0);
		system("cls");
		printf("    Beginner        record: %d\n", record[0]);
		printf("  Intermediate      record: %d\n", record[1]);
		printf("     Expter         record: %d\n", record[2]);
		printf("  Custom Borad  \n      Quit      ");

		MOVE_CURSOR(0, mode),	putchar('[');
		MOVE_CURSOR(15, mode),	putchar(']');
		char ch = getch();
		
		if (ch == 'z' || ch == 'Z' || ch == ' ' || ch == 13) {
			if (mode == 4) break;
			else if (mode == 0) width = height = 9, mines = 10;
			else if (mode == 1) width = height = 16, mines = 40;
			else if (mode == 2) width = 32, height = 16, mines = 99;
			else if (mode == 3) {
				system("cls");
				printf(".Width=");
				scanf("%d", &width);
				printf(".Height=");
				scanf("%d", &height);
				printf(".Mines=");
				scanf("%d", &mines);
				if (width < 1) width = 1;
				if (height < 1) height = 1;
				if (width > 32) width = 32;
				if (height > 32) height = 32;
				if (mines >= width * height) mines = width * height - 1;
			}

			//初始化方向数组、分配内存、创建窗口并开始游戏
			d[0] = -width - 1, d[1] = -width, d[2] = -width + 1, d[3] = -1, d[4] = 1, d[5] = width - 1, d[6] = width, d[7] = width + 1;
			mp0 = (char*)malloc(width * height), mp1 = (char*)malloc(width * height);
			window = SDL_CreateWindow("mine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * 20, height * 20 + 30, SDL_WINDOW_SHOWN);
			render = SDL_CreateRenderer(window, -1, 0);
			game();

			//释放内存并关闭窗口
			free(mp0), free(mp1);
			SDL_DestroyWindow(window);
			SDL_Quit();
		} else if (ch == 'x' || ch == 'X') mode = 4;
		else if (ch == 72) --mode;
		else if (ch == 80) ++mode;

		mode = (mode + 5) % 5;
	}
	
	//将记录写入文本中
	fp = fopen("record.txt", "w");
	fprintf(fp, "%d\n%d\n%d", record[0], record[1], record[2]);
	fclose(fp);
	
	SDL_FreeSurface(picA);
	SDL_FreeSurface(picB);
	SDL_FreeSurface(picC);
	SDL_FreeSurface(picD);
	SDL_Quit();
	return 0;
}
