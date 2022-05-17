#include <vector>
#include <iostream>
#include <string.h>
using namespace std;

// #include <QDebug>
// #include <QPoint>
// #include <QMap>

#define C_NONE 0//���ӣ�����,����,����
#define C_BLACK 1
#define C_WHITE 2

#define RIGHT 0//�������ҡ����ϡ����ϡ�����
#define UP 1
#define UPRIGHT 2
#define UPLEFT 3

//�������͵Ĵ��� /Ȩ��
#define OTHER 0//��������
#define WIN 1//1000000,��Ӯ
#define LOSE 2//-10000000
#define FLEX4 3//50000,�׻�4
#define flex4 4//-100000
#define BLOCK4 5//400
#define block4 6//-100000
#define FLEX3 7//400
#define flex3 8//-8000
#define BLOCK3 9//20
#define block3 10//-40
#define FLEX2 11//20
#define flex2 12//-40
#define BLOCK2 13//1
#define block2 14//-2
#define FLEX1 15//1
#define flex1 16//-2

enum gameMode { PLAYER, AI };
enum gameStatus { UNDERWAY, FINISH };
enum gameTurn { T_BLACK, T_WHITE };//�ֵ�˭��
enum gameResult { R_BLACK, R_WHITE, R_DRAW };//����Ӯ������Ӯ������

class QPoint
{
public:
    int _x=7;
    int _y=7;
    QPoint() {};
    ~QPoint() {};
    QPoint(int x, int y) {
        _x = x;
        _y = y;
    };
    int x() {
        return _x;
    };
    int y() {
        return _y;
    };
    void setX(int num) {
        _x = num;
    };
    void setY(int num) {
        _y = num;
    };
};

struct EVALUATION {
    int score;
    gameResult result;
    int STAT[8];//���沿�����εĸ���,�±�WIN=1Ϊ����5,LOSE=2Ϊ����5,FLEX4=3Ϊ�׻�4,BLOCK4=5Ϊ�׳�4,FLEX3=7Ϊ�׻�3
};
struct _POINTS {//�������λ��,[0]�������,[19]�������
    QPoint pos[20];
    int score[20]={0};//�˴����ӵľ��Ʒ���
};
struct DECISION {
    QPoint pos;//λ��
    int eval=0;//�Է���������
};


class chessAi
{
public:
    int chesses[15][15];//����
    DECISION decision;//���漫��С�����õ���Ҫ�ߵ�λ��

