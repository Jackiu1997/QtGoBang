#ifndef GOBANGMODEL_H
#define GOBANGMODEL_H

#include <QObject>
#include <vector>
#include <stack>
using std::vector;
using std::stack;
using std::pair;
using std::make_pair;

// AI评分表项目
struct MaskItem {
    vector<int> typeVec;
    int stoneNum;
    int mask;
};

// AI搜索状态
struct Status {
    int alpha;
    int beta;

    int dropType; // 当前状态落子类型
    int nowStep;
    int bestStep;

    vector<vector<int> > board;
    vector<pair<int, int> > stepVec;

    Status() {}
    Status(int type, vector<vector<int> > b)
        : alpha(-99999999),
          beta(99999999),
          dropType(type),
          nowStep(0),
          bestStep(0),
          board(b)
    {}
};

// 五子棋游戏模型
class GobangModel
{
public:
    enum Player{BLACK, WHITE} nowPlayer;
    enum GameMode{PVP, PVC} gameMode;
    enum GameStatus{READY, PLAYING, WIN, DEAD} gameStatus;

    const int boardWidth = 15;
    vector<vector<int> > gameBoard;

    GobangModel();
    void initGameBoard();
    void startPVP();
    void startPVC();
    void gameOneStep(int row, int col, Player player);

    void aiDropStone();
private:
    vector<MaskItem> maskTable; // 评分表

    // 游戏胜负判断
    bool judgeWinner(vector<vector<int> > board);
    bool judgeDead(vector<vector<int> > board);

    // AI 功能辅助函数
    bool generateMaskTable();
    bool isContainType(vector<int> line, vector<int> type);
    int calculateLineMask(vector<int> line);
    int calculateMapMask(vector<vector<int> > board);
    bool getNewBoardStatus(Status &oldStatus, Status &newStatus);

    // AI 落子计算函数
    pair<int, int> simpleAiDrop(vector<vector<int> > board);
    pair<int, int> alphaBetaDrop(vector<vector<int> > board, int level);
    void getPossibleSteps(Status &status);
};

#endif // GOBANGMODEL_H
