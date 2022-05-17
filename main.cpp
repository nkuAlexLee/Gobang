#include <graphics.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <algorithm>
#include <MMsystem.h>
#pragma comment(lib, "WINMM.LIB")
#include "AI.h"


#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#pragma comment (lib,"winmm.lib")
#pragma comment (lib, "user32.lib")
#pragma comment (lib,"gdi32.lib")
#pragma comment (lib,"shell32.lib")
#pragma comment (lib,"ole32.lib")
using namespace std;


// 颜色常量
const int WINDOW_WIDTH = 860;
const int WINDOW_HEIGHT = 660;
const int LightSteelBlue = RGB(176, 196, 222);
const int MediumAquamarine = RGB(102, 205, 170);
// 错误码
const int G_POS_PLACED = -4;
const int G_RANGE_ERR = -3;
const int G_STAT_ERR = -2;
const int G_ERR = -1;
const int G_OK = 0;
const int G_FINISH = 1;
const int G_WIN = 2;
const int G_AI = 1;
const int G_HUMAN = 0;
const int G_AI_AI = 2;
const int MOUSEBUTTONUP = 1;
const int KEYBOARDBUTTONUP = 2;
const int sound_background = 0;
const int sound_white = 1;
const int sound_black = 2;
const int sound_error = 3;
const int sound_start = 4;
int turn = 1;
int model = 0;
int level = 15;
int grade = 10;
int MAX = 1008611;
int chesspad[15][15] = { 0 };
// done为1,AI不行动，为-1,AI行动
int done = 1;
int depth = 6;
int map_size = 15;

/*LPWSTR ctl(char* szStr) {
    WCHAR wszClassName[256];
    memset(wszClassName, 0, sizeof(wszClassName));
    MultiByteToWideChar(CP_ACP, 0, szStr, strlen(szStr) + 1, wszClassName, sizeof(wszClassName) / sizeof(wszClassName[0]));
    return wszClassName;
};*/

