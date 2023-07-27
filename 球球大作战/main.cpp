#include<stdio.h>
#include<easyx.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
/*
*球球大作战
*/

const int GridSize = 10; //网格大小


//玩家、食物、人机
/*
* 都是图形，需要坐标（x，y），颜色，半径，标记（isDie）
*/
//封装->结构体
struct Sprite   //精灵
{
	double x;  //x坐标
	double y;  //y坐标
	DWORD color;  //颜色
	double r;  //半径
	bool isDie;  //是否死亡
	char name[20];  //名字

	int food_index;  //食物索引
};
//提供函数来操作Sprite结构体
void sprite_init(Sprite* spr, double x, double y, double r, const char* name = "Sprite")  //初始化
{
	spr->x = x;
	spr->y = y;
	spr->r = r;
	spr->color = RGB(rand() % 256, rand() % 256, rand() % 256);
	spr->isDie = false;
	strcpy(spr->name, name);
	spr->food_index = -1;
}
//绘制
void sprite_draw(Sprite* spr, bool showName = true)
{
	//判断是否还活着
	if (!spr->isDie)
	{
		//填充颜色
		setfillcolor(spr->color);
		//画圆
		solidcircle(spr->x, spr->y, spr->r);
		if (showName)  //是否显示名字
		{
			//设置文字颜色
			settextcolor(RGB(216, 39, 211));
			//设置背景透明
			setbkmode(TRANSPARENT);
			//设置字体样式
			settextstyle(20, 0, "楷体");
			//绘制名字
			outtextxy(spr->x - textwidth(spr->name) / 2, spr->y - textheight(spr->name) / 2, spr->name);
		}

	}
}

//移动函数
void sprite_moveBy(Sprite* spr, double dx, double dy)
{
	spr->x += dx;
	spr->y += dy;
}

//获得两个sprite之间的距离
double sprite_getDistance(Sprite* spr1, Sprite* spr2)
{
	double dx = spr1->x - spr2->x;
	double dy = spr1->y - spr2->y;
	return sqrt(dx * dx + dy * dy);
}

#define FOOD_NUM 200  //食物数量
#define AI_NUM 10  //人机数量
Sprite gamer;  //玩家
Sprite foods[FOOD_NUM];  //食物
Sprite ai[AI_NUM];  //人机
const char* aiNames[] = { "阿布","Awkae","无心","Ro","马飞飞","科马","卢本伟","Pdd","乌鸦坐飞机","JJ" };  //人机名字



//初始化
void init()
{
	//初始化随机数种子
	srand((unsigned int)time(NULL));
	//初始化玩家
	sprite_init(&gamer, 100, 100, 10, "Bao");
	//初始化食物
	for (int i = 0; i < FOOD_NUM; i++)
	{
		sprite_init(&foods[i], rand() % getwidth(), rand() % getheight(), 1 + rand() % 5);
	}
	//初始化人机
	for (int i = 0; i < AI_NUM; i++)
	{
		sprite_init(&ai[i], rand() % getwidth(), rand() % getheight(), 10 + rand() % 5, aiNames[i]);
	}
}

//绘制界面
void draw()
{
	//设置颜色
	setlinecolor(RGB(194, 185, 201));
	//绘制格子
	for (int i = 0; i < getheight() / 10; i++)
	{
		line(0, i * GridSize, getwidth(), i * GridSize);
	}
	for (int i = 0; i < getwidth() / 10; i++)
	{
		line(i * GridSize, 0, i * GridSize, getheight());
	}
	//绘制玩家
	sprite_draw(&gamer);
	//绘制食物
	for (int i = 0; i < FOOD_NUM; i++)
	{
		sprite_draw(&foods[i], false);
	}
	//绘制人机
	for (int i = 0; i < AI_NUM; i++)
	{
		sprite_draw(&ai[i], true);
	}
}

//按键处理
void keyDownDeal()
{
	double speed = 0.5;//移动速度
	//获取按键
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		sprite_moveBy(&gamer, 0, -speed);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		sprite_moveBy(&gamer, 0, speed);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		sprite_moveBy(&gamer, -speed, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		sprite_moveBy(&gamer, speed, 0);
	}
}

//判断能否吃食物
void gamerEatFood()
{
	for (int i = 0; i < FOOD_NUM; i++)
	{
		//判断玩家和食物的距离
		if (!foods[i].isDie && sprite_getDistance(&gamer, &foods[i]) < gamer.r)
		{
			//吃掉食物
			foods[i].isDie = true;
			//玩家半径增加
			gamer.r += foods[i].r / 5;
		}
	}

}

