#define _CRT_SECURE_NO_WARNINGS

/*头文件*/
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>

/*宏定义*/
#define U 1
#define D 2
#define L 3
#define R 4													// 蛇的状态：上下左右

/*定义全局变量*/
typedef struct snake										// 蛇身的一个节点
{
	int x;													// 节点的x坐标
	int y;													// 节点的y坐标
	struct snake* next;										// 蛇身的下一个节点
}snake;

int score = 0, add = 1;										// 总得分与每次吃食物得分
int HighScore = 0;											// 最高分
int status, sleeptime = 350;									// 蛇前进状态，每次运行的时间间隔
snake* head, * food;										// 蛇头的指针，食物指针
snake* q;													// 遍历蛇时用到的指针
int endgamestatus = 0;										// 游戏结束的情况：撞到墙；咬到自己；主动退出游戏
HANDLE hOut;												// 控制台句柄

/*函数声明*/
void gotoxy(int x, int y);									// 设置光标位置
int color(int c);											// 更改文字颜色
void printsnake();											// 字符画，蛇
void welcometogame();										// 开始界面
void createMap();											// 绘制地图
void scoreandtips();										// 游戏界面右侧的得分和小提示
void initsnake();											// 初始化蛇身，画蛇身
void createfood();											// 创建并随机出现食物
int biteself();												// 判断是否咬到自己
void cantcrosswall();										// 判断是否撞墙
void speedup();												// 加速
void speeddown();											// 减速
void snakemove();											// 控制蛇前进方向
void keyboardControl();										// 控制键盘按键
void Lostdraw();											// 游戏结束界面
void endgame();												// 游戏结束
void choose();												// 游戏失败后的选择
void File_out();											// 在文件中读取最高分
void File_in();												// 将最高分存进文件
void explation();											// 游戏说明

/*
	文字颜色函数
*/
int color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);					// 更改文字颜色
	return 0;
}

/*
	设置光标位置
*/
void gotoxy(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);					// 定位光标位置
}

/*
	字符蛇
*/
void printsnake()
{
	gotoxy(35, 1);
	color(6);
	printf("/^\\/^\\");																// 蛇眼睛

	gotoxy(34, 2);
	printf("|___|  0|");															// 蛇眼睛

	gotoxy(33, 2);
	color(2);
	printf("_");

	gotoxy(25, 3);
	color(12);
	printf("\\/");																	// 蛇信

	gotoxy(31, 3);
	color(2);
	printf("/");

	gotoxy(37, 3);
	color(6);
	printf(" \\_/");																// 蛇眼睛

	gotoxy(41, 3);
	color(10);
	printf(" \\");

	gotoxy(26, 4);
	color(12);
	printf("\\_____");																// 舌头

	gotoxy(32, 4);
	printf("___________/");

	gotoxy(31, 4);
	color(2);
	printf("|");

	gotoxy(43, 4);
	color(10);
	printf("\\");

	gotoxy(32, 5);
	color(2);
	printf("\\________");															// 蛇嘴

	gotoxy(44, 5);
	color(10);
	printf("\\");

	gotoxy(39, 6);
	printf("|      |                   \\");											// 下面都是画蛇身

	gotoxy(38, 7);
	printf("/      /                     \\");

	gotoxy(37, 8);
	printf("/      /                     \\  \\");

	gotoxy(35, 9);
	printf("/      /                        \\  \\");

	gotoxy(34, 10);
	printf("/     /                           \\  \\");

	gotoxy(33, 11);
	printf("/     /            _----_           \\  \\");

	gotoxy(32, 12);
	printf("/     /          _-~       ~-_       |   |");

	gotoxy(31, 13);
	printf("(       (      _-~     _--_    ~-_   _/    |");

	gotoxy(32, 14);
	printf("\\      ~-___-~   _-~     ~-_    ~-_-~    /");

	gotoxy(33, 15);
	printf("~-_           _-~          ~-_        _-~");

	gotoxy(35, 16);
	printf("~--_______-~               ~-____-~");
}