struct MoveStack {
    int x = 0;
    int y = 0;
    int type = 0;
};
void soundPlay(int type) {
    // // 加载音效文件
    // mciSendString(TEXT("open res\\black.wav alias sound_black"), NULL, 0,NULL);
    // mciSendString(TEXT("open res\\white.wav alias sound_white"), NULL, 0,NULL);
    // //PlaySound(TEXT("F:\\c.wav"),NULL,SND_FILENAME | SND_ASYNC);
    // try{
    //     int music_win =rand()%9+1;
    //     char mid[30];
    //     sprintf(mid,"open res\\%d.wav alias sound_background",music_win);
    //     mciSendString(TEXT(mid),NULL, 0,NULL);
    // }
    // catch (...){
    //     mciSendString(TEXT("open res\\1.wav alias sound_background"), NULL, 0,NULL);
    // };
    // mciSendString(TEXT("open res\\error.wav alias sound_error"), NULL, 0,NULL);
    // mciSendString(TEXT("open res\\start.wav alias sound_start"), NULL, 0,NULL);
    if (type == sound_black)
    {
        mciSendString(TEXT("close sound_black"), NULL, 0, NULL);
        mciSendString(TEXT("open res\\black.wav alias sound_black"), NULL, 0, NULL);
        mciSendString(TEXT("play sound_black"), NULL, 0, NULL);
    }
    else if (type == sound_white)
    {
        mciSendString(TEXT("close sound_white"), NULL, 0, NULL);
        mciSendString(TEXT("open res\\white.wav alias sound_white"), NULL, 0, NULL);
        mciSendString(TEXT("play sound_white"), NULL, 0, NULL);
    }
    else if (type == sound_background)
    {
        //mciSendString(TEXT("close sound_background"), NULL, 0,NULL);
        mciSendString(TEXT("open res\\1.mp3 alias sound_background"), NULL, 0, NULL);
        // try{
        //     int music_win =rand()%9+1;
        //     char mid[30];
        //     sprintf(mid,"open res\\%d.wav alias sound_background",music_win);
        //     mciSendString(TEXT(mid),NULL, 0,NULL);
        // }
        // catch (...){
        //     mciSendString(TEXT("open res\\1.wav alias sound_background"), NULL, 0,NULL);
        // };
        mciSendString(TEXT("play sound_background repeat"), NULL, 0, NULL);

    }
    else if (type == sound_error)
    {
        mciSendString(TEXT("close sound_error"), NULL, 0, NULL);
        mciSendString(TEXT("open res\\error.wav alias sound_error"), NULL, 0, NULL);
        mciSendString(TEXT("play sound_error"), NULL, 0, NULL);
    }
    else if (type == sound_start)
    {
        mciSendString(TEXT("close sound_start"), NULL, 0, NULL);
        mciSendString(TEXT("open res\\start.wav alias sound_start"), NULL, 0, NULL);
        mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    }
};
class Logic
{
public:
    static int map[15][15];
    static int status;
    static int winner;
    static std::vector<MoveStack> move_stack;
    static int map_size;
    Logic() {};
    ~Logic() {};
    static int check_winner();
    static int check();
    static void set_status(int stat);
    static int move(int x, int y);
    static int is_finish(int err_code);
    static void start_move();
    static MoveStack get_last_move();
    static int get_winner();
    static int get_steps();
    static int rollback();
    static int get_status();
    static std::vector<MoveStack> get_move_stack();
    static void refresh_stack();
    bool is_empty();
};
int Logic::map[15][15] = { 0 };
int Logic::status = 0;
int Logic::winner = 0;
int Logic::map_size = 15;
std::vector<MoveStack> Logic::move_stack;
/*判断输赢的算法: 只需要判断当前落子相关的四条直线（横、竖、左斜、右斜），是否形成5个连子。
将直线上的落子(黑~ 1，白~ -1)，依次相加，连续的子绝对值之和达到5，即可判定为胜利*/
int Logic::check_winner() {
    int tmp = 0;
    MoveStack last_step = move_stack.back();
    // 竖向直线, x 固定
    for (int y = 0; y < map_size; y++) {
        // 必须是连续的
        if (y > 0 && map[last_step.x][y] != map[last_step.x][y - 1])
        {
            tmp = 0;
        };
        tmp += map[last_step.x][y];
        if (abs(tmp) >= 5) {
            return last_step.type;
        };
    };
    // 横向直线, y 固定
    tmp = 0;
    for (int x = 0; x < map_size; x++) {
        // 必须是连续的
        if (x > 0 && map[x][last_step.y] != map[x - 1][last_step.y])
        {
            tmp = 0;
        };
        tmp += map[x][last_step.y];
        if (abs(tmp) >= 5) {
            return last_step.type;
        };
    };
    tmp = 0;
    int min_dist;
    if (last_step.x <= last_step.y) {
        min_dist = last_step.x;
    }
    else {
        min_dist = last_step.y;
    };
    int top_point[2] = { last_step.x - min_dist, last_step.y - min_dist };
    for (int incr = 0; incr < map_size; incr++) {
        // 不能超出棋盘边
        if (top_point[0] + incr > map_size - 1 || top_point[1] + incr > map_size - 1)
        {
            break;
        };
        // 必须是连续的
        if (incr > 0 && map[top_point[0] + incr][top_point[1] + incr] != map[top_point[0] + incr - 1][top_point[1] + incr - 1]) {
            tmp = 0;
        };
        tmp += map[top_point[0] + incr][top_point[1] + incr];
        if (abs(tmp) >= 5) {
            return last_step.type;
        };
    }
    // 左斜直线，计算出右上角顶点的坐标。然后x递减、y递增，到达最左下角顶点。
    tmp = 0;
    if ((map_size - 1 - last_step.x) <= last_step.y) {
        min_dist = map_size - 1 - last_step.x;
    }
    else {
        min_dist = last_step.y;
    };
    top_point[0] = last_step.x + min_dist;
    top_point[1] = last_step.y - min_dist;
    for (int incr = 0; incr < map_size; incr++) {
        // 不能超出棋盘边界
        if (top_point[0] - incr < 0 || (top_point[1] + incr) >(map_size - 1)) {
            break;
        };
        // 必须是连续的
        if (incr > 0 && (map[top_point[0] - incr][top_point[1] + incr] != map[top_point[0] - incr + 1][top_point[1] + incr - 1])) {
            tmp = 0;
        };
        tmp += map[top_point[0] - incr][top_point[1] + incr];
        if (abs(tmp) >= 5) {
            return last_step.type;
        };
    };
    return 0;
};
int Logic::check() {
    // 所有步数已经走完
    if (move_stack.size() >= map_size * map_size) {
        return G_FINISH;
    };
    // 赢了
    int win = check_winner();
    if (win != 0) {
        winner = win;
        return G_WIN;
    }
    // 未结束
    return G_OK;
}
/* 获取当前状态
    0 ~ 未开局
    1 ~ 等待黑棋落子
    2 ~ 等待白棋落子
    3 ~ 结束（一方获胜）
    4 ~ 结束（棋盘走满）*/