//玩家与ai互吃
// 判断两个精灵是否发生吃掉行为，并更新相应的半径和死亡状态
void eat(Sprite* eater, Sprite* food)
{
	if (eater->r > food->r && sprite_getDistance(eater, food) < eater->r)
	{
		// 吃掉食物或AI球
		food->isDie = true;
		// 增加吃掉者的半径
		eater->r += food->r / 5;
	}
}

// 玩家和AI球与其他AI球和食物球之间的吃掉行为
void gamerAndAiEat()
{
	// 判断玩家和AI球之间的吃掉行为
	for (int i = 0; i < AI_NUM; i++)
	{
		if (!gamer.isDie && !ai[i].isDie)
		{
			eat(&gamer, &ai[i]);
		}
	}

	// 判断AI球之间的吃掉行为
	for (int i = 0; i < AI_NUM; i++)
	{
		for (int j = i + 1; j < AI_NUM; j++)
		{
			if (!ai[i].isDie && !ai[j].isDie)
			{
				eat(&ai[i], &ai[j]);
				eat(&ai[j], &ai[i]);
			}
		}
	}

	// 判断AI球与食物球之间的吃掉行为
	for (int i = 0; i < AI_NUM; i++)
	{
		for (int j = 0; j < FOOD_NUM; j++)
		{
			if (!ai[i].isDie && !foods[j].isDie)
			{
				eat(&ai[i], &foods[j]);
			}
		}
	}
}


// 追击算法，让AI球 chase 追击 target 球
void chase(Sprite* chase, Sprite* target, double stepSize)
{
	double dx = target->x - chase->x;
	double dy = target->y - chase->y;
	double distance = sqrt(dx * dx + dy * dy);

	// 球的移动方向向量
	double moveX = dx / distance;
	double moveY = dy / distance;

	// 计算下一个位置
	double nextX = chase->x + moveX * stepSize;
	double nextY = chase->y + moveY * stepSize;

	// 移动球
	sprite_moveBy(chase, nextX - chase->x, nextY - chase->y);
}

// AI移动，使AI球去追击食物球或比它小的最近的AI球
void aiMove()
{
	for (int i = 0; i < AI_NUM; i++)
	{
		if (!ai[i].isDie)
		{
			// 选择最近的食物球作为追击目标
			int closestFoodIndex = -1;
			double closestFoodDistance = INFINITY;

			for (int j = 0; j < FOOD_NUM; j++)
			{
				if (!foods[j].isDie)
				{
					double distance = sprite_getDistance(&ai[i], &foods[j]);
					if (distance < closestFoodDistance && ai[i].r > foods[j].r)
					{
						closestFoodDistance = distance;
						closestFoodIndex = j;
					}
				}
			}

			// 选择比当前AI球小且最近的AI球作为追击目标
			int closestAiIndex = -1;
			double closestAiDistance = INFINITY;

			for (int j = 0; j < AI_NUM; j++)
			{
				if (i != j && !ai[j].isDie && ai[i].r > ai[j].r)
				{
					double distance = sprite_getDistance(&ai[i], &ai[j]);
					if (distance < closestAiDistance)
					{
						closestAiDistance = distance;
						closestAiIndex = j;
					}
				}
			}

			if (closestFoodIndex != -1)
			{
				// 选择食物球作为追击目标
				chase(&ai[i], &foods[closestFoodIndex], 0.1);
			}
			else if (closestAiIndex != -1)
			{
				// 选择比当前AI球小且最近的AI球作为追击目标
				chase(&ai[i], &ai[closestAiIndex], 0.1);
			}
			else
			{
				// 没有合适的食物球或比它小的AI球，改为追击玩家球
				chase(&ai[i], &gamer, 0.1);
			}
		}
	}
}


int main()
{
	//创建图像窗口
	initgraph(1024, 640);
	//设置窗口背景颜色
	setbkcolor(WHITE);
	//用设置的背景颜色填充屏幕
	cleardevice();
	init();
	while (true)
	{
		//开始双缓冲绘图
		BeginBatchDraw();
		cleardevice();//绘制之前清屏

		draw();
		keyDownDeal();
		gamerEatFood();
		gamerAndAiEat();
		aiMove();
		//结束双缓冲绘图
		EndBatchDraw();
	}

	getchar();
	return 0;
}