/*
	欢迎界面
*/
void welcometogame()
{
	int n;
	int i, j = 1;
	gotoxy(43, 18);
	color(11);
	printf("贪 吃 蛇 大 作 战");
	color(14);														// 黄色边框
	for (i = 20; i <= 26; i++)										// 输出上下边框---
	{
		for (j = 27; j <= 74; j++)
		{
			gotoxy(j, i);
			if (i == 20 || i == 26)
			{
				printf("-");
			}
			else if (j == 27 || j == 74)
			{
				printf("|");
			}
		}
	}
	color(12);
	gotoxy(35, 22);													// 设置文字显示位置
	printf("1.开始游戏");
	gotoxy(55, 22);
	printf("2.游戏说明");
	gotoxy(35, 24);
	printf("3.退出游戏");
	gotoxy(29, 27);
	color(3);
	printf("请选择【1 2 3】:【  】\b\b\b");
	color(14);
	scanf("%d", &n);
	switch (n)
	{
	case 1:
		system("cls");
		createMap();
		initsnake();
		createfood();
		break;
	case 2:
		explation();
		break;
	case 3:
		exit(0);
		break;
	default:
		color(12);
		gotoxy(48, 28);
		printf("请输入1-3之间的数");
		_getch();													// 按下任意键
		system("cls");
		printsnake();
		welcometogame();

	}
}

/*
	创建地图
*/
void createMap()
{
	int i, j;
	for (i = 0; i < 58; i += 2)										// 打印上下边框
	{
		gotoxy(i, 0);
		color(5);
		printf("□");
		gotoxy(i, 26);
		printf("□");
	}
	for (i = 1; i < 26; i++)										// 打印左右边框
	{
		gotoxy(0, i);
		printf("□");
		gotoxy(56, i);
		printf("□");
	}
	for (i = 2; i < 56; i += 2)										// 打印中间网格
	{
		for (j = 1; j < 26; j++)
		{
			gotoxy(i, j);
			color(3);
			printf("■\n\n");
		}
	}
}

/*
	游戏界面右侧的得分和小提示
*/
void scoreandtips()
{
	File_out();														// 读取文件save.txt中的内容
	gotoxy(64, 4);
	color(11);
	printf("☆最高纪录☆: %d", HighScore);
	gotoxy(64, 8);
	color(14);
	printf("得分: %d  ", score);
	color(13);
	gotoxy(73, 11);
	printf("小 提 示");
	gotoxy(60, 13);
	color(6);
	printf("+-----------------------------------+");
	gotoxy(60, 25);
	printf("+-----------------------------------+");
	color(3);
	gotoxy(64, 14);
	printf("每个食物得分: %d", add);
	gotoxy(64, 16);
	printf("不能撞墙，不能咬到自己");
	gotoxy(64, 18);
	printf("用↑ ↓ ← →分别控制蛇的移动");
	gotoxy(64, 20);
	printf("F1：加速，F2：减速");
	gotoxy(64, 22);
	printf("Space：暂停游戏");
	gotoxy(64, 24);
	printf("Esc：退出游戏");
}

/*
	在文件中读取最高分
*/
void File_out()
{
	FILE* fp;
	fp = fopen("save.txt", "a+");
	fscanf(fp, "%d", &HighScore);
	fclose(fp);
}

/*
	初始化蛇身，画蛇身
*/
void initsnake()
{
	snake* tail;
	int i;
	tail = (snake*)malloc(sizeof(snake));											// 从蛇尾开始，头插法，以xy设定开始位置
	tail->x = 24;
	tail->y = 5;
	tail->next = NULL;
	for (i = 1; i <= 4; i++)
	{
		head = (snake*)malloc(sizeof snake);
		head->next = tail;
		head->x = 24 + 2 * i;
		head->y = 5;
		tail = head;
	}
	while (tail != NULL)
	{
		gotoxy(tail->x, tail->y);
		color(14);
		printf("★");																// 输出蛇身
		tail = tail->next;
	}
}