int Logic::get_status() {
    return status;
};
//获取走棋记录
void Logic::refresh_stack() {
    move_stack.clear();
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            map[i][j] = { 0 };
        };
    };
}
std::vector<MoveStack> Logic::get_move_stack() {
    return move_stack;
};
void Logic::set_status(int stat) {
    status = stat;
};
//开始状态
void Logic::start_move() {
    status = 1;
};
//最后一步棋子
MoveStack Logic::get_last_move() {
    return move_stack.back();
}
//获胜者
int Logic::get_winner() {
    return winner;
};
//步数表示
int Logic::get_steps() {
    int answer = move_stack.size();
    return answer;
};
int Logic::is_finish(int err_code) {
    if (err_code == G_FINISH || err_code == G_WIN)
    {
        return 1;
    };
    return 0;
};
int Logic::move(int x, int y) {
    if (status != 1 && status != 2) {
        return G_STAT_ERR;
    };
    if (map_size <= x || x < 0 || map_size <= y || y < 0) {
        return G_RANGE_ERR;
    };
    if (map[x][y] != 0) {
        return G_POS_PLACED;
    };
    MoveStack mid;
    status == 1 ? mid.type = 1 : mid.type = -1;
    map[x][y] = mid.type;
    mid.x = x;
    mid.y = y;
    move_stack.push_back(mid);

    // 判断是否结束
    int ret = check();
    if (is_finish(ret)) {
        if (ret == G_WIN) {
            set_status(3);
        }
        else {
            set_status(4);
        };
        return ret;
    };

    // 切换状态
    MoveStack last_step = move_stack.back();
    int stat;
    last_step.type == 1 ? stat = 2 : stat = 1;
    set_status(stat);
    return G_OK;
}
// 悔一步棋
int Logic::rollback() {
    if (model == G_HUMAN) {
        if (move_stack.size() == 0) {
            return G_ERR;
        };
        MoveStack step = move_stack.back();
        move_stack.pop_back();
        map[step.x][step.y] = 0;
        // 刷新当前状态
        if (step.type == 1) {
            status = 1;
        }  // 如果当前悔的是黑棋，那么状态切换为等待黑棋落子
        else if (step.type == -1) {
            status = 2;
        }
        else {
            return G_ERR;
        };
    }
    else if (model == G_AI) {
        if (move_stack.size() == 0 || move_stack.size() == 1) {
            return G_ERR;
        };
        MoveStack step = move_stack.back();
        move_stack.pop_back();
        map[step.x][step.y] = 0;
        if (move_stack.size() >= 1) {
            step = move_stack.back();
            move_stack.pop_back();
            map[step.x][step.y] = 0;
        };
        if (step.type == 1) {
            status = 1;
        }  // 如果当前悔的是黑棋，那么状态切换为等待黑棋落子
        else if (step.type == -1) {
            status = 2;
        }
        else {
            return G_ERR;
        };
    };
    return G_OK;
};
bool Logic::is_empty() {
    int flag = 0;
    for (int x = 0; x < 15; x++) {
        for (int y = 0; y < 15; y++) {
            if (map[x][y] != 0) {
                flag = 1;
            }
        };
    };
    if (flag == 0) {
        return true;
    }
    else {
        return false;
    };
};
ExMessage touch() {
    ExMessage msg;
    msg = getmessage(EM_MOUSE);
    if (!msg.lbutton) {
        msg.x = -1;
        msg.y = -1;
    };
    return msg;
}

class Draw :public Logic
{
private:
    //棋盘方格数
    int SIZE = 15;
    //棋盘方格大小
    int UNIT = 40;
    //标题
    //string TITLE = "古风五子"
    int PANEL_WIDTH = 200;  // 右侧面板宽度
    int BORDER_WIDTH = 50;  // 预留宽度

    // 计算棋盘的有效范围
    int RANGE_X[2] = { BORDER_WIDTH, BORDER_WIDTH + (SIZE - 1) * UNIT };
    int RANGE_Y[2] = { BORDER_WIDTH, BORDER_WIDTH + (SIZE - 1) * UNIT };

    // 计算状态面板的有效范围
    int PANEL_X[2] = { BORDER_WIDTH + (SIZE - 1) * UNIT,BORDER_WIDTH + (SIZE - 1) * UNIT + PANEL_WIDTH };
    int PANEL_Y[2] = { BORDER_WIDTH, BORDER_WIDTH + (SIZE - 1) * UNIT };

