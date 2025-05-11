#include <graphics.h>
#include <conio.h>
#include <list>
#include <stdio.h>

// ��Ϸ���ڳߴ�
#define WIDTH 480
#define HEIGHT 700

// ��Դ·������
const wchar_t PLAYER_IMG_PATH[] = L"sc/player1.png";
const wchar_t ENEMY_IMG_PATH[] = L"sc/enemy1.png";
const wchar_t BG_IMG_PATH[] = L"sc/background1.png";

// ��ҷɻ��ṹ��
struct Plane {
    int x, y;
    int speed;
    bool alive;
    IMAGE img;

    int width() const { return img.getwidth(); }
    int height() const { return img.getheight(); }
};

// �ӵ��ṹ��
struct Bullet {
    int x, y;
    int speed;
    bool alive;
};

// �л��ṹ��
struct Enemy {
    int x, y;
    int speed;
    bool alive;
    IMAGE* img;  // ʹ�ù���ͼƬ��Դ

    int width() const { return img->getwidth(); }
    int height() const { return img->getheight(); }
};

// ȫ����Ϸ����
Plane player;
std::list<Bullet> bullets;
std::list<Enemy> enemies;
int score = 0;

// ȫ��ͼƬ��Դ
IMAGE bkimg;     // ����ͼƬ
IMAGE enemyImg;  // �л�����ͼƬ

// ��ʼ����Ϸ
void GameInit() {
    // ����ͼ�δ���
    initgraph(WIDTH, HEIGHT);

    // ������Ϸ��Դ
    loadimage(&bkimg, BG_IMG_PATH);
    loadimage(&player.img, PLAYER_IMG_PATH);
    loadimage(&enemyImg, ENEMY_IMG_PATH);

    // ��ʼ�����
    player.x = (WIDTH - player.width()) / 2;
    player.y = HEIGHT - 100;
    player.speed = 5;
    player.alive = true;
}

// ��ҿ���
void PlayerControl() {
    // �����ƶ�
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

    // ������ƣ��ո����
    static int shootInterval = 0;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (shootInterval == 0) {
            // �ӷɻ����ķ����ӵ�
            bullets.push_back({
                player.x + player.width() / 2 - 3,  // ����
                player.y - 10,
                8,
                true
                });
            shootInterval = 10;
        }
    }
    if (shootInterval > 0) shootInterval--;
}

// ���ɵл�
void CreateEnemy() {
    static int interval = 0;
    if (interval-- <= 0) {
        enemies.push_back({
            rand() % (WIDTH - enemyImg.getwidth()),  // ���ˮƽλ��
            -enemyImg.getheight(),                   // ����Ļ�Ϸ�����
            3,                                       // �����ٶ�
            true,
            &enemyImg                                // ����ͼƬ��Դ
            });
        interval = 50;  // ���ɼ��
    }
}

// ��ײ���
bool CheckCollision(int x1, int y1, int w1, int h1,
    int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2;
}

// ��Ϸ�߼�����
void GameRun() {
    // �����ӵ�״̬
    for (auto& bullet : bullets) {
        bullet.y -= bullet.speed;
        if (bullet.y < 0) bullet.alive = false;

        // �ӵ���л���ײ���
        for (auto& enemy : enemies) {
            if (CheckCollision(
                bullet.x, bullet.y, 6, 6,          // �ӵ��ߴ�
                enemy.x, enemy.y,
                enemy.width(), enemy.height()
            )) {
                bullet.alive = false;
                enemy.alive = false;
                score += 10;
            }
        }
    }

    // ���µл�״̬
    for (auto& enemy : enemies) {
        enemy.y += enemy.speed;
        if (enemy.y > HEIGHT) enemy.alive = false;

        // �����л���ײ���
        if (CheckCollision(
            player.x, player.y,
            player.width(), player.height(),
            enemy.x, enemy.y,
            enemy.width(), enemy.height()
        )) {
            player.alive = false;
        }
    }

    // ������Ч����
    bullets.remove_if([](auto& b) { return !b.alive; });
    enemies.remove_if([](auto& e) { return !e.alive; });
}

// ��Ϸ�������
void GameDraw() {
    // ���Ʊ���
    putimage(0, 0, &bkimg);

    // ������ҷɻ�
    if (player.alive) {
        putimage(player.x, player.y, &player.img);
    }

    // �����ӵ�����ɫԲ�Σ�
    setfillcolor(YELLOW);
    for (auto& bullet : bullets) {
        solidcircle(bullet.x, bullet.y, 3);
    }

    // ���Ƶл�
    for (auto& enemy : enemies) {
        putimage(enemy.x, enemy.y, enemy.img);
    }

    // ���Ƶ÷�
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);

    // ����֡��
    Sleep(16);
}

int main() {
    GameInit();

    // ��Ϸ��ѭ��
    while (player.alive) {
        // ESC���˳����
        if (_kbhit()) {
            if (_getch() == 27) break;
        }

        PlayerControl();
        CreateEnemy();
        GameRun();
        GameDraw();
    }

    // ��Ϸ��������
    if (!player.alive) {
        cleardevice();
        putimage(0, 0, &bkimg);
        settextstyle(40, 0, L"΢���ź�");
        settextcolor(RED);
        outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, L"��Ϸ������");
        Sleep(2000);
    }

    closegraph();
    return 0;
}