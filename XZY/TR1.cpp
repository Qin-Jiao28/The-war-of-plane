#include <graphics.h>      
#include <conio.h>     //����̨����ͷ�ļ�    
#include <list>
#include<stdio.h>

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
 int main()
 {
     GameInit();
     system("pause");
 }