    // 计算窗口大小
    // 参数
    int offset_x = PANEL_X[0] + 50;
    int offset_y = PANEL_Y[0] + 400;
    int btn_h = 50;
    int btn_w = 150;
    int btn_gap = 20;
    int btn_text_x = 35;
    int btn_text_y = 15;
    //电脑提示
    int BTN_RANGE_DEGREE_GAME_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_DEGREE_GAME_Y[2] = { offset_y - (btn_h + btn_gap) * 3 , offset_y - (btn_h + btn_gap) * 3 + btn_h };
    //先手顺序
    int BTN_RANGE_TURN_GAME_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_TURN_GAME_Y[2] = { offset_y - (btn_h + btn_gap) * 2 , offset_y - (btn_h + btn_gap) * 2 + btn_h };
    // 人机对战
    int BTN_RANGE_AI_GAME_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_AI_GAME_Y[2] = { offset_y - (btn_h + btn_gap) , offset_y - (btn_h + btn_gap) + btn_h };
    // 新的一局
    int BTN_RANGE_NEW_START_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_NEW_START_Y[2] = { offset_y, offset_y + btn_h };
    // 退出游戏
    int BTN_RANGE_EXIT_GAME_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_EXIT_GAME_Y[2] = { offset_y + btn_h + btn_gap,offset_y + btn_h + btn_gap + btn_h };
    // 悔棋
    int BTN_RANGE_RB_X[2] = { offset_x, offset_x + btn_w };
    int BTN_RANGE_RB_Y[2] = { offset_y + (btn_h + btn_gap) * 2,offset_y + (btn_h + btn_gap) * 2 + btn_h };



public:
    Draw(int map_size, int map_unit);
    Draw() {};
    void drawMap();
    void drawLine(int color, int start_x, int start_y, int end_x, int end_y, int thickness);
    void drawWord(char text[20], int color, int site_x, int site_y, int size, char type[20], int mode = TRANSPARENT);
    void drawCircle(int color, int x, int y, int size);
    void drawChess();
    void drawPanel();
    void drawRectangle(int color, int top_x, int top_y, int bottom_x, int bottom_y);
    void drawImage();
    void redrawAll();
    void start();
    void gameInit();
    int doHumanMove(int pos[2]);
    int doAImove();
    void doTurn();
    void doDegree();
    int doBtn(int pos[2]);
    void doRollback();
    void doNewStart();
    void doAI();
    ~Draw();
};

