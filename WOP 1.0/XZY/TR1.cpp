#include <graphics.h>      
#include <conio.h>     //控制台输入头文件    
#include <list>
#include<stdio.h>

// 定义游戏窗口尺寸
#define WIDTH 480
#define HEIGHT 700

// 定义玩家飞机结构体
struct Plane 
{
    int x, y;            // 坐标
    int speed;           // 移动速度
    bool alive;          // 是否存活
    IMAGE img;           // 飞机图片
};

// 定义子弹结构体
struct Bullet 
{
    int x, y;
    int speed;
    bool alive;
};

// 定义敌机结构体
struct Enemy 
{
    int x, y;
    int speed;
    bool alive;
    IMAGE img;
};

Plane player;                   // 玩家飞机
std::list<Bullet> bullets;      // 子弹链表
std::list<Enemy> enemies;       // 敌机链表
int score = 0;                  // 游戏得分

// 初始化游戏
void GameInit() 
{
    initgraph(WIDTH, HEIGHT);   // 创建图形窗口
    IMAGE bkimg;//背景图片
    loadimage(&bkimg, L"sc/background1.png");
    loadimage(&player.img, L"sc/player1.png");  // 加载玩家图片
    player.x = WIDTH / 2 - 33;  // 初始位置居中
    player.y = HEIGHT - 100;
    player.speed = 5;//设置速度
    player.alive = true;
    cleardevice();
    putimage(0, 0, &bkimg);
}

//设置控制方式
void PlayerControl() {
    // 左右移动
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        player.x -= player.speed;
        if (player.x < 0) player.x = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        player.x += player.speed;
        if (player.x > WIDTH - 66) player.x = WIDTH - 66;
    }

    // 空格键射击
    static int shootInterval = 0;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (shootInterval == 0) {
            bullets.push_back({ player.x + 20, player.y - 20, 8, true });
            shootInterval = 10; // 射击间隔
        }
    }
    if (shootInterval > 0) shootInterval--;
}
 int main()
 {
     GameInit();
     system("pause");
 }