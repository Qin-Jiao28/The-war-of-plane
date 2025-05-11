#include <graphics.h>
#include <conio.h>
#include <list>
#include <stdio.h>

// 游戏窗口尺寸
#define WIDTH 480
#define HEIGHT 700

// 资源路径常量
const wchar_t PLAYER_IMG_PATH[] = L"sc/player1.png";
const wchar_t ENEMY_IMG_PATH[] = L"sc/enemy1.png";
const wchar_t BG_IMG_PATH[] = L"sc/background1.png";

// 玩家飞机结构体
struct Plane {
    int x, y;
    int speed;
    bool alive;
    IMAGE img;

    int width() const { return img.getwidth(); }
    int height() const { return img.getheight(); }
};

// 子弹结构体
struct Bullet {
    int x, y;
    int speed;
    bool alive;
};

// 敌机结构体
struct Enemy {
    int x, y;
    int speed;
    bool alive;
    IMAGE* img;  // 使用共享图片资源

    int width() const { return img->getwidth(); }
    int height() const { return img->getheight(); }
};

// 全局游戏对象
Plane player;
std::list<Bullet> bullets;
std::list<Enemy> enemies;
int score = 0;

// 全局图片资源
IMAGE bkimg;     // 背景图片
IMAGE enemyImg;  // 敌机共享图片

// 初始化游戏
void GameInit() {
    // 创建图形窗口
    initgraph(WIDTH, HEIGHT);

    // 加载游戏资源
    loadimage(&bkimg, BG_IMG_PATH);
    loadimage(&player.img, PLAYER_IMG_PATH);
    loadimage(&enemyImg, ENEMY_IMG_PATH);

    // 初始化玩家
    player.x = (WIDTH - player.width()) / 2;
    player.y = HEIGHT - 100;
    player.speed = 5;
    player.alive = true;
}

// 玩家控制
void PlayerControl() {
    // 左右移动
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        player.x -= player.speed;
        if (player.x < 0) player.x = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        player.x += player.speed;
        if (player.x > WIDTH - player.width()) {
            player.x = WIDTH - player.width();
        }
    }

    // 射击控制（空格键）
    static int shootInterval = 0;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (shootInterval == 0) {
            // 从飞机中心发射子弹
            bullets.push_back({
                player.x + player.width() / 2 - 3,  // 居中
                player.y - 10,
                8,
                true
                });
            shootInterval = 10;
        }
    }
    if (shootInterval > 0) shootInterval--;
}

// 生成敌机
void CreateEnemy() {
    static int interval = 0;
    if (interval-- <= 0) {
        enemies.push_back({
            rand() % (WIDTH - enemyImg.getwidth()),  // 随机水平位置
            -enemyImg.getheight(),                   // 从屏幕上方出现
            3,                                       // 下落速度
            true,
            &enemyImg                                // 共享图片资源
            });
        interval = 50;  // 生成间隔
    }
}

// 碰撞检测
bool CheckCollision(int x1, int y1, int w1, int h1,
    int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2;
}

// 游戏逻辑更新
void GameRun() {
    // 更新子弹状态
    for (auto& bullet : bullets) {
        bullet.y -= bullet.speed;
        if (bullet.y < 0) bullet.alive = false;

        // 子弹与敌机碰撞检测
        for (auto& enemy : enemies) {
            if (CheckCollision(
                bullet.x, bullet.y, 6, 6,          // 子弹尺寸
                enemy.x, enemy.y,
                enemy.width(), enemy.height()
            )) {
                bullet.alive = false;
                enemy.alive = false;
                score += 10;
            }
        }
    }

    // 更新敌机状态
    for (auto& enemy : enemies) {
        enemy.y += enemy.speed;
        if (enemy.y > HEIGHT) enemy.alive = false;

        // 玩家与敌机碰撞检测
        if (CheckCollision(
            player.x, player.y,
            player.width(), player.height(),
            enemy.x, enemy.y,
            enemy.width(), enemy.height()
        )) {
            player.alive = false;
        }
    }

    // 清理无效对象
    bullets.remove_if([](auto& b) { return !b.alive; });
    enemies.remove_if([](auto& e) { return !e.alive; });
}

// 游戏画面绘制
void GameDraw() {
    // 绘制背景
    putimage(0, 0, &bkimg);

    // 绘制玩家飞机
    if (player.alive) {
        putimage(player.x, player.y, &player.img);
    }

    // 绘制子弹（黄色圆形）
    setfillcolor(YELLOW);
    for (auto& bullet : bullets) {
        solidcircle(bullet.x, bullet.y, 3);
    }

    // 绘制敌机
    for (auto& enemy : enemies) {
        putimage(enemy.x, enemy.y, enemy.img);
    }

    // 绘制得分
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);

    // 控制帧率
    Sleep(16);
}

int main() {
    GameInit();

    // 游戏主循环
    while (player.alive) {
        // ESC键退出检测
        if (_kbhit()) {
            if (_getch() == 27) break;
        }

        PlayerControl();
        CreateEnemy();
        GameRun();
        GameDraw();
    }

    // 游戏结束处理
    if (!player.alive) {
        cleardevice();
        putimage(0, 0, &bkimg);
        settextstyle(40, 0, L"微软雅黑");
        settextcolor(RED);
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, L"游戏结束！");
        Sleep(2000);
    }

    closegraph();
    return 0;
}