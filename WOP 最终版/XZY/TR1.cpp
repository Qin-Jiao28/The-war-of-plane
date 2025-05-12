#include <graphics.h>
#include <conio.h>
#include <list>
#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
// 游戏窗口尺寸
#define WIDTH 480
#define HEIGHT 700

// 资源路径常量
const wchar_t PLAYER_IMG_PATH[] = L"sc/player1.png";
const wchar_t ENEMY_IMG_PATH[] = L"sc/enemy1.png";//用宽字符型常量数组存储图片路径
const wchar_t BG_IMG_PATH[] = L"sc/background1.png";

// 玩家飞机结构体
struct Plane {
    int x, y;
    int speed;
    bool alive;
    IMAGE img;

    int width() const { return img.getwidth(); }//用成员函数获取图片数据，更为方便
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

 //初始化游戏
void GameInit() {
    // 创建图形窗口
    initgraph(WIDTH, HEIGHT, EW_SHOWCONSOLE);
    //利用双缓冲解决闪烁
    BeginBatchDraw();
    // 加载游戏资源
    loadimage(&bkimg, BG_IMG_PATH);
    //loadimage(&player.img, PLAYER_IMG_PATH);

    IMAGE player_temp;
    loadimage(&player_temp, PLAYER_IMG_PATH);
    player.img.Resize(player_temp.getwidth(), player_temp.getheight());
    GetImageBuffer(&player.img); // 初始化缓冲区
    memcpy(GetImageBuffer(&player.img), GetImageBuffer(&player_temp),
        player_temp.getwidth() * player_temp.getheight() * sizeof(DWORD));
    loadimage(&enemyImg, ENEMY_IMG_PATH);

    // 初始化玩家
    player.x = (WIDTH - player.width()) / 2;
    player.y = HEIGHT - 100;
    player.speed = 5;
    player.alive = true;
}

// 带透明通道绘制函数
void DrawImageWithAlpha(int x, int y, IMAGE* img) {
    DWORD* dst = GetImageBuffer();            // 绘图设备内存
    DWORD* src = GetImageBuffer(img);         // 图片源数据
    int imgWidth = img->getwidth();
    int imgHeight = img->getheight();

    // 逐像素混合
    for (int iy = 0; iy < imgHeight; iy++) {
        for (int ix = 0; ix < imgWidth; ix++) {
            int pos = iy * imgWidth + ix;
            BYTE a = (src[pos] >> 24) & 0xff; // 获取Alpha通道
            if (a != 0) {                     // 忽略完全透明像素
                int dx = x + ix;
                int dy = y + iy;
                if (dx >= 0 && dx < WIDTH && dy >= 0 && dy < HEIGHT) {
                    // Alpha混合计算
                    BYTE sr = GetRValue(src[pos]);
                    BYTE sg = GetGValue(src[pos]);
                    BYTE sb = GetBValue(src[pos]);
                    BYTE dr = GetRValue(dst[dy * WIDTH + dx]);
                    BYTE dg = GetGValue(dst[dy * WIDTH + dx]);
                    BYTE db = GetBValue(dst[dy * WIDTH + dx]);

                    dst[dy * WIDTH + dx] = RGB(
                        (sr * a + dr * (255 - a)) / 255,
                        (sg * a + dg * (255 - a)) / 255,
                        (sb * a + db * (255 - a)) / 255
                    );
                }
            }
        }
    }
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
                enemy.x, enemy.y,                 //敌机尺寸
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
    cleardevice();
    // 绘制背景
    putimage(0, 0, &bkimg);

    // 绘制玩家飞机
    if (player.alive) {
        DrawImageWithAlpha(player.x, player.y, &player.img);
    }

    // 绘制子弹（黄色圆形）
    setfillcolor(YELLOW);
    for (auto& bullet : bullets) {
        solidcircle(bullet.x, bullet.y, 3);
    }

    // 绘制敌机
    for (auto& enemy : enemies) {
        DrawImageWithAlpha(enemy.x, enemy.y, enemy.img);
    }

    // 绘制得分
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);
    FlushBatchDraw();
    // 控制帧率
    Sleep(16);
}

// 全局控制变量
bool gameRestart = false;