Draw::Draw(int map_size, int map_unit)
{
    SIZE = map_size;
    UNIT = map_unit;
};
Draw::~Draw() {}
void Draw::drawLine(int color, int start_x, int start_y, int end_x, int end_y, int thickness) {
    setcolor(BLACK);
    line(start_x, start_y, end_x, end_y);
};
void Draw::drawWord(char text[], int color, int site_x, int site_y, int size, char type[],int mode) {
    setcolor(color);
    settextstyle(size, 0, type,0,0,200,false, false, false);
    setbkmode(mode);
    outtextxy(site_x, site_y, text);
};
void Draw::drawCircle(int color, int x, int y, int size) {
    setcolor(color);
    circle(x, y, size);
    setfillcolor(color);
    floodfill(x + size / 2, y + size / 2, color);
    floodfill(x - size / 2, y + size / 2, color);
    floodfill(x + size / 2, y - size / 2, color);
    floodfill(x - size / 2, y - size / 2, color);
};
void Draw::drawRectangle(int color, int top_x, int top_y, int bottom_x, int bottom_y) {
    setcolor(color);
    rectangle(top_x, top_y, top_x + bottom_x, top_y + bottom_y);
    setfillcolor(color);
    floodfill(top_x + 1, top_y + 1, color);
};
void Draw::drawImage() {
    IMAGE pimg;
    loadimage(&pimg,TEXT("res\\bg.jpg"));
    putimage(0, 0, &pimg);
};
void Draw::drawMap() {
    int POS_START[2] = { 50,50 };
    //绘制行
    for (int item = 0; item < SIZE; item++) {
        drawLine(BLACK, POS_START[0], POS_START[1] + item * UNIT, POS_START[0] + (SIZE - 1) * UNIT, POS_START[1] + item * UNIT, 1);
        char text[10];
        _itoa_s(item + 1, text, 10);
        drawWord(text, BLACK, POS_START[0] - 30, POS_START[1] + item * UNIT - 10, 16, (char*)"arial");
    };
    //绘制列
    for (int item = 0; item < SIZE; item++) {
        drawLine(BLACK, POS_START[0] + item * UNIT, POS_START[1], POS_START[0] + item * UNIT, POS_START[1] + (SIZE - 1) * UNIT, 1);
        char text[10];
        _itoa_s(item + 1, text, 10);
        drawWord(text, BLACK, POS_START[0] + item * UNIT - 10, POS_START[1] - 30, 16, (char*)"arial");
    };
    //绘制五点
    drawCircle(BLACK, POS_START[0] + round((map_size - 1) / 2) * UNIT, POS_START[1] + round((map_size - 1) / 2) * UNIT, 3);
    drawCircle(BLACK, POS_START[0] + round((map_size - 1) / 2 - 2) * UNIT, POS_START[1] + round((map_size - 1) / 2 - 2) * UNIT, 3);
    drawCircle(BLACK, POS_START[0] + round((map_size - 1) / 2 - 2) * UNIT, POS_START[1] + round((map_size - 1) / 2 + 2) * UNIT, 3);
    drawCircle(BLACK, POS_START[0] + round((map_size - 1) / 2 + 2) * UNIT, POS_START[1] + round((map_size - 1) / 2 - 2) * UNIT, 3);
    drawCircle(BLACK, POS_START[0] + round((map_size - 1) / 2 + 2) * UNIT, POS_START[1] + round((map_size - 1) / 2 + 2) * UNIT, 3);
};
// 绘制棋子
void Draw::drawChess() {
    std::vector<MoveStack> mst = get_move_stack();
    for (int item = 0; item < mst.size(); item++) {
        int x = BORDER_WIDTH + mst[item].x * UNIT;
        int y = BORDER_WIDTH + mst[item].y * UNIT;
        int t_color;
        mst[item].type == 1 ? t_color = BLACK : t_color = WHITE;
        drawCircle(t_color, x, y, int(UNIT / 2.5));
    };
};
void Draw::drawPanel() {
    drawRectangle(WHITE, PANEL_X[0] + 30, 0, 1000, 1000);
    // 走棋状态
    int stat = get_status();
    char stat_str[20];
    char* p = stat_str;
    if (stat == 0) {
        p = (char*)"点击开始按钮";
    }
    else if (stat == 1) {
        p = (char*)"等待黑棋落子..";
    }
    else if (stat == 2) {
        p = (char*)"等待白棋落子..";
    }
    else if (stat == 4) {
        p = (char*)"和棋！        ";
    }
    else if (stat == 3) {
        int winner = get_winner();
        if (winner == 1) {
            p = (char*)"黑棋获胜！      ";
        }
        else {
            p = (char*)"白棋获胜！      ";
        };
    }
    else {
        p = (char*)"";
    };
    drawWord(p, BLACK, PANEL_X[0] + 50, PANEL_Y[0] + 50, 20, (char*)"simhei", OPAQUE);
    //步数
    int steps = get_steps();
    char step_str[10];
    sprintf_s(step_str, "步数:%d", steps);
    drawWord(step_str, BLACK, PANEL_X[0] + 50, PANEL_Y[0] + 100, 20, (char*)"simhei", OPAQUE);
    //落子位置
    if (!is_empty()){
        MoveStack las = get_last_move();
        char las_str[20];
        if (stat == 1) {
            sprintf_s(las_str, "白方落子:(%d，%d)  ", las.x + 1, las.y + 1);
            drawWord(las_str, BLACK, PANEL_X[0] + 50, PANEL_Y[0] + 150, 20, (char*)"simhei", OPAQUE);
        }
        else if (stat == 2) {
            sprintf_s(las_str, "黑方落子:(%d，%d)  ", las.x + 1, las.y + 1);
            drawWord(las_str, BLACK, PANEL_X[0] + 50, PANEL_Y[0] + 150, 20, (char*)"simhei", OPAQUE);
        }; 
    }
    else {
        drawWord((char*)"未有落子", BLACK, PANEL_X[0] + 50, PANEL_Y[0] + 150, 20, (char*)"simhei", OPAQUE);
    }
    
    //水印
    drawWord((char*)"水木出品", BLACK, PANEL_X[0] + 50, PANEL_Y[0], 20, (char*)"simhei");
    //难度模式
    if (model == G_AI) {
        drawRectangle(MediumAquamarine, offset_x, offset_y - (btn_h + btn_gap) * 3, btn_w, btn_h);
        //     if(depth == 6){
        //         drawWord((char*)"简单模式",WHITE,offset_x + btn_text_x, offset_y - (btn_h + btn_gap)*3 + btn_text_y,20,(char*)"simhei");
        //     };
        //     if(depth == 8){
        //         drawWord((char*)"困难模式",WHITE,offset_x + btn_text_x, offset_y - (btn_h + btn_gap)*3 + btn_text_y,20,(char*)"simhei");
        //     };
        // };
        drawWord((char*)"电脑提示", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) * 3 + btn_text_y, 20, (char*)"simhei");
    };
    //先手顺序
    if (model == G_AI) {
        drawRectangle(MediumAquamarine, offset_x, offset_y - (btn_h + btn_gap) * 2, btn_w, btn_h);
        if (turn == 1) {
            drawWord((char*)"玩家先手", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) * 2 + btn_text_y, 20, (char*)"simhei");
        };
        if (turn == -1) {
            drawWord((char*)"电脑先手", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) * 2 + btn_text_y, 20, (char*)"simhei");
        };
    };
    // 人机对战
    drawRectangle(MediumAquamarine, offset_x, offset_y - (btn_h + btn_gap), btn_w, btn_h);
    if (model == G_AI) {
        drawWord((char*)"人机模式", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) + btn_text_y, 20, (char*)"simhei");
    };
    if (model == G_HUMAN) {
        drawWord((char*)"双人模式", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) + btn_text_y, 20, (char*)"simhei");
    };
    if (model == G_AI_AI) {
        drawWord((char*)"机器对弈", WHITE, offset_x + btn_text_x, offset_y - (btn_h + btn_gap) + btn_text_y, 20, (char*)"simhei");
    };
    // 新的一局
    drawRectangle(MediumAquamarine, offset_x, offset_y, btn_w, btn_h);
    drawWord((char*)"新开一局", WHITE, offset_x + btn_text_x, offset_y + btn_text_y, 20, (char*)"simhei");
    // 退出游戏
    drawRectangle(MediumAquamarine, offset_x, offset_y + btn_h + btn_gap, btn_w, btn_h);
    drawWord((char*)"退出游戏", WHITE, offset_x + btn_text_x, offset_y + btn_h + btn_gap + btn_text_y, 20, (char*)"simhei");
    // 悔棋
    if (model != G_AI_AI) {
        drawRectangle(MediumAquamarine, offset_x, offset_y + (btn_h + btn_gap) * 2, btn_w, btn_h);
        drawWord((char*)"悔一步棋", WHITE, offset_x + btn_text_x, offset_y + (btn_h + btn_gap) * 2 + btn_text_y, 20, (char*)"simhei");
    };
};
void Draw::redrawAll() {
    cleardevice();
    //重刷背景图
    drawImage();
    //绘制棋盘
    drawMap();
    //绘制棋子
    drawChess();
    //绘制面板
    drawPanel();
}

