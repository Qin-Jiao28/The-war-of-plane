#include <graphics.h>
#include <conio.h>
#include <list>
#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
// ��Ϸ���ڳߴ�
#define WIDTH 480
#define HEIGHT 700

// ��Դ·������
const wchar_t PLAYER_IMG_PATH[] = L"sc/player1.png";
const wchar_t ENEMY_IMG_PATH[] = L"sc/enemy1.png";//�ÿ��ַ��ͳ�������洢ͼƬ·��
const wchar_t BG_IMG_PATH[] = L"sc/background1.png";

// ��ҷɻ��ṹ��
struct Plane {
    int x, y;
    int speed;
    bool alive;
    IMAGE img;

    int width() const { return img.getwidth(); }//�ó�Ա������ȡͼƬ���ݣ���Ϊ����
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

 //��ʼ����Ϸ
void GameInit() {
    // ����ͼ�δ���
    initgraph(WIDTH, HEIGHT, EW_SHOWCONSOLE);
    //����˫��������˸
    BeginBatchDraw();
    // ������Ϸ��Դ
    loadimage(&bkimg, BG_IMG_PATH);
    //loadimage(&player.img, PLAYER_IMG_PATH);

    IMAGE player_temp;
    loadimage(&player_temp, PLAYER_IMG_PATH);
    player.img.Resize(player_temp.getwidth(), player_temp.getheight());
    GetImageBuffer(&player.img); // ��ʼ��������
    memcpy(GetImageBuffer(&player.img), GetImageBuffer(&player_temp),
        player_temp.getwidth() * player_temp.getheight() * sizeof(DWORD));
    loadimage(&enemyImg, ENEMY_IMG_PATH);

    // ��ʼ�����
    player.x = (WIDTH - player.width()) / 2;
    player.y = HEIGHT - 100;
    player.speed = 5;
    player.alive = true;
}

// ��͸��ͨ�����ƺ���
void DrawImageWithAlpha(int x, int y, IMAGE* img) {
    DWORD* dst = GetImageBuffer();            // ��ͼ�豸�ڴ�
    DWORD* src = GetImageBuffer(img);         // ͼƬԴ����
    int imgWidth = img->getwidth();
    int imgHeight = img->getheight();

    // �����ػ��
    for (int iy = 0; iy < imgHeight; iy++) {
        for (int ix = 0; ix < imgWidth; ix++) {
            int pos = iy * imgWidth + ix;
            BYTE a = (src[pos] >> 24) & 0xff; // ��ȡAlphaͨ��
            if (a != 0) {                     // ������ȫ͸������
                int dx = x + ix;
                int dy = y + iy;
                if (dx >= 0 && dx < WIDTH && dy >= 0 && dy < HEIGHT) {
                    // Alpha��ϼ���
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
                enemy.x, enemy.y,                 //�л��ߴ�
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
    cleardevice();
    // ���Ʊ���
    putimage(0, 0, &bkimg);

    // ������ҷɻ�
    if (player.alive) {
        DrawImageWithAlpha(player.x, player.y, &player.img);
    }

    // �����ӵ�����ɫԲ�Σ�
    setfillcolor(YELLOW);
    for (auto& bullet : bullets) {
        solidcircle(bullet.x, bullet.y, 3);
    }

    // ���Ƶл�
    for (auto& enemy : enemies) {
        DrawImageWithAlpha(enemy.x, enemy.y, enemy.img);
    }

    // ���Ƶ÷�
    setbkmode(TRANSPARENT);
    settextcolor(WHITE);
    settextstyle(20, 0, L"Consolas");
    wchar_t str[32];
    swprintf_s(str, L"Score: %d", score);
    outtextxy(10, 10, str);
    FlushBatchDraw();
    // ����֡��
    Sleep(16);
}

// ȫ�ֿ��Ʊ���
bool gameRestart = false;

int main() {
    mciSendString(L"open sc\\bgm.mp3 alias bgm", NULL, 0, NULL);
    mciSendString(L"play bgm repeat", NULL, 0, NULL); // ѭ������
    mciSendString(L"setaudio bgm volume to 100", NULL, 0, NULL);
    do {
        // ��ʼ����Ϸ
        GameInit();
        gameRestart = false;  // �����������
        score = 0;           // ���õ÷�

        // ��Ϸ��ѭ��
        while (player.alive) {
            PlayerControl();
            CreateEnemy();
            GameRun();
            GameDraw();
        }

        // ��Ϸ��������
        if (!player.alive) {
            // �������Ԫ�ز���
            const int btnWidth = 160;
            const int btnHeight = 50;
            const int btnSpacing = 20;

            // ��ť������
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

            // ��������ѭ��
            bool exitMenu = false;
            MOUSEMSG mouseMsg;
            while (!exitMenu) {
                // ���Ʊ���
                cleardevice();
                putimage(0, 0, &bkimg);

                // ���ư�͸�����
                setfillcolor(RGB(180, 30, 30, 30));
                solidroundrect(WIDTH / 2 - 200, HEIGHT / 2 - 100,
                    WIDTH / 2 + 200, HEIGHT / 2 + 150, 20, 20);

                setbkmode(TRANSPARENT);

                // ���Ʊ���
                settextcolor(HSVtoRGB(0, 0.8f, 1)); // ��ɫ
                settextstyle(48, 0, L"΢���ź�");
                outtextxy(WIDTH / 2 - textwidth(L"��Ϸ����") / 2, HEIGHT / 2 - 80, L"��Ϸ����");

                // ���Ƶ÷�
                settextcolor(WHITE);
                settextstyle(28, 0, L"Consolas");
                wchar_t scoreStr[32];
                swprintf_s(scoreStr, L"�÷�: %d", score);
                outtextxy(WIDTH / 2 - textwidth(scoreStr) / 2, HEIGHT / 2 - 30, scoreStr);

                // ������״̬
                bool restartHover = false;
                bool exitHover = false;
                if (MouseHit()) {
                    mouseMsg = GetMouseMsg();

                    // ������λ��
                    restartHover = (mouseMsg.x >= restartBtn.left &&
                        mouseMsg.x <= restartBtn.right &&
                        mouseMsg.y >= restartBtn.top &&
                        mouseMsg.y <= restartBtn.bottom);

                    exitHover = (mouseMsg.x >= exitBtn.left &&
                        mouseMsg.x <= exitBtn.right &&
                        mouseMsg.y >= exitBtn.top &&
                        mouseMsg.y <= exitBtn.bottom);

                    // �������¼�
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

                // ���ư�ť����
                auto DrawButton = [](RECT rect, LPCWSTR text, bool hover) {
                    // ��ť����
                    setfillcolor(hover ? RGB(80, 80, 80) : RGB(60, 60, 60));
                    solidroundrect(rect.left, rect.top, rect.right, rect.bottom, 8, 8);

                    // ��ť�߿�
                    setlinecolor(hover ? RGB(150, 150, 150) : RGB(100, 100, 100));
                    roundrect(rect.left, rect.top, rect.right, rect.bottom, 8, 8);

                    // ��ť����
                    settextcolor(WHITE);
                    settextstyle(28, 0, L"΢���ź�");
                    int textW = textwidth(text);
                    int textH = textheight(text);
                    outtextxy((rect.left + rect.right - textW) / 2,
                        (rect.top + rect.bottom - textH) / 2 - 2,
                        text);
                    };

                // ����������ť
                DrawButton(restartBtn, L"���¿�ʼ", restartHover);
                DrawButton(exitBtn, L"�˳���Ϸ", exitHover);

                // ˢ�»���
                FlushBatchDraw();
                Sleep(10);
            }

            // �����Ϸ����
            bullets.clear();
            enemies.clear();
        }
    } while (gameRestart);

    // �ر�ͼ�δ���
    closegraph();
    EndBatchDraw();
    mciSendString(L"close bgm", NULL, 0, NULL);
    return 0;
}