int main() {
    mciSendString(L"open sc\\bgm.mp3 alias bgm", NULL, 0, NULL);
    mciSendString(L"play bgm repeat", NULL, 0, NULL); // 循环播放
    mciSendString(L"setaudio bgm volume to 100", NULL, 0, NULL);
    do {
        // 初始化游戏
        GameInit();
        gameRestart = false;  // 重置重启标记
        score = 0;           // 重置得分

        // 游戏主循环
        while (player.alive) {
            PlayerControl();
            CreateEnemy();
            GameRun();
            GameDraw();
        }

        // 游戏结束处理
        if (!player.alive) {
            // 定义界面元素参数
            const int btnWidth = 160;
            const int btnHeight = 50;
            const int btnSpacing = 20;

            // 按钮区域定义
            const RECT restartBtn = {
                WIDTH / 2 - btnWidth - btnSpacing / 2,
                HEIGHT / 2 + 40,
                WIDTH / 2 - btnSpacing / 2,
                HEIGHT / 2 + 40 + btnHeight
            };
            const RECT exitBtn = {
                WIDTH / 2 + btnSpacing / 2,
                HEIGHT / 2 + 40,
                WIDTH / 2 + btnWidth + btnSpacing / 2,
                HEIGHT / 2 + 40 + btnHeight
            };

            // 结束画面循环
            bool exitMenu = false;
            MOUSEMSG mouseMsg;
            while (!exitMenu) {
                // 绘制背景
                cleardevice();
                putimage(0, 0, &bkimg);

                // 绘制半透明面板
                setfillcolor(RGB(180, 30, 30, 30));
                solidroundrect(WIDTH / 2 - 200, HEIGHT / 2 - 100,
                    WIDTH / 2 + 200, HEIGHT / 2 + 150, 20, 20);

                setbkmode(TRANSPARENT);

                // 绘制标题
                settextcolor(HSVtoRGB(0, 0.8f, 1)); // 红色
                settextstyle(48, 0, L"微软雅黑");
                outtextxy(WIDTH / 2 - textwidth(L"游戏结束") / 2, HEIGHT / 2 - 80, L"游戏结束");

                // 绘制得分
                settextcolor(WHITE);
                settextstyle(28, 0, L"Consolas");
                wchar_t scoreStr[32];
                swprintf_s(scoreStr, L"得分: %d", score);
                outtextxy(WIDTH / 2 - textwidth(scoreStr) / 2, HEIGHT / 2 - 30, scoreStr);

                // 检测鼠标状态
                bool restartHover = false;
                bool exitHover = false;
                if (MouseHit()) {
                    mouseMsg = GetMouseMsg();

                    // 检测鼠标位置
                    restartHover = (mouseMsg.x >= restartBtn.left &&
                        mouseMsg.x <= restartBtn.right &&
                        mouseMsg.y >= restartBtn.top &&
                        mouseMsg.y <= restartBtn.bottom);

                    exitHover = (mouseMsg.x >= exitBtn.left &&
                        mouseMsg.x <= exitBtn.right &&
                        mouseMsg.y >= exitBtn.top &&
                        mouseMsg.y <= exitBtn.bottom);

                    // 处理点击事件
                    if (mouseMsg.uMsg == WM_LBUTTONDOWN) {
                        if (restartHover) {
                            gameRestart = true;
                            exitMenu = true;
                        }
                        else if (exitHover) {
                            gameRestart = false;
                            exitMenu = true;
                        }
                    }
                }

                // 绘制按钮函数
                auto DrawButton = [](RECT rect, LPCWSTR text, bool hover) {
                    // 按钮背景
                    setfillcolor(hover ? RGB(80, 80, 80) : RGB(60, 60, 60));
                    solidroundrect(rect.left, rect.top, rect.right, rect.bottom, 8, 8);

                    // 按钮边框
                    setlinecolor(hover ? RGB(150, 150, 150) : RGB(100, 100, 100));
                    roundrect(rect.left, rect.top, rect.right, rect.bottom, 8, 8);

                    // 按钮文字
                    settextcolor(WHITE);
                    settextstyle(28, 0, L"微软雅黑");
                    int textW = textwidth(text);
                    int textH = textheight(text);
                    outtextxy((rect.left + rect.right - textW) / 2,
                        (rect.top + rect.bottom - textH) / 2 - 2,
                        text);
                    };

                // 绘制两个按钮
                DrawButton(restartBtn, L"重新开始", restartHover);
                DrawButton(exitBtn, L"退出游戏", exitHover);

                // 刷新画面
                FlushBatchDraw();
                Sleep(10);
            }

            // 清空游戏对象
            bullets.clear();
            enemies.clear();
        }
    } while (gameRestart);

    // 关闭图形窗口
    closegraph();
    EndBatchDraw();
    mciSendString(L"close bgm", NULL, 0, NULL);
    return 0;
}