void Draw::gameInit() {
    // 初始化pygame
    // 设置窗口的大小，单位为像素
    // 设置窗口标题
    // 设置背景颜色
    setbkcolor(WHITE);
    drawImage();


    //绘制棋盘
    drawMap();

    // 绘制右侧的状态面板
    drawPanel();
};
int Draw::doHumanMove(int pos[2]) {
    // 落子在棋盘之外无效
    if (pos[0] < RANGE_X[0] || pos[0] > RANGE_X[1] || pos[1] < RANGE_Y[0] || pos[1] > RANGE_Y[1]) {
        soundPlay(sound_error);
        return G_ERR;
    };
    if (!is_empty()) {
        MoveStack last_move = get_last_move();
        int t_color;
        last_move.type == 1 ? t_color = BLACK : t_color = WHITE;
        int x = BORDER_WIDTH + UNIT * last_move.x;
        int y = BORDER_WIDTH + UNIT * last_move.y;
        drawCircle(t_color, x, y, 3);
        drawCircle(t_color, x, y, (int)(UNIT / 2.5));
    }
    // 判断当前落子的位置,需要吸附在最近的落棋点
    int s_x = round(((float)pos[0] - (float)BORDER_WIDTH) / (float)UNIT);
    int s_y = round(((float)pos[1] - (float)BORDER_WIDTH) / (float)UNIT);
    int x = BORDER_WIDTH + UNIT * s_x;
    int y = BORDER_WIDTH + UNIT * s_y;
    // 先move，再draw
    if (map[s_x][s_y] != 0) {
        soundPlay(sound_error);
        return G_ERR;
    };
    int ret = move(s_x, s_y);
    if (ret < 0) {
        done = -done;
        soundPlay(sound_error);
        //mciSendString(TEXT("play sound_error"), NULL, 0, NULL);
        return G_ERR;
    };
    // draw
    if (ret >= 0) {
        MoveStack last_move = get_last_move();
        int t_color;
        last_move.type == 1 ? t_color = BLACK : t_color = WHITE;
        drawCircle(t_color, x, y, (int)(UNIT / 2.5));
        drawCircle(RED, x, y, 3);
        //drawCircle(BLACK, [x, y], int(UNIT / 2.5), 1);
        drawPanel();
    };
    // 播放落子音效
    if (get_status() == 1) {
        soundPlay(sound_black);
        //mciSendString(TEXT("play sound_black"), NULL, 0, NULL);
    }
    else {
        soundPlay(sound_white);
        //mciSendString(TEXT("play sound_white"), NULL, 0, NULL);
    };
    if (get_status() >= 3) {
        mciSendString(TEXT("stop sound_background"), NULL, 0, NULL);
        soundPlay(sound_start);
        //mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    };
    return G_OK;
};
int Draw::doAImove() {
    if (!is_empty()) {
        MoveStack last_move = get_last_move();
        int t_color;
        last_move.type == 1 ? t_color = BLACK : t_color = WHITE;
        int x = BORDER_WIDTH + UNIT * last_move.x;
        int y = BORDER_WIDTH + UNIT * last_move.y;
        drawCircle(t_color, x, y, 3);
        drawCircle(t_color, x, y, (int)(UNIT / 2.5));
    }
    int setting[2];
    if (!is_empty()) {
        chessAi ai;
        for (int x = 0; x < 15; x++) {
            for (int y = 0; y < 15; y++) {
                map[x][y] == -1 ? ai.chesses[x][y] = 2 : ai.chesses[x][y] = map[x][y];
            };
        };
        ai.nodeNum = 0;
        if (!ai.analyse_kill(ai.chesses, 16)) {
            //cout<"没找到杀棋";
            ai.analyse(ai.chesses, 6, -INT_MAX, INT_MAX);
        }
        else {
            //cout<<"找到了杀棋";
        };
        setting[0] = ai.decision.pos.x();
        setting[1] = ai.decision.pos.y();
        ai.~chessAi();
    }
    else {
        setting[0] = rand() % 5 + 5;
        setting[1] = rand() % 5 + 5;
    };
    if (map[setting[0]][setting[1]] != 0) {
        status = 4;
        drawPanel();
        return G_ERR;
    };
    //e.analyse(m,3,-999999,999999);
    cout << "x: " << setting[0] << " " << "y: " << setting[1] << endl;
    if (get_status() == 1) {
        soundPlay(sound_black);
        //mciSendString(TEXT("play sound_black"), NULL, 0, NULL);
    }
    else {
        soundPlay(sound_white);
        //mciSendString(TEXT("play sound_white"), NULL, 0, NULL);
    }
    // 先move，再draw
    int ret = move(setting[0], setting[1]);
    if (ret < 0) {
        soundPlay(sound_error);
        //mciSendString(TEXT("play sound_error"), NULL, 0, NULL);
        return G_ERR;
    };
    // draw
    if (ret >= 0) {
        MoveStack last_move = get_last_move();
        int t_color;
        last_move.type == 1 ? t_color = BLACK : t_color = WHITE;
        int x = BORDER_WIDTH + UNIT * setting[0];
        int y = BORDER_WIDTH + UNIT * setting[1];
        drawCircle(t_color, x, y, (int)(UNIT / 2.5));
        drawCircle(RED, x, y, 3);
        // pygame.draw.circle(screen, BLACK, [x, y], int(UNIT / 2.5), 1)
        drawPanel();
    }
    if (get_status() >= 3) {
        mciSendString(TEXT("stop sound_background"), NULL, 0, NULL);
        soundPlay(sound_start);
        //mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    };
    Sleep(100);
    return G_OK;
};

void Draw::doRollback() {
    if (get_status() >= 3) {
        soundPlay(sound_background);
        done = 1;
        //mciSendString(TEXT("play sound_background"), NULL, 0, NULL);
    };
    if (rollback() == G_OK) {
        redrawAll();
    };
};
void Draw::doAI() {
    model = (model + 1) % 3;
    if (model != G_AI) {
        turn = 1;
    };
    refresh_stack();
    redrawAll();
    soundPlay(sound_start);
    soundPlay(sound_background);
    //mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    start();
};
void Draw::doTurn() {
    turn = turn * (-1);
    done = done * (-1);
    refresh_stack();
    redrawAll();
    soundPlay(sound_start);
    soundPlay(sound_background);
    //mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    start();
   
};
void Draw::doDegree() {
    if (model == G_AI) {
        doAImove();
        done = -1;
    };
    // depth==6?depth=8:depth=6;
    // refresh_stack();
    // redrawAll();
    // mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    // start();
};
int Draw::doBtn(int pos[2]) {
    if (pos[0] > BTN_RANGE_NEW_START_X[0] && pos[0] < BTN_RANGE_NEW_START_X[1]) {
        if (pos[1] > BTN_RANGE_NEW_START_Y[0] && pos[1] < BTN_RANGE_NEW_START_Y[1]) {
            doNewStart();
            cout << "doNewStart" << endl;
            return G_OK;
        }
        else if (BTN_RANGE_AI_GAME_Y[0] < pos[1] && pos[1] < BTN_RANGE_AI_GAME_Y[1]) {
            doAI();
            cout << "doAI" << endl;
            return G_OK;
        }
        else if ((model == G_AI)&&BTN_RANGE_TURN_GAME_Y[0] < pos[1] && pos[1] < BTN_RANGE_TURN_GAME_Y[1]) {
            doTurn();
            std::cout << "doTurn" << endl;
            return G_OK;
        }
        else if((get_status() <3)&&(model == G_AI)&&BTN_RANGE_DEGREE_GAME_Y[0] < pos[1]&& pos[1] < BTN_RANGE_DEGREE_GAME_Y[1]){
            doDegree();
            cout<<"doDegree"<<endl;
            return G_OK;
        }
        else if (BTN_RANGE_EXIT_GAME_Y[0] < pos[1] && pos[1] < BTN_RANGE_EXIT_GAME_Y[1]) {
            closegraph();
            exit(0);
            return G_OK;
        }
        else if (BTN_RANGE_RB_Y[0] < pos[1] && pos[1] < BTN_RANGE_RB_Y[1] && model != G_AI_AI) {
            doRollback();
            cout << "doRollback" << endl;
            return G_OK;
        }
    }
    else {
        return G_ERR;
    };
    redrawAll();
    return G_ERR;
};
void Draw::start() {
    srand((unsigned)time(NULL));
    done = turn;
    start_move();
    gameInit();
    soundPlay(sound_background);
    //mciSendString(TEXT("play sound_background"), NULL, 0, NULL);
    // 程序主循环
    while (true) {
        ExMessage msg = touch();
        int pos[2] = { msg.x,msg.y };
        //cout<<"model:"<<model<<"done:"<<done<<endl;
        //std::cout<<pos[0]<<' '<<pos[1]<<' '<<endl;
            // 判断事件是否为退出事件
            // 落子事件
        if (pos[0] > 0) {
            int btn = doBtn(pos);
            if (btn == G_OK) {
                Sleep(300);
            }
            //std::cout<<btn<<endl;
            else if (btn==G_ERR && done == 1)
            {
                // 非按钮事件，则处理走棋
                //std::cout<<btn<<endl; 
                int key = doHumanMove(pos);
                //redrawAll();
                if (model > 0 && key != G_ERR) {
                    done = done * (-1);
                };
            };
        }
        else if (get_status() <= 2 && done == -1) {
            //mciSendString(TEXT("stop sound_background"), NULL, 0, NULL);
            soundPlay(sound_background);
            if (model == G_AI) {
                std::cout << "run" << endl;
                doAImove();
                //drawAll();
                done = done * (-1);
            }
            else if (model == G_AI_AI) {
                std::cout << "run" << endl;
                doAImove();
                //redrawAll();
            };
        }

    };
};
void Draw::doNewStart() {
    refresh_stack();
    redrawAll();
    soundPlay(sound_start);
    soundPlay(sound_background);
    //mciSendString(TEXT("play sound_start"), NULL, 0, NULL);
    start();
}

int main(int argc, char* argv[]) {
    ShowWindow(FindWindow("ConsoleWindowClass", argv[0]), 0);
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    Draw draw;
    draw.start();
};