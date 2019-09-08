#ifndef GOBANGMODEL_H
#define GOBANGMODEL_H

#include <QObject>
#include <vector>
#include <stack>
using std::vector;
using std::stack;
using std::pair;
using std::make_pair;

// 评分表项目
struct MaskItem {
    vector<int> typeVec;
    int stoneNum;
    int mask;
};

// 搜索结点状态栈
struct Status {
    int stoneType; // 当前状态落子类型
    int nowIndex;
    int nodeMask;
    vector<vector<int> > board;
    vector<pair<int, int> > clickVec;

    Status() {}
    Status(int type, vector<vector<int> > b) : stoneType(type), board(b), nowIndex(0), nodeMask(0) {
        int width = board.size();
        for (int row = 0; row < width; row++) {
            for (int col = 0; col < width; col++) {
                if (board[row][col] == 0)
                    clickVec.push_back(make_pair(row, col));
            }
        }
    }
};

class GobangModel
{
public:
    enum Player{BLACK, WHITE} nowPlayer;
    enum GameMode{PVP, PVC} gameMode;
    enum GameStatus{READY, PLAYING, WIN, DEAD} gameStatus;

    const int boardWidth = 15;
    vector<vector<int>> gameBoard;

    vector<MaskItem> maskTable; // 评分表

    GobangModel();
    void initGameBoard();
    void startPVP();
    void startPVC();
    void placeStone(int row, int col);

    void aiDropStone();
private:

    bool judgeWinner(vector<vector<int> > board);
    bool judgeDead(vector<vector<int> > board);

    bool generateMaskTable();
    bool isContainType(vector<int> line, vector<int> type);
    int calculateLineMask(vector<int> line);
    int calculateMapMask(vector<vector<int> > board);
    pair<int, int> alphaBetaCut(vector<vector<int> > board);

    bool getNewStatus(Status &oldStatus, Status &newStatus);
    int sortStautusClickVec(Status &status, int level);
};

#endif // GOBANGMODEL_H
