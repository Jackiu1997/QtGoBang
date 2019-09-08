#include "gobangmodel.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QStringList>

GobangModel::GobangModel()
{
    initGameBoard();
    // 生成评分表完成
    generateMaskTable();
}

void GobangModel::initGameBoard()
{
    gameBoard.clear();
    for (int row = 0; row < boardWidth; row++) {
        vector<int> line;
        for (int col = 0; col < boardWidth; col++) line.push_back(0);
        gameBoard.push_back(line);
    }
    gameStatus = READY;
}

void GobangModel::startPVP()
{
    initGameBoard();

    gameMode = PVP;
    gameStatus = PLAYING;
    nowPlayer = BLACK;
}

void GobangModel::startPVC()
{
    initGameBoard();

    gameMode = PVC;
    gameStatus = PLAYING;
    nowPlayer = BLACK;
}


// Game Logic
void GobangModel::gameOneStep(int row, int col, Player player)
{
    if (gameStatus == PLAYING) {
        if (gameBoard[row][col] == 0) {
            switch (player) {
            case BLACK: // 黑方落子
                gameBoard[row][col] = 1;
                nowPlayer = WHITE;
                break;
            case WHITE: // 白方落子
                gameBoard[row][col] = -1;
                nowPlayer = BLACK;
                break;
            }
        }
    }

    if (judgeWinner(gameBoard)) {
        gameStatus = WIN;
    } else if (judgeDead(gameBoard)) {
        gameStatus = DEAD;
    }
}

bool GobangModel::judgeWinner(vector<vector<int> > board)
{
    if (nowPlayer == BLACK) {
        for (auto &line: board)
            for (auto &item: line)
                item = -item;
    }

    vector<int> line;
    // 横向
    for (int row = 0; row < boardWidth; row++) {
        line = board[row];
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }
    // 纵向
    for (int col = 0; col < boardWidth; col++) {
        for (int row = 0; row < boardWidth; row++) {
            line.push_back(board[row][col]);
        }
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }

    // 左下斜线
    for (int row = 0; row < boardWidth - 4; row++) {
        for (int nRow = row, col = 0; nRow < boardWidth; nRow++, col++) {
            line.push_back(board[nRow][col]);
        }
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }
    // 右上斜线
    for (int col = 1; col < boardWidth - 4; col++) {
        for (int nCol = col, row = 0; nCol < boardWidth; nCol++, row++) {
            line.push_back(board[row][nCol]);
        }
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }


    // 左上斜线
    for (int row = boardWidth - 1; row >= 4; row--) {
        for (int nRow = row, col = 0; nRow >= 0; nRow--, col++) {
            line.push_back(board[nRow][col]);
        }
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }
    // 右下斜线
    for (int col = 1; col < boardWidth - 4; col++) {
        for (int nCol = col, row = boardWidth - 1; nCol < boardWidth; nCol++, row--) {
            line.push_back(board[row][nCol]);
        }
        if (isContainType(line, maskTable[0].typeVec)) return true;
        line.clear();
    }

    return false;
}

bool GobangModel::judgeDead(vector<vector<int> > board)
{
    for (int row = 0; row < boardWidth; row++) {
        for (int col = 0; col < boardWidth; col++) {
            if (board[row][col] == 0) return false;
        }
    }

    return true;
}