/*
	随机出现食物
*/
void createfood()
{
	snake* food_1;
	srand((unsigned)time(NULL));
	food_1 = (snake*)calloc(1, sizeof snake);
	// 保证其为偶数，使得食物能与蛇头对齐，然后食物会出现在网格线上
	food_1->x = 1;
	while ((food_1->x % 2) != 0)
	{
		food_1->x = rand() % 52 + 2;
	}
	food_1->y = rand() % 24 + 1;
	if (food_1->x == 0 || food_1->x == 56 || food_1->y == 0 || food_1->y == 26)		// 如果食物在边框
	{
		free(food_1);
		createfood();
	}
	q = head;
	while (q->next != NULL)
	{
		if (q->x == food_1->x && q->y == food_1->y)									// 判断蛇身是否与食物重合
		{
			free(food_1);
			createfood();															// 重新创建食物
		}
		
		q = q->next;
	}
	gotoxy(food_1->x, food_1->y);													// 设置食物位置
	food = food_1;
	//free(food_1);
	color(12);
	printf("●");
}

/*
	判断是否咬到了自己
*/
int biteself()
{
	snake* self;																	// 定义self为蛇身上的一个节点
	self = head->next;
	while (self != NULL)
	{
		if (self->x == head->x && self->y == head->y)								// 如果self和蛇头重合
		{
			return 1;																
		}
		self = self->next;
	}
	return 0;
}

/*
	设置蛇撞墙的情况
*/
void cantcrosswall()
{
	if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26)
	{
		endgamestatus = 1;
		endgame();
	}
}

/*
	加速，蛇吃到食物或按F1键，会自动提速
*/
void speedup()
{
	if (sleeptime >= 50)													// 如果时间间隔大于等于50
	{
		sleeptime -= 30;
		add += 2;															// 每吃一次食物的得分加2
	}
}

/*
	减速，按F2键
*/
void speeddown()
{
	if (sleeptime < 350)
	{
		sleeptime += 30;
		if (add > 2)
		{
			add -= 2;
		}
		//if (sleeptime >= 350)
		//{
		//	add = 1;														// 保证最低分为1
		//}
	}
}

/*
	控制方向
*/
void snakemove()																	// 蛇前进 上U 下D 左L 右R
{
	snake* nexthead;
	cantcrosswall();
	nexthead = (snake*)malloc(sizeof snake);										// 为下一步开辟空间
	if (status == U)
	{
		nexthead->x = head->x;														// 向上前进时，x坐标不变，y坐标-1
		nexthead->y = head->y - 1;
		nexthead->next = head;
		head = nexthead;
		q = head;																	// 指针q指向蛇头
		// 如果下一个位置上有食物，下一个位置的坐标和食物的坐标相同
		if (nexthead->x == food->x && nexthead->y == food->y)
		{
			while (q != NULL)
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");														// 原来食物的位置，将食物替换成蛇身
				q = q->next;														// 指针q指向蛇身的下一位，循环操作
			}
			score += add;
			speedup();
			createfood();
		}
		else
		{
			while (q->next->next != NULL)											// 如果没遇到食物
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");														// 蛇正常往前走，输出当前位置的蛇身
				q = q->next;
			}
			// 经过上面的循环，q指向旧蛇尾的上一位（即新蛇尾），将旧蛇尾取消
			gotoxy(q->next->x, q->next->y);
			color(3);
			printf("■");
			free(q->next);															// 释放旧蛇尾
			q->next = NULL;
		}
	}
	if (status == D)
	{
		nexthead->x = head->x;														// 向下前进时，x坐标不变，y坐标+1
		nexthead->y = head->y + 1;
		nexthead->next = head;
		head = nexthead;
		q = head;
		if (nexthead->x == food->x && nexthead->y == food->y)						// 吃到了食物
		{
			while (q != NULL)
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");
				q = q->next;
			}
			score += add;
			speedup();
			createfood();
		}
		else
		{
			while (q->next->next != NULL)											// 如果没遇到食物
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");														// 蛇正常往前走，输出当前位置的蛇身
				q = q->next;
			}
			// 经过上面的循环，q指向旧蛇尾的上一位（即新蛇尾），将旧蛇尾取消
			gotoxy(q->next->x, q->next->y);
			color(3);
			printf("■");
			free(q->next);															// 释放旧蛇尾
			q->next = NULL;
		}
	}
	if (status == L)
	{
		nexthead->x = head->x - 2;														// 向左前进时，x坐标-2，y坐标不变
		nexthead->y = head->y;
		nexthead->next = head;
		head = nexthead;
		q = head;
		if (nexthead->x == food->x && nexthead->y == food->y)						// 吃到了食物
		{
			while (q != NULL)
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");
				q = q->next;
			}
			score += add;
			speedup();
			createfood();
		}
		else
		{
			while (q->next->next != NULL)											// 如果没遇到食物
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");														// 蛇正常往前走，输出当前位置的蛇身
				q = q->next;
			}
			// 经过上面的循环，q指向旧蛇尾的上一位（即新蛇尾），将旧蛇尾取消
			gotoxy(q->next->x, q->next->y);
			color(3);
			printf("■");
			free(q->next);															// 释放旧蛇尾
			q->next = NULL;
		}
	}
	if (status == R)
	{
		nexthead->x = head->x + 2;														// 向右前进时，x坐标+2，y坐标不变
		nexthead->y = head->y;
		nexthead->next = head;
		head = nexthead;
		q = head;
		if (nexthead->x == food->x && nexthead->y == food->y)						// 吃到了食物
		{
			while (q != NULL)
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");
				q = q->next;
			}
			score += add;
			speedup();
			createfood();
		}
		else
		{
			while (q->next->next != NULL)											// 如果没遇到食物
			{
				gotoxy(q->x, q->y);
				color(14);
				printf("★");														// 蛇正常往前走，输出当前位置的蛇身
				q = q->next;
			}
			// 经过上面的循环，q指向旧蛇尾的上一位（即新蛇尾），将旧蛇尾取消
			gotoxy(q->next->x, q->next->y);
			color(3);
			printf("■");
			free(q->next);															// 释放旧蛇尾
			q->next = NULL;
		}
	}
	if (biteself())
	{
		endgamestatus = 2;
		endgame();
	}
}

