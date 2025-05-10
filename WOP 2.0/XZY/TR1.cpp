#include <graphics.h>      
#include <conio.h>     //控制台输入头文件    
#include <list>
#include<stdio.h>
#include<algorithm>

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

// 生成敌机
void CreateEnemy() {
    static int createInterval = 0;
    if (createInterval == 0) {
        IMAGE img;
        loadimage(&img, L"sc/enemy1.png");
        enemies.push_back({
            rand() % (WIDTH - 50),                           //敌机的长宽数据后续需更改
            -50,
            3,
            true,
            img
            });
        createInterval = 50; // 敌机生成间隔
    }
    createInterval--;
}

// 碰撞检测
bool CheckCollision(int x1, int y1, int w1, int h1,
                    int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&   //是否在水平和竖直方向上都有重叠（即碰撞）
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

void GameRun() {
    // 处理子弹
    for (auto& bullet : bullets) {
        bullet.y -= bullet.speed;
        if (bullet.y < 0) bullet.alive = false;

        // 子弹与敌机碰撞检测
        for (auto& enemy : enemies) {
            if (CheckCollision(bullet.x, bullet.y, 5, 10,
                enemy.x, enemy.y, 50, 50)) {                 //敌机的长宽数据后续需更改
                bullet.alive = false;
                enemy.alive = false;
                score += 10;
            }
        }
    }

    // 处理敌机
    for (auto& enemy : enemies) {
        enemy.y += enemy.speed;
        if (enemy.y > HEIGHT) enemy.alive = false;

        // 玩家与敌机碰撞检测
        if (CheckCollision(player.x, player.y, 50, 50,
            enemy.x, enemy.y, 50, 50)) {
            player.alive = false;
        }
    }

    // 移除无效对象
    bullets.remove_if([](Bullet b) { return !b.alive; });
    enemies.remove_if([](Enemy e) { return !e.alive; });
}

// 游戏绘制
void GameDraw() {
    //cleardevice();  // 清空画面

    // 绘制玩家
    if (player.alive) {
        putimage(player.x, player.y, &player.img);
    }

    // 绘制子弹
    for (auto& bullet : bullets) {
        setfillcolor(YELLOW);
        solidcircle(bullet.x, bullet.y, 3);
    }

    // 绘制敌机
    for (auto& enemy : enemies) {
        putimage(enemy.x, enemy.y, &enemy.img);
    }

    // 绘制得分
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);

    Sleep(16);  // 控制帧率
}
 int main()
 {
     GameInit();
     while (player.alive) {
         PlayerControl();
         CreateEnemy();
         GameRun();
         GameDraw();
     }

     // 添加游戏结束提示
     if (!player.alive) {
         cleardevice();
         settextstyle(40, 0, L"微软雅黑");
         settextcolor(RED);
         outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, L"游戏结束！");
         Sleep(2000);
         _getch();
     }
         closegraph();  // 关闭图形窗口
         return 0;
     }