// AI
<<<<<<< HEAD
void GobangModel::aiDropStone()
{
    pair<int,int> aiDrop = alphaBetaDrop(gameBoard, 3); // 目前最高算力在四层
    gameOneStep(aiDrop.first, aiDrop.second, WHITE);
=======
void GobangModel::aiDropStone() {
    int row, col, whiteCount = 0;
    for (int i = 0; i < boardWidth; i++) {
        for (int j = 0; j < boardWidth; j++) {
            if (gameBoard[i][j] == -1) {
                whiteCount++;
                row = i; col =j;
            }
        }
    }

    if (whiteCount == 0) {
        row = rand() % 5 + 5;
        col = rand() % 5 + 5;
        if (gameBoard[row][col] != 0) {
            placeStone(row - 1, col);
        } else placeStone(row, col);
    }
    else if (whiteCount == 1) {
        if (gameBoard[row-1][col] == 0) {
            placeStone(row - 1, col);
        }
        else if (gameBoard[row+1][col] == 0) {
            placeStone(row + 1, col);
        }
        else if (gameBoard[row][col+1] == 0) {
            placeStone(row, col+1);
        }
        else if (gameBoard[row][col-1] == 0) {
            placeStone(row, col-1);
        }
    }
    else {
        pair<int,int> aiDrop = alphaBetaCut(gameBoard);
        placeStone(aiDrop.first, aiDrop.second);
    }
>>>>>>> dev#alpha_beta
}

bool GobangModel::generateMaskTable()
{
    QFile file("E:/WorkSpace/Qt/Gobang/maskTable2.txt");
    if(!file.open(QIODevice::ReadOnly)) return false;

    QTextStream *out = new QTextStream(&file);
    QStringList tempOption = out->readAll().split("\r\n");

    MaskItem item;
    for (int i = 0; i < tempOption.count(); ) {
        QStringList maskList = tempOption.at(i++).split(" ");
        if (maskList.count() == 3) {
            item.mask = maskList.at(0).toInt();
            item.stoneNum = maskList.at(1).toInt();
            for (int j = 0; j < maskList.at(2).toInt(); j++) {
                QStringList dataList = tempOption.at(i++).split(" ");
                for (int q = 0; q < dataList.count(); q++) {
                    item.typeVec.push_back(dataList.at(q).toInt());
                }
                maskTable.push_back(item);
                item.typeVec.clear();
            }
        }
    }
    file.close();

    return true;
}

bool GobangModel::isContainType(vector<int> line, vector<int> type)
{
    int lineSize = line.size(), typeSize = type.size();
    if (lineSize < typeSize) return false;

    bool isContain = false;

    QString lineString;
    for (auto &it: line) {
        it = it == -1 ? 2 : it;
        lineString += QString::number(it);
    }
    QString typeString;
    for (auto it: type) {
        it = it == -1 ? 2 : it;
        typeString += QString::number(it);
    }
    isContain = lineString.contains(typeString);

    if (!isContain) {
        vector<int> reType = type;
        std::reverse(reType.begin(), reType.end());
        if (reType != type) {
            QString reTypeString;
            for (auto it: reType) reTypeString += QString::number(it);
            isContain = lineString.contains(reTypeString);
        }
    }

    return isContain;
}

int GobangModel::calculateLineMask(vector<int> line)
{
    int blackCount = 0, whiteCount = 0;
    vector<int> whiteLine = line;
    for (auto &item: whiteLine) {
        if (item == 1) blackCount++;
        else if (item == -1) whiteCount++;
        item = -item; // 反转黑白棋子
    }

    int totalMask = 0;
    for (auto item: maskTable) {
        // 计算黑子得分（负分）
        if (blackCount >= item.stoneNum)
            totalMask += isContainType(line, item.typeVec) ? -item.mask*1.1 : 0; //失败威胁
        // 计算白子得分（正分）
        if (whiteCount >= item.stoneNum)
            totalMask += isContainType(whiteLine, item.typeVec) ? item.mask : 0;
    }

    return totalMask;
}

int GobangModel::calculateMapMask(vector<vector<int> > board)
{
    int allMask = 0;
    vector<int> line;

    // 横向
    for (int row = 0; row < boardWidth; row++) {
        allMask += calculateLineMask(board[row]);
    }
    // 纵向
    for (int col = 0; col < boardWidth; col++) {
        for (int row = 0; row < boardWidth; row++) {
            line.push_back(board[row][col]);
        }
        allMask += calculateLineMask(line);
        line.clear();
    }

    // 左下斜线
    for (int row = 0; row < boardWidth - 4; row++) {
        for (int nRow = row, col = 0; nRow < boardWidth; nRow++, col++) {
            line.push_back(board[nRow][col]);
        }
        allMask += calculateLineMask(line);
        line.clear();
    }
    // 右上斜线
    for (int col = 1; col < boardWidth - 4; col++) {
        for (int nCol = col, row = 0; nCol < boardWidth; nCol++, row++) {
            line.push_back(board[row][nCol]);
        }
        allMask += calculateLineMask(line);
        line.clear();
    }


    // 左上斜线
    for (int row = boardWidth - 1; row >= 4; row--) {
        for (int nRow = row, col = 0; nRow >= 0; nRow--, col++) {
            line.push_back(board[nRow][col]);
        }
        allMask += calculateLineMask(line);
        line.clear();
    }
    // 右下斜线
    for (int col = 1; col < boardWidth - 4; col++) {
        for (int nCol = col, row = boardWidth - 1; nCol < boardWidth; nCol++, row--) {
            line.push_back(board[row][nCol]);
        }
        allMask += calculateLineMask(line);
        line.clear();
    }

    return allMask;
}

<<<<<<< HEAD
void GobangModel::getPossibleSteps(Status &status) {
    int width = status.board.size();
    // 选择所有未落子点
    for (int row = 0; row < width; row++)
        for (int col = 0; col < width; col++)
            if (status.board[row][col] == 0) status.stepVec.push_back(make_pair(row, col));

    // 删除无用的落子点（周围八个方向没有棋子）
    for (auto ite = status.stepVec.begin(); ite != status.stepVec.end();) {
        int counter = 0, row = ite->first, col = ite->second;
        status.board[row][col] = 3; //测试用子,表示这里有一个子反正不是0
        for (int i = ((row - 1) > 0 ? (row - 1) : 0); i <= ((row + 1) < 14 ? (row + 1) : 14); ++i)
            for (int j = ((col - 1) > 0 ? (col - 1) : 0); j <= ((col + 1) < 14 ? (col + 1) : 14); ++j)
                counter += status.board[i][j] != 0 ? 1 : 0;
        status.board[row][col] = 0;
        if (counter > 1) ++ite;
        else ite = status.stepVec.erase(ite);
=======
int GobangModel::sortStautusClickVec(Status &status) {

    vector<pair<int, int> > maskSort, clickVec;
    clickVec = status.clickVec;
    status.clickVec.clear();

    for (int index = 0; index < clickVec.size(); index++) {
        vector<vector<int> > newBoard = status.board;
        int row = clickVec[index].first;
        int col = clickVec[index].second;
        newBoard[row][col] = status.stoneType;

        int mask = calculateMapMask(newBoard);
        maskSort.push_back(make_pair(index, mask));
>>>>>>> dev#alpha_beta
    }
    // 排序有用的落子点

}

bool GobangModel::getNewBoardStatus(Status &oldStatus, Status &newStatus)
{
    if (oldStatus.nowStep == (int)oldStatus.stepVec.size()) return false;

    int nowStep = oldStatus.nowStep;
    oldStatus.nowStep++;
    vector<vector<int> > newBoard = oldStatus.board;
    newBoard[oldStatus.stepVec[nowStep].first][oldStatus.stepVec[nowStep].second] = oldStatus.dropType;

    newStatus = Status(-oldStatus.dropType, newBoard);
    newStatus.alpha = oldStatus.alpha;
    newStatus.beta = oldStatus.beta;
    getPossibleSteps(newStatus);

    return true;
}

<<<<<<< HEAD
pair<int, int> GobangModel::simpleAiDrop(vector<vector<int> > board)
{
    // 统计玩家或者电脑连成的子
    int manNum = 0; // 玩家连成子的个数
    int botNum = 0;    // AI连成子的个数
    int emptyNum = 0;  // 各方向空白位的个数

    // 清空评分数组
    vector<vector<int> > scoreMapVec;
    for (int i = 0; i < boardWidth; i++) {
        std::vector<int> lineScores;
        for (int j = 0; j < boardWidth; j++)
            lineScores.push_back(0);
        scoreMapVec.push_back(lineScores);
    }

    // 计分（此处是完全遍历，其实可以用bfs或者dfs加减枝降低复杂度，通过调整权重值，调整AI智能程度以及攻守风格）
    for (int row = 0; row < boardWidth; row++)
        for (int col = 0; col < boardWidth; col++) {
            // 空白点就算
            if (row > 0 && col > 0 && board[row][col] == 0) {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++) {
                        // 重置
                        manNum = 0;
                        botNum = 0;
                        emptyNum = 0;

                        // 原坐标不算 每个方向延伸4个子
                        if (!(y == 0 && x == 0)) {
                            // 对玩家黑子评分（正反两个方向）
                            for (int i = 1; i <= 4; i++) {
                                if (row + i * y > 0 && row + i * y < boardWidth &&
                                    col + i * x > 0 && col + i * x < boardWidth &&
                                    board[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    manNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < boardWidth &&
                                         col + i * x > 0 && col + i * x < boardWidth &&
                                         board[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else break; // 出边界
                            }

                            for (int i = 1; i <= 4; i++) {
                                if (row - i * y > 0 && row - i * y < boardWidth &&
                                    col - i * x > 0 && col - i * x < boardWidth &&
                                    board[row - i * y][col - i * x] == 1) // 玩家的子
                                {
                                    manNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < boardWidth &&
                                         col - i * x > 0 && col - i * x < boardWidth &&
                                         board[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else break; // 出边界
                            }

                            switch (manNum) {
                            case 1: // 杀二
                                scoreMapVec[row][col] += 10;
                                break;
                            case 2: // 杀二
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 30;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 40;
                                break;
                            case 3: // 杀四
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 60;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 110;
                                break;
                            case 4: // 杀五
                                scoreMapVec[row][col] += 10100;
                                break;
                            default:
                                break;
                            }

                            // 进行一次清空
                            emptyNum = 0;
                            // 对AI白子评分
                            for (int i = 1; i <= 4; i++) {
                                if (row + i * y > 0 && row + i * y < boardWidth &&
                                    col + i * x > 0 && col + i * x < boardWidth &&
                                    board[row + i * y][col + i * x] == 1)    // 玩家的子
                                {
                                    botNum++;
                                }
                                else if (row + i * y > 0 && row + i * y < boardWidth &&
                                         col + i * x > 0 && col + i * x < boardWidth &&
                                         board[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else break;           // 出边界
                            }

                            for (int i = 1; i <= 4; i++) {
                                if (row - i * y > 0 && row - i * y < boardWidth &&
                                    col - i * x > 0 && col - i * x < boardWidth &&
                                    board[row - i * y][col - i * x] == -1)    // AI的子
                                {
                                    botNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < boardWidth &&
                                         col - i * x > 0 && col - i * x < boardWidth &&
                                         board[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else break;           // 出边界
                            }

                            switch (botNum) {
                            case 0: // 普通下子
                                scoreMapVec[row][col] += 5;
                                break;
                            case 1: // 活二
                                scoreMapVec[row][col] += 10;
                                break;
                            case 2: // 三
                                if (emptyNum == 1)                // 死三
                                    scoreMapVec[row][col] += 25;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 50;  // 活三
                            case 3: // 四
                                if (emptyNum == 1)                // 死四
                                    scoreMapVec[row][col] += 55;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 100; // 活四
                            case 4: // 活五
                                scoreMapVec[row][col] += 10000;   // 活五
                                break;
                            default:
                                break;
                            }
                        }
                    }
=======
pair<int, int> GobangModel::alphaBetaCut(vector<vector<int> > board) {
    int level = 4;
    int alpha = -10000000, beta = 10000000;
    vector<pair<int, int> > bestClickVec;
>>>>>>> dev#alpha_beta

            }
        }

    int maxScore = 0;
    vector<pair<int, int> > maxPoints;
    for (int row = 1; row < boardWidth; row++) {
        for (int col = 1; col < boardWidth; col++) {
            // 前提是这个坐标是空的
            if (board[row][col] == 0) {
                if (scoreMapVec[row][col] > maxScore) { // 找最大的数和坐标
                    maxPoints.clear();
                    maxScore = scoreMapVec[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                }
                else if (scoreMapVec[row][col] == maxScore)     // 如果有多个最大的数，都存起来
                    maxPoints.push_back(std::make_pair(row, col));
            }
        }
    }

    // 随机落子，如果有多个点的话
    srand((unsigned)time(0));
    return maxPoints.at(rand() % maxPoints.size());
}

pair<int, int> GobangModel::alphaBetaDrop(vector<vector<int> > board, int level)
{
    Status rootStatus(-1, board);
    getPossibleSteps(rootStatus);

    int alphaCut = 0, betaCut = 0;
    vector<pair<int, int> > bestStepVec;
    bestStepVec.push_back(rootStatus.stepVec[0]);

    stack<Status> statusStack;
    statusStack.push(rootStatus);
    while (!statusStack.empty()) {
        int stackSize = statusStack.size();
        Status nowStatus = statusStack.top();
<<<<<<< HEAD
=======
        int nowMask = nowStatus.nodeMask;
>>>>>>> dev#alpha_beta

        // 获取新状态深入
        Status newStatus;
<<<<<<< HEAD
        if (stackSize != level && getNewBoardStatus(nowStatus, newStatus)) {
=======
        if (stackSize != level-1 && getNewStatus(nowStatus, newStatus)) {
>>>>>>> dev#alpha_beta
            statusStack.pop();
            statusStack.push(nowStatus);
            statusStack.push(newStatus);
            continue;
        }

        // 最终bestStep返回
        if (stackSize == 1) break;

        statusStack.pop();
        Status parent = statusStack.top();

<<<<<<< HEAD
        switch (stackSize%2) {
        case 1: // max层 alpha剪枝 beta上传
            nowStatus.alpha = stackSize == level ? calculateMapMask(nowStatus.board) : nowStatus.alpha;

            // alpha剪枝
            if (parent.alpha != -99999999 && nowStatus.alpha <= parent.alpha) {
=======
        switch (stackSize) {
        case 3:
            nowMask = sortStautusClickVec(nowStatus);
            // alpha Cut
            if (alpha != -10000000 && nowMask <= alpha) {
                if (nowMask == alpha) {
                    bestClickVec.push_back(parent.clickVec[parent.nowIndex - 1]);
                }
>>>>>>> dev#alpha_beta
                statusStack.pop();
                alphaCut++;
                continue;
            }
<<<<<<< HEAD
            // 上传alpha至beta
            if (nowStatus.alpha < parent.beta) {
                parent.beta = nowStatus.alpha;
                parent.bestStep = parent.nowStep - 1;
            }
            break;

        case 0: // min层 beta剪枝 alpha上传
            nowStatus.beta = stackSize == level ? calculateMapMask(nowStatus.board) : nowStatus.beta;

            // beta剪枝
            if (nowStatus.beta != 99999999 && nowStatus.beta >= parent.beta) {
=======
            beta = nowMask < beta ? nowMask : beta;
            parent.nodeMask = beta;
            statusStack.pop();
            statusStack.push(parent);
            continue;
        case 2:
            if (nowMask == alpha) {
                bestClickVec.push_back(parent.clickVec[parent.nowIndex - 1]);
            } else if (nowMask > alpha) {
                alpha = nowMask;
                parent.nodeMask = alpha;
                bestClickVec.clear();
                bestClickVec.push_back(parent.clickVec[parent.nowIndex - 1]);
            }
            statusStack.pop();
            statusStack.push(parent);
            continue;
        case 1:
            statusStack.pop();
            continue;
        }

        /*
        switch (stackSize % 2) {
        case 0: // 偶数层 min beta剪枝
            if (beta != 10000000 && nowMask >= beta) {
>>>>>>> dev#alpha_beta
                statusStack.pop();
                betaCut++;
                continue;
            }
<<<<<<< HEAD
            // 上传beta至alpha
            if (nowStatus.beta > parent.alpha) {
                parent.alpha = nowStatus.beta;
                parent.bestStep = parent.nowStep - 1;
            }
=======
            alpha = nowMask > alpha ? nowMask : alpha;
            parent.nodeMask = alpha;
            break;
        case 1: // 奇数层 max alpha剪枝
            if (alpha != -10000000 && nowMask <= alpha) {
                statusStack.pop();
                alphaCut++;
                continue;
            }
            beta = nowMask < beta ? nowMask : beta;
            parent.nodeMask = beta;
>>>>>>> dev#alpha_beta
            break;
        }
        statusStack.pop();
        statusStack.push(parent);
        */
    }

<<<<<<< HEAD
    return statusStack.top().stepVec[statusStack.top().bestStep];

    srand(time(NULL));
    return bestStepVec[rand() % bestStepVec.size()];
=======
    srand(time(NULL));
    return bestClickVec[rand() % bestClickVec.size()];
>>>>>>> dev#alpha_beta
}
