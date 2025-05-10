#include <graphics.h>      
#include <conio.h>     //����̨����ͷ�ļ�    
#include <list>
#include<stdio.h>
#include<algorithm>

// ������Ϸ���ڳߴ�
#define WIDTH 480
#define HEIGHT 700

// ������ҷɻ��ṹ��
struct Plane 
{
    int x, y;            // ����
    int speed;           // �ƶ��ٶ�
    bool alive;          // �Ƿ���
    IMAGE img;           // �ɻ�ͼƬ
};

// �����ӵ��ṹ��
struct Bullet 
{
    int x, y;
    int speed;
    bool alive;
};

// ����л��ṹ��
struct Enemy 
{
    int x, y;
    int speed;
    bool alive;
    IMAGE img;
};

Plane player;                   // ��ҷɻ�
std::list<Bullet> bullets;      // �ӵ�����
std::list<Enemy> enemies;       // �л�����
int score = 0;                  // ��Ϸ�÷�

// ��ʼ����Ϸ
void GameInit() 
{
    initgraph(WIDTH, HEIGHT);   // ����ͼ�δ���
    IMAGE bkimg;//����ͼƬ
    loadimage(&bkimg, L"sc/background1.png");
    loadimage(&player.img, L"sc/player1.png");  // �������ͼƬ
    player.x = WIDTH / 2 - 33;  // ��ʼλ�þ���
    player.y = HEIGHT - 100;
    player.speed = 5;//�����ٶ�
    player.alive = true;
    cleardevice();
    putimage(0, 0, &bkimg);
}

//���ÿ��Ʒ�ʽ
void PlayerControl() {
    // �����ƶ�
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        player.x -= player.speed;
        if (player.x < 0) player.x = 0;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        player.x += player.speed;
        if (player.x > WIDTH - 66) player.x = WIDTH - 66;
    }

    // �ո�����
    static int shootInterval = 0;
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (shootInterval == 0) {
            bullets.push_back({ player.x + 20, player.y - 20, 8, true });
            shootInterval = 10; // ������
        }
    }
    if (shootInterval > 0) shootInterval--;
}

// ���ɵл�
void CreateEnemy() {
    static int createInterval = 0;
    if (createInterval == 0) {
        IMAGE img;
        loadimage(&img, L"sc/enemy1.png");
        enemies.push_back({
            rand() % (WIDTH - 50),                           //�л��ĳ������ݺ��������
            -50,
            3,
            true,
            img
            });
        createInterval = 50; // �л����ɼ��
    }
    createInterval--;
}

// ��ײ���
bool CheckCollision(int x1, int y1, int w1, int h1,
                    int x2, int y2, int w2, int h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&   //�Ƿ���ˮƽ����ֱ�����϶����ص�������ײ��
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

void GameRun() {
    // �����ӵ�
    for (auto& bullet : bullets) {
        bullet.y -= bullet.speed;
        if (bullet.y < 0) bullet.alive = false;

        // �ӵ���л���ײ���
        for (auto& enemy : enemies) {
            if (CheckCollision(bullet.x, bullet.y, 5, 10,
                enemy.x, enemy.y, 50, 50)) {                 //�л��ĳ������ݺ��������
                bullet.alive = false;
                enemy.alive = false;
                score += 10;
            }
        }
    }

    // ����л�
    for (auto& enemy : enemies) {
        enemy.y += enemy.speed;
        if (enemy.y > HEIGHT) enemy.alive = false;

        // �����л���ײ���
        if (CheckCollision(player.x, player.y, 50, 50,
            enemy.x, enemy.y, 50, 50)) {
            player.alive = false;
        }
    }

    // �Ƴ���Ч����
    bullets.remove_if([](Bullet b) { return !b.alive; });
    enemies.remove_if([](Enemy e) { return !e.alive; });
}

// ��Ϸ����
void GameDraw() {
    //cleardevice();  // ��ջ���

    // �������
    if (player.alive) {
        putimage(player.x, player.y, &player.img);
    }

    // �����ӵ�
    for (auto& bullet : bullets) {
        setfillcolor(YELLOW);
        solidcircle(bullet.x, bullet.y, 3);
    }

    // ���Ƶл�
    for (auto& enemy : enemies) {
        putimage(enemy.x, enemy.y, &enemy.img);
    }

    // ���Ƶ÷�
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);

    Sleep(16);  // ����֡��
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

     // �����Ϸ������ʾ
     if (!player.alive) {
         cleardevice();
         settextstyle(40, 0, L"΢���ź�");
         settextcolor(RED);
         outtextxy(WIDTH / 2 - 100, HEIGHT / 2 - 20, L"��Ϸ������");
         Sleep(2000);
         _getch();
     }
         closegraph();  // �ر�ͼ�δ���
         return 0;
     }