/*
	控制键盘按键
*/
void keyboardControl()
{
	status = R;																		// 初始蛇向右移动
	while (1)
	{
		scoreandtips();																// 游戏界面右侧的得分和小提示
		// GeyAsyncKeyState函数用来判断函数调用时指定虚拟键的状态
		if (GetAsyncKeyState(VK_UP) && status != D)
		{
			status = U;																// 如果蛇不是向下前进时，按向上键↑，执行向上前进
		}
		else if (GetAsyncKeyState(VK_DOWN) && status != U)
		{
			status = D;																// 如果蛇不是向上前进时，按向下键↓，执行向下前进
		}
		else if (GetAsyncKeyState(VK_LEFT) && status != R)
		{
			status = L;																// 如果蛇不是向右前进时，按向左键←，执行向左前进
		}
		else if (GetAsyncKeyState(VK_RIGHT) && status != L)
		{
			status = R;																// 如果蛇不是向左前进时，按向右键→，执行向右前进
		}
		if (GetAsyncKeyState(VK_SPACE))												// 按暂停键，暂停进程
		{
			while (1)
			{
				Sleep(300);
				if (GetAsyncKeyState(VK_SPACE))
				{
					break;
				}
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE))
		{
			endgamestatus = 3;														// 按ESC键，跳转到结束界面
			break;
		}
		else if (GetAsyncKeyState(VK_F1))											// 按F1键，加速
		{
			speedup();
		}
		else if (GetAsyncKeyState(VK_F2))											// 按F2键，减速
		{
			speeddown();
		}
		Sleep(sleeptime);
		snakemove();																// 不按键时，蛇保持前进
	}
}

/*
	失败界面
*/
void Lostdraw()
{
	system("cls");
	int i;
	gotoxy(46, 2);
	color(6);
	printf("\\\\\\|///");															// 小人的头发
	gotoxy(43, 3);
	printf("\\\\");
	gotoxy(47, 3);
	color(15);
	printf(".-.-");																	// 眉毛
	gotoxy(54, 3);
	color(6);
	printf("//");

	gotoxy(44, 4);
	color(14);
	printf("(");																	// 左耳

	gotoxy(47, 4);
	color(15);
	printf(".@.@");																	// 眼睛

	gotoxy(54, 4);
	color(14);
	printf(")");																	// 右耳

	gotoxy(17, 5);
	color(11);
	printf("+------------------------");											// 上边框

	gotoxy(35, 5);
	color(14);
	printf("o00o");																	// 左手

	gotoxy(39, 5);
	color(11);
	printf("----------");															// 上边框

	gotoxy(48, 5);
	color(14);
	printf("(_)");																	// 嘴

	gotoxy(51, 5);
	color(11);
	printf("----------");															// 上边框

	gotoxy(61, 5);
	color(14);
	printf("o00o");																	// 右手

	gotoxy(65, 5);
	color(11);
	printf("-----------------+");													// 上边框

	for (i = 6; i <= 19; i++)														// 竖边框
	{
		gotoxy(17, i);
		printf("|");
		gotoxy(82, i);
		printf("|");
	}

	gotoxy(17, 20);
	printf("+-----------------------------");									// 下边框

	gotoxy(48, 20);
	color(14);
	printf("☆☆☆");

	gotoxy(52, 20);
	color(11);
	printf("------------------------------+");												// 下边框
}