    int nodeNum = 0;

private:
    int tuple6type[4][4][4][4][4][4];//���ͱ�ʶ����,0����,1����,2����,3�߽�
    _POINTS points;//�������λ��

public://̰���㷨����
    chessAi();
    ~chessAi() {};
    int calcOnePosGreedy(int board[15][15], int row, int col, int C_ME);//����ĳһ��λ�ö���me�ķ���(̰��)
    int tupleScoreGreedy(int black, int white, int C_ME);//����ڰ��Ӹ�������Ԫ�����me�ķ���(̰���㷨)
    QPoint getXY(int row, int col, int dir, int rel);//���ڸ����㡢�����ƫ�������õ�ʵ������
    bool checkBound(int x, int y);//���߽�
    QPoint findBestMoveGreedy(int C_ME);//̰���㷨ֻ��һ��,Ч��������,����Ŀ���ǳ

public://��������������
    void init_tuple6type();//�������ж����鸳��ֵ
    _POINTS seekPoints(int board[15][15]);//���ɶ��ڰ��ӵ����20������λ�ü�����
    void copyBoard(int A[15][15], int B[15][15]);//��A���̸��Ƶ�B����
    void reverseBoard(int A[15][15], int B[15][15]);//��A���̺ڰ��ӵߵ��������B����
    EVALUATION evaluate(int board[15][15], bool needPrint = false);//�����board�ĺ��ӵľ��ƹ�ֵ����,�������ж���Ӯ
    int analyse(int board[15][15], int depth, int alpha, int beta);//����������С������ab��֦
    int analysefirst(int(*board)[15], int depth, int alpha, int beta);
    bool analyse_kill(int board[15][15], int depth);//����ɱ��,���ҵ�ɱ���򷵻�true
    bool analyse_kill_first(int(*board)[15], int depth);
    vector<QPoint> seek_kill_points(int board[15][15]);//�Ұ������5,��4,��4,��3��ɱ��λ��

};
chessAi::chessAi()
{
    init_tuple6type();
    cout << "��ʼ��ai";
}
//�����
bool chessAi::checkBound(int x, int y) {
    if (x >= 0 && x < 15 && y >= 0 && y < 15)return true;
    else return false;
}
//�����
QPoint chessAi::getXY(int row, int col, int dir, int rel) {
    QPoint p;
    if (dir == RIGHT) {
        p.setX(row);
        p.setY(col + rel);
    }
    else if (dir == UP) {
        p.setX(row - rel);
        p.setY(col);
    }
    else if (dir == UPRIGHT) {
        p.setX(row - rel);
        p.setY(col + rel);
    }
    else if (dir == UPLEFT) {
        p.setX(row - rel);
        p.setY(col - rel);
    }
    return p;
}
//�����
int chessAi::calcOnePosGreedy(int board[15][15], int row, int col, int C_ME) {
    int sum = 0;
    for (int i = 0; i < 4; ++i) {//�ĸ�����
        for (int j = 0; j < 5; ++j) {//ÿ�����������5����Ԫ��
            QPoint start = getXY(row, col, RIGHT + i, j - 4);//��Ԫ�鶥��λ��
            QPoint end = getXY(start.x(), start.y(), RIGHT + i, 4);//��Ԫ����Զλ��
            if (checkBound(start.x(), start.y()) && checkBound(end.x(), end.y())) {//����Ԫ���±���Ϸ�
                int blackChess = 0;
                int whiteChess = 0;
                for (int k = 0; k < 5; ++k) {//����Ԫ��5��λ�ÿ���
                    QPoint tmp = getXY(start.x(), start.y(), RIGHT + i, k);
                    if (board[tmp.x()][tmp.y()] == C_BLACK)blackChess++;
                    if (board[tmp.x()][tmp.y()] == C_WHITE)whiteChess++;
                }
                sum += tupleScoreGreedy(blackChess, whiteChess, C_ME);
            }
        }
    }
    return sum;
}
//�����
int chessAi::tupleScoreGreedy(int black, int white, int C_ME) {
    //��5
    if (C_ME == C_BLACK && black == 5)return 9999999;
    if (C_ME == C_WHITE && white == 5)return 9999999;
    //ȫ��
    if (black == 0 && white == 0)return 7;
    //polluted
    else if (black >= 1 && white >= 1)return 0;
    else if (C_ME == C_BLACK) {
        if (black == 1 && white == 0)return 35;
        else if (black == 2 && white == 0)return 800;
        else if (black == 3 && white == 0)return 15000;
        else if (black == 4 && white == 0)return 800000;
        else if (black == 0 && white == 1)return 15;
        else if (black == 0 && white == 2)return 400;
        else if (black == 0 && white == 3)return 1800;
        else return 100000;
    }
    else {
        if (black == 1 && white == 0)return 15;
        else if (black == 2 && white == 0)return 400;
        else if (black == 3 && white == 0)return 1800;
        else if (black == 4 && white == 0)return 100000;
        else if (black == 0 && white == 1)return 35;
        else if (black == 0 && white == 2)return 800;
        else if (black == 0 && white == 3)return 15000;
        else return 800000;
    }
}
QPoint chessAi::findBestMoveGreedy(int C_ME) {
    int bestScore = 0;
    int bestRow = 0, bestCol = 0;
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (chesses[i][j] == C_NONE) {//�յ�λ��
                int score = calcOnePosGreedy(chesses, i, j, C_ME);
                if (bestScore < score) {
                    bestScore = score;
                    bestRow = i;
                    bestCol = j;
                }
            }
        }
    }
    QPoint p(bestRow, bestCol);
    return p;
}
void chessAi::init_tuple6type() {
    memset(tuple6type, 0, sizeof(tuple6type));//ȫ����Ϊ0
    //����5,aiӮ
    tuple6type[2][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][0] = WIN;
    tuple6type[0][2][2][2][2][2] = WIN;
    tuple6type[2][2][2][2][2][1] = WIN;
    tuple6type[1][2][2][2][2][2] = WIN;
    tuple6type[3][2][2][2][2][2] = WIN;//�߽翼��
    tuple6type[2][2][2][2][2][3] = WIN;
    //����5,ai��
    tuple6type[1][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][0] = LOSE;
    tuple6type[0][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][2] = LOSE;
    tuple6type[2][1][1][1][1][1] = LOSE;
    tuple6type[3][1][1][1][1][1] = LOSE;
    tuple6type[1][1][1][1][1][3] = LOSE;
    //�׻�4
    tuple6type[0][2][2][2][2][0] = FLEX4;
    //�ڻ�4
    tuple6type[0][1][1][1][1][0] = flex4;
    //�׻�3
    tuple6type[0][2][2][2][0][0] = FLEX3;
    tuple6type[0][0][2][2][2][0] = FLEX3;
    tuple6type[0][2][0][2][2][0] = FLEX3;
    tuple6type[0][2][2][0][2][0] = FLEX3;
    //�ڻ�3
    tuple6type[0][1][1][1][0][0] = flex3;
    tuple6type[0][0][1][1][1][0] = flex3;
    tuple6type[0][1][0][1][1][0] = flex3;
    tuple6type[0][1][1][0][1][0] = flex3;
    //�׻�2
    tuple6type[0][2][2][0][0][0] = FLEX2;
    tuple6type[0][2][0][2][0][0] = FLEX2;
    tuple6type[0][2][0][0][2][0] = FLEX2;
    tuple6type[0][0][2][2][0][0] = FLEX2;
    tuple6type[0][0][2][0][2][0] = FLEX2;
    tuple6type[0][0][0][2][2][0] = FLEX2;
    //�ڻ�2
    tuple6type[0][1][1][0][0][0] = flex2;
    tuple6type[0][1][0][1][0][0] = flex2;
    tuple6type[0][1][0][0][1][0] = flex2;
    tuple6type[0][0][1][1][0][0] = flex2;
    tuple6type[0][0][1][0][1][0] = flex2;
    tuple6type[0][0][0][1][1][0] = flex2;
    //�׻�1
    tuple6type[0][2][0][0][0][0] = FLEX1;
    tuple6type[0][0][2][0][0][0] = FLEX1;
    tuple6type[0][0][0][2][0][0] = FLEX1;
    tuple6type[0][0][0][0][2][0] = FLEX1;
    //�ڻ�1
    tuple6type[0][1][0][0][0][0] = flex1;
    tuple6type[0][0][1][0][0][0] = flex1;
    tuple6type[0][0][0][1][0][0] = flex1;
    tuple6type[0][0][0][0][1][0] = flex1;

    int p1, p2, p3, p4, p5, p6, x, y, ix, iy;//x:��5�кڸ���,y:��5�а׸���,ix:��5�кڸ���,iy:��5�а׸���
    for (p1 = 0; p1 < 4; ++p1) {
        for (p2 = 0; p2 < 3; ++p2) {
            for (p3 = 0; p3 < 3; ++p3) {
                for (p4 = 0; p4 < 3; ++p4) {
                    for (p5 = 0; p5 < 3; ++p5) {
                        for (p6 = 0; p6 < 4; ++p6) {
                            x = y = ix = iy = 0;

                            if (p1 == 1)x++;
                            else if (p1 == 2)y++;

                            if (p2 == 1) { x++; ix++; }
                            else if (p2 == 2) { y++; iy++; }

                            if (p3 == 1) { x++; ix++; }
                            else if (p3 == 2) { y++; iy++; }

                            if (p4 == 1) { x++; ix++; }
                            else if (p4 == 2) { y++; iy++; }

                            if (p5 == 1) { x++; ix++; }
                            else if (p5 == 2) { y++; iy++; }

                            if (p6 == 1)ix++;
                            else if (p6 == 2)iy++;

                            if (p1 == 3 || p6 == 3) {//�б߽�
                                if (p1 == 3 && p6 != 3) {//��߽�
                                    //�׳�4
                                    if (ix == 0 && iy == 4) {//���ұ��п�λ�ǻ�4Ҳû��ϵ����Ϊ��4Ȩ��Զ���ڳ�4���ټ��ϳ�4Ȩ�ر仯���Բ���
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //�ڳ�4
                                    if (ix == 4 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //����3
                                    if (ix == 0 && iy == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //����3
                                    if (ix == 3 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //����2
                                    if (ix == 0 && iy == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //����2
                                    if (ix == 2 && iy == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                                else if (p6 == 3 && p1 != 3) {//�ұ߽�
                                   //�׳�4
                                    if (x == 0 && y == 4) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                    }
                                    //�ڳ�4
                                    if (x == 4 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                    }
                                    //����3
                                    if (x == 3 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                    }
                                    //����3
                                    if (x == 0 && y == 3) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                    }
                                    //����2
                                    if (x == 2 && y == 0) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                    }
                                    //����2
                                    if (x == 0 && y == 2) {
                                        if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                            tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                    }
                                }
                            }
                            else {//�ޱ߽�
                               //�׳�4
                                if ((x == 0 && y == 4) || (ix == 0 && iy == 4)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK4;
                                }
                                //�ڳ�4
                                if ((x == 4 && y == 0) || (ix == 4 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block4;
                                }
                                //����3
                                if ((x == 0 && y == 3) || (ix == 0 && iy == 3)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK3;
                                }
                                //����3
                                if ((x == 3 && y == 0) || (ix == 3 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block3;
                                }
                                //����2
                                if ((x == 0 && y == 2) || (ix == 0 && iy == 2)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = BLOCK2;
                                }
                                //����2
                                if ((x == 2 && y == 0) || (ix == 2 && iy == 0)) {
                                    if (tuple6type[p1][p2][p3][p4][p5][p6] == 0)
                                        tuple6type[p1][p2][p3][p4][p5][p6] = block2;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
//�����
EVALUATION chessAi::evaluate(int board[15][15], bool needPrint) {
    //������Ȩ��
    int weight[17] = { 0,1000000,-10000000,50000,-100000,400,-100000,400,-8000,20,-50,20,-50,1,-3,1,-3 };

    int i, j, type;
    int stat[4][17];//ͳ��4��������ÿ�����͵ĸ���
    memset(stat, 0, sizeof(stat));


    int A[17][17];//�����߽�����������,board[i][j]=A[i-1][j-1],3��ʾ�߽�
    for (int i = 0; i < 17; ++i)A[i][0] = 3;
    for (int i = 0; i < 17; ++i)A[i][16] = 3;
    for (int j = 0; j < 17; ++j)A[0][j] = 3;
    for (int j = 0; j < 17; ++j)A[16][j] = 3;
    for (int i = 0; i < 15; ++i)
        for (int j = 0; j < 15; ++j)
            A[i + 1][j + 1] = board[i][j];

    //�жϺ�������
    for (i = 1; i <= 15; ++i) {
        for (j = 0; j < 12; ++j) {
            type = tuple6type[A[i][j]][A[i][j + 1]][A[i][j + 2]][A[i][j + 3]][A[i][j + 4]][A[i][j + 5]];
            stat[0][type]++;
        }
    }
    //�ж���������
    for (j = 1; j <= 15; ++j) {
        for (i = 0; i < 12; ++i) {
            type = tuple6type[A[i][j]][A[i + 1][j]][A[i + 2][j]][A[i + 3][j]][A[i + 4][j]][A[i + 5][j]];
            stat[1][type]++;
        }
    }
    //�ж���������������
    for (i = 0; i < 12; ++i) {
        for (j = 0; j < 12; ++j) {
            type = tuple6type[A[i][j]][A[i + 1][j + 1]][A[i + 2][j + 2]][A[i + 3][j + 3]][A[i + 4][j + 4]][A[i + 5][j + 5]];
            stat[2][type]++;
        }
    }
    //�ж���������������
    for (i = 0; i < 12; ++i) {
        for (j = 5; j < 17; ++j) {
            type = tuple6type[A[i][j]][A[i + 1][j - 1]][A[i + 2][j - 2]][A[i + 3][j - 3]][A[i + 4][j - 4]][A[i + 5][j - 5]];
            stat[3][type]++;
        }
    }

    EVALUATION eval;
    memset(eval.STAT, 0, sizeof(eval.STAT));

    int score = 0;
    for (i = 1; i < 17; ++i) {
        score += (stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i]) * weight[i];//�����Ʒ�

        int count = stat[0][i] + stat[1][i] + stat[2][i] + stat[3][i];//ͳ�����з����ϲ������͵ĸ���
        if (i == WIN)eval.STAT[WIN] = count;
        else if (i == LOSE)eval.STAT[LOSE] = count;
        else if (i == FLEX4)eval.STAT[FLEX4] = count;
        else if (i == BLOCK4)eval.STAT[BLOCK4] = count;
        else if (i == FLEX3)eval.STAT[FLEX3] = count;
    }

    /* if(needPrint==true){
         cout<<"���������ж�";
         for(int i=0;i<17;++i)
             cout<<i<<":"<<STAT[i];
     }
 */
    eval.result = R_DRAW;

    //��Ӯ
    if (eval.STAT[WIN] > 0)eval.result = R_WHITE;
    //��Ӯ
    else if (eval.STAT[LOSE] > 0)eval.result = R_BLACK;

    eval.score = score;
    return eval;
}
//�����
_POINTS chessAi::seekPoints(int board[15][15]) {
    bool B[15][15];//�������
    int worth[15][15];
    _POINTS best_points;

    memset(B, 0, sizeof(B));
    for (int i = 0; i < 15; ++i) {//ÿ���ǿյ㸽��8����������3�����,����Խ������Ϊ����
        for (int j = 0; j < 15; ++j) {
            if (board[i][j] != C_NONE) {
                for (int k = -3; k <= 3; ++k) {
                    if (i + k >= 0 && i + k < 15) {
                        B[i + k][j] = true;
                        if (j + k >= 0 && j + k < 15)B[i + k][j + k] = true;
                        if (j - k >= 0 && j - k < 15)B[i + k][j - k] = true;
                    }
                    if (j + k >= 0 && j + k < 15)B[i][j + k] = true;
                }
            }
        }
    }

    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            worth[i][j] = -INT_MAX;
            if (board[i][j] == C_NONE && B[i][j] == true) {
                //board[i][j]=C_BLACK;
                worth[i][j] = calcOnePosGreedy(board, i, j, C_WHITE);
                //worth[i][j]=evaluate(board).score;
                //board[i][j]=C_NONE;
            }
        }
    }

    int w;
    for (int k = 0; k < 20; ++k) {
        w = -INT_MAX;
        for (int i = 0; i < 15; ++i) {
            for (int j = 0; j < 15; ++j) {
                if (worth[i][j] > w) {
                    w = worth[i][j];
                    QPoint tmp(i, j);
                    best_points.pos[k] = tmp;
                }
            }
        }

        int x = best_points.pos[k].x(), y = best_points.pos[k].y();
        board[x][y] = C_WHITE;
        best_points.score[k] = evaluate(board).score;
        board[x][y] = C_NONE;

        worth[best_points.pos[k].x()][best_points.pos[k].y()] = -INT_MAX;//�������һ��,������һ���λ�úͷ���
    }
    return best_points;
}
//�����
void chessAi::copyBoard(int(*A)[15], int(*B)[15]) {
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (A[i][j] == C_NONE)B[i][j] = C_NONE;
            else if (A[i][j] == C_BLACK)B[i][j] = C_BLACK;
            else B[i][j] = C_WHITE;
        }
    }
}
//�����
void chessAi::reverseBoard(int(*A)[15], int(*B)[15]) {
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            if (A[i][j] == C_NONE)B[i][j] = C_NONE;
            else if (A[i][j] == C_BLACK)B[i][j] = C_WHITE;
            else B[i][j] = C_BLACK;
        }
    }
}

int chessAi::analyse(int(*board)[15], int depth, int alpha, int beta) {
    EVALUATION EVAL = evaluate(board);
    if (depth == 0 || EVAL.result != R_DRAW) {//�ִ������/���ģ�����ӿ��Էֳ���Ӯ����ôֱ�ӷ��ؽ��������Ҫ������
        nodeNum += 1;
        if (depth == 0) {
            _POINTS P;
            P = seekPoints(board);

            return P.score[0];//�������λ�ö�Ӧ����߷�
        }
        else return EVAL.score;
    }
    else if (depth % 2 == 0) {//max��,�ҷ�(��)����
       //cout<<"�׷����ߣ�";

        _POINTS P = seekPoints(board);

        for (int i = 0; i < 10; ++i) {
            //cout<<"�׷�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();
            int sameBoard[15][15];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].x()][P.pos[i].y()] = C_WHITE;//ģ�⼺������,������board,������ܸı�board����Ϣ
            int a = analyse(sameBoard, depth - 1, alpha, beta);
            if (a > alpha) {
                alpha = a;
                if (depth == 6) {
                    cout << "set decision:" << P.pos[i].x() << P.pos[i].y();
                    decision.pos.setX(P.pos[i].x());
                    decision.pos.setY(P.pos[i].y());
                    decision.eval = a;
                }
            }
            if (beta <= alpha)break;//��֦
        }
        return alpha;
    }
    else {//min��,�з�(��)����
       //cout<<"�ڷ����ߣ�";

        int rBoard[15][15];
        reverseBoard(board, rBoard);
        _POINTS P = seekPoints(rBoard);//�Ҷ��ں��ӵ����λ��,��Ҫ�����̲�ͬ��ɫ��ת,��ΪseekPoint�����ɫ�������λ��

        for (int i = 0; i < 10; ++i) {
            //cout<<"�ڷ�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();

            int sameBoard[15][15];
            copyBoard(board, sameBoard);

            sameBoard[P.pos[i].x()][P.pos[i].y()] = C_BLACK;//ģ��з�����
            int a = analyse(sameBoard, depth - 1, alpha, beta);
            if (a < beta)beta = a;
            if (beta <= alpha)break;//��֦
        }
        return beta;
    }
}
int chessAi::analysefirst(int(*board)[15], int depth, int alpha, int beta) {
    EVALUATION EVAL = evaluate(board);
    if (depth == 1 || EVAL.result != R_DRAW) {//�ִ������/���ģ�����ӿ��Էֳ���Ӯ����ôֱ�ӷ��ؽ��������Ҫ������
        nodeNum += 1;
        if (depth == 1) {
            _POINTS P;
            P = seekPoints(board);
            return P.score[0];//�������λ�ö�Ӧ����߷�
        }
        else return EVAL.score;
    }
    else if (depth % 2 == 1) {//max��,�ҷ�(��)����
       //cout<<"�׷����ߣ�";

        _POINTS P = seekPoints(board);

        for (int i = 0; i < 10; ++i) {
            //cout<<"�׷�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();
            int sameBoard[15][15];
            copyBoard(board, sameBoard);
            sameBoard[P.pos[i].x()][P.pos[i].y()] = C_WHITE;//ģ�⼺������,������board,������ܸı�board����Ϣ
            int a = analysefirst(sameBoard, depth - 1, alpha, beta);
            if (a > alpha) {
                alpha = a;
                if (depth == 6) {
                    cout << "set decision:" << P.pos[i].x() << P.pos[i].y();
                    decision.pos.setX(P.pos[i].x());
                    decision.pos.setY(P.pos[i].y());
                    decision.eval = a;
                }
            }
            if (beta <= alpha)break;//��֦
        }
        return alpha;
    }
    else {//min��,�з�(��)����
       //cout<<"�ڷ����ߣ�";

        int rBoard[15][15];
        reverseBoard(board, rBoard);
        _POINTS P = seekPoints(rBoard);//�Ҷ��ں��ӵ����λ��,��Ҫ�����̲�ͬ��ɫ��ת,��ΪseekPoint�����ɫ�������λ��

        for (int i = 0; i < 10; ++i) {
            //cout<<"�ڷ�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();

            int sameBoard[15][15];
            copyBoard(board, sameBoard);

            sameBoard[P.pos[i].x()][P.pos[i].y()] = C_BLACK;//ģ��з�����
            int a = analysefirst(sameBoard, depth - 1, alpha, beta);
            if (a < beta)beta = a;
            if (beta <= alpha)break;//��֦
        }
        return beta;
    }
}
bool chessAi::analyse_kill(int(*board)[15], int depth) {
    EVALUATION EVAL = evaluate(board);
    if (depth == 0 || EVAL.result != R_DRAW) {
        if (depth == 0) {//���ִ������,��һ���԰�������λ��,�����廹ûӮ�򷵻�false
            _POINTS P;
            P = seekPoints(board);
            board[P.pos[0].x()][P.pos[0].y()] = C_WHITE;

            gameResult result = evaluate(board).result;
            if (result == R_WHITE)return true;
            else return false;
        }
        else if (EVAL.result == R_WHITE)return true;//�ҵ�����ɱ��
        else return false;//������
    }
    else if (depth % 2 == 0) {//max��,�ҷ�(��)����
        if (depth == 16 || depth == 14) {//�ʼ4��ѡ�������ߵ�10����
            _POINTS P = seekPoints(board);
            for (int i = 0; i < 10; ++i) {
                int sameBoard[15][15];
                copyBoard(board, sameBoard);

                sameBoard[P.pos[i].x()][P.pos[i].y()] = C_WHITE;//ģ�⼺������

                if (analyse_kill(sameBoard, depth - 1)) {
                    if (depth == 16) {
                        cout << "kill set decision:" << P.pos[i].x() << P.pos[i].y();

                        decision.pos.setX(P.pos[i].x());
                        decision.pos.setY(P.pos[i].y());
                        decision.eval = INT_MAX;//����һ��..
                    }
                    return true;
                }
            }
            return false;
        }
        else {//����ֻѡɱ���
            vector<QPoint> pointList = seek_kill_points(board);//����ɱ���

            if (pointList.size() == 0)return false;//û��ɱ���
            for (auto i : pointList) {
                //cout<<"�׷�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();
                int sameBoard[15][15];
                copyBoard(board, sameBoard);

                sameBoard[i.x()][i.y()] = C_WHITE;//ģ�⼺������

                if (analyse_kill(sameBoard, depth - 1)) {
                    /*if(depth==16){
                        cout<<"kill set decision:"<<i.x()<<i.y();

                        decision.pos.setX(i.x());
                        decision.pos.setY(i.y());
                        decision.eval=INT_MAX;//����һ��..
                    }*/
                    return true;
                }
            }
            return false;
        }
    }
    else {//min��,�з�(��)����,ֻ�¶��Լ���õ���
        int rBoard[15][15];
        reverseBoard(board, rBoard);
        _POINTS P = seekPoints(rBoard);//�Ҷ��ں��ӵ����λ��,��Ҫ�����̲�ͬ��ɫ��ת,��ΪseekPoint�����ɫ�������λ��

        int sameBoard[15][15];
        copyBoard(board, sameBoard);
        sameBoard[P.pos[0].x()][P.pos[0].y()] = C_BLACK;//ģ��з�����:ֻ����õ�һ��
        //�����֦
        return analyse_kill(sameBoard, depth - 1);
    }
}
bool chessAi::analyse_kill_first(int(*board)[15], int depth) {
    EVALUATION EVAL = evaluate(board);
    if (depth == 1 || EVAL.result != R_DRAW) {
        if (depth == 1) {//���ִ������,��һ���԰�������λ��,�����廹ûӮ�򷵻�false
            _POINTS P;
            P = seekPoints(board);
            board[P.pos[0].x()][P.pos[0].y()] = C_WHITE;

            gameResult result = evaluate(board).result;
            if (result == R_WHITE)return true;
            else return false;
        }
        else if (EVAL.result == R_WHITE)return true;//�ҵ�����ɱ��
        else return false;//������
    }
    else if (depth % 2 == 1) {//max��,�ҷ�(��)����
        if (depth == 17 || depth == 15) {//�ʼ4��ѡ�������ߵ�10����
            _POINTS P = seekPoints(board);
            for (int i = 0; i < 10; ++i) {
                int sameBoard[15][15];
                copyBoard(board, sameBoard);

                sameBoard[P.pos[i].x()][P.pos[i].y()] = C_WHITE;//ģ�⼺������

                if (analyse_kill_first(sameBoard, depth - 1)) {
                    if (depth == 17) {
                        cout << "kill set decision:" << P.pos[i].x() << P.pos[i].y();

                        decision.pos.setX(P.pos[i].x());
                        decision.pos.setY(P.pos[i].y());
                        decision.eval = INT_MAX;//����һ��..
                    }
                    return true;
                }
            }
            return false;
        }
        else {//����ֻѡɱ���
            vector<QPoint> pointList = seek_kill_points(board);//����ɱ���

            if (pointList.size() == 0)return false;//û��ɱ���
            for (auto i : pointList) {
                //cout<<"�׷�ģ����"<<P.pos[i].x()<<","<<P.pos[i].y();
                int sameBoard[15][15];
                copyBoard(board, sameBoard);

                sameBoard[i.x()][i.y()] = C_WHITE;//ģ�⼺������

                if (analyse_kill_first(sameBoard, depth - 1)) {
                    /*if(depth==16){
                        cout<<"kill set decision:"<<i.x()<<i.y();

                        decision.pos.setX(i.x());
                        decision.pos.setY(i.y());
                        decision.eval=INT_MAX;//����һ��..
                    }*/
                    return true;
                }
            }
            return false;
        }
    }
    else {//min��,�з�(��)����,ֻ�¶��Լ���õ���
        int rBoard[15][15];
        reverseBoard(board, rBoard);
        _POINTS P = seekPoints(rBoard);//�Ҷ��ں��ӵ����λ��,��Ҫ�����̲�ͬ��ɫ��ת,��ΪseekPoint�����ɫ�������λ��

        int sameBoard[15][15];
        copyBoard(board, sameBoard);
        sameBoard[P.pos[0].x()][P.pos[0].y()] = C_BLACK;//ģ��з�����:ֻ����õ�һ��
        //�����֦
        return analyse_kill_first(sameBoard, depth - 1);
    }
}
vector<QPoint> chessAi::seek_kill_points(int(*board)[15]) {
    vector<QPoint> pointList;

    _POINTS P = seekPoints(board);//һ����˵,�ܳ�4���߻�3�ı�������ǰ20�ĵ���

    int sameBoard[15][15];
    copyBoard(board, sameBoard);

    for (int i = 0; i < 20; ++i) {
        sameBoard[P.pos[i].x()][P.pos[i].y()] = C_WHITE;//ģ������
        if (evaluate(sameBoard).STAT[WIN] > 0) {//������5
            pointList.push_back(P.pos[i]);
        }
        else if (evaluate(sameBoard).STAT[FLEX4] > evaluate(board).STAT[FLEX4]) {//�����»�4
            pointList.push_back(P.pos[i]);
        }
        else if (evaluate(sameBoard).STAT[BLOCK4] > evaluate(board).STAT[BLOCK4]) {//�����³�4
            pointList.push_back(P.pos[i]);
        }
        else if (evaluate(sameBoard).STAT[FLEX3] > evaluate(board).STAT[FLEX3]) {//�����»�3
            pointList.push_back(P.pos[i]);
        }
        sameBoard[P.pos[i].x()][P.pos[i].y()] = C_NONE;//��ԭ����
    }
    return pointList;
}