/*
	结束游戏
*/
void endgame()
{
	system("cls");
	if (endgamestatus == 1)															// 如果撞到了墙
	{
		Lostdraw();
		gotoxy(35, 9);
		color(12);
		printf("撞墙，游戏结束");
	}
	else if (endgamestatus == 2)													// 如果咬到自己
	{
		Lostdraw();
		gotoxy(35, 9);
		color(12);
		printf("咬到自己，游戏结束");
	}
	else if (endgamestatus == 3)													// 如果按Esc退出
	{
		Lostdraw();
		gotoxy(40, 9);
		color(12);
		printf("您已经结束了游戏");
	}
	gotoxy(43, 12);
	color(13);
	printf("您的得分是 %d", score);
	if (score >= HighScore)															// 如果分数高于最高分
	{
		color(10);
		gotoxy(33, 16);
		printf("刷新最高纪录！");
		File_in();																	// 把最高分写进文件
	}
	else
	{
		color(10);
		gotoxy(33, 16);
		printf("离最高分还差：%d", HighScore - score);
	}
	choose();
}

/*
	将最高分存储于文件中
*/
void File_in()
{
	FILE* fp;
	fp = fopen("save.txt", "w+");
	fprintf(fp, "%d", score);														// 将分数写进文件
	fclose(fp);
}

/*
	边框下面的分支选项
*/
void choose()
{
	int n;
	gotoxy(25, 23);
	color(12);
	printf("我要重新玩一局-------1");
	gotoxy(52, 23);
	printf("不玩了，退出吧-------2");
	gotoxy(46, 25);
	color(11);
	printf("选择：");
	scanf("%d", &n);
	switch (n)
	{
	case 1:
		system("cls");
		score = 0;
		sleeptime = 350;													// 设定初始速度
		add = 1;
		printsnake();
		welcometogame();
		break;
	case 2:
		exit(0);
		break;
	default:
		gotoxy(35, 27);
		color(12);
		printf("##您的输入有误，请重新输入##");
		system("pause >nul");												// 按任意键
		endgame();
		choose();
		break;
	}
}

/*
	游戏说明
*/
void explation()
{
	int i, j = 1;
	system("cls");
	color(13);
	gotoxy(44, 3);
	printf("游戏说明");
	color(2);
	for (i = 6; i <= 22; i++)														// 输出上下边框
	{
		for (j = 20; j <= 75; j++)													// 输出左右边框
		{
			gotoxy(j, i);
			if (i == 6 || i == 22) printf("=");
			else if (j == 20 || j == 75) printf("||");
		}
	}
	color(3);
	gotoxy(30, 8);
	printf("tip1: 不能撞墙，不能咬到自己");
	color(10);
	gotoxy(30, 11);
	printf("tip2: 用↑ ↓ ← →分别控制蛇的移动");
	color(14);
	gotoxy(30, 14);
	printf("tip3: F1为加速，F2为减速");
	color(11);
	gotoxy(30, 17);
	printf("tip4: 按空格键暂停游戏，再按空格键继续");
	color(4);
	gotoxy(30, 20);
	printf("tip5: 按Esc退出游戏");
	_getch();
	system("cls");
	printsnake();
	welcometogame();
}

/*
	主函数
*/
int main()
{
	system("mode con cols=100 lines=30");											// 设置控制台的宽高
	printsnake();
	welcometogame();
	File_out();
	keyboardControl();
	endgame();
	return 0;																		
}










