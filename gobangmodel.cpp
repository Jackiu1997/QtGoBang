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
        for (int col = 0; col < boardWidth; col++) {
            line.push_back(0);
        }
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
void GobangModel::placeStone(int row, int col)
{
    if (gameStatus == PLAYING) {
        if (gameBoard[row][col] == 0) {
            // 黑方落子
            if (nowPlayer == BLACK) {
                gameBoard[row][col] = 1;
                nowPlayer = WHITE;
            }
            // 白方落子
            else {
                gameBoard[row][col] = -1;
                nowPlayer = BLACK;
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
void GobangModel::aiDropStone() {
    int whiteCount = 0;
    for (int i = 0; i < boardWidth; i++) {
        for (int j = 0; j < boardWidth; j++) {
            if (gameBoard[i][j] == -1) whiteCount++;
        }
    }

    if (whiteCount == 0 || whiteCount == 1) {
        int row = rand() % 5 + 5;
        int col = rand() % 5 + 5;
        if (gameBoard[row][col] != 0) {
            placeStone(row - 1, col);
        } else placeStone(row, col);
    } else {
        pair<int,int> aiDrop = alphaBetaCut(gameBoard);
        placeStone(aiDrop.first, aiDrop.second);
    }
}

bool GobangModel::generateMaskTable() {
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

bool GobangModel::isContainType(vector<int> line, vector<int> type) {
    int lineSize = line.size(), typeSize = type.size();
    if (lineSize < typeSize) return false;

    bool isContain = false;

    QString lineString;
    for (auto it: line) lineString += QString::number(it);
    QString typeString;
    for (auto it: type) typeString += QString::number(it);
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

int GobangModel::calculateLineMask(vector<int> line) {
    int blackCount = 0, whiteCount = 0;
    for (auto item: line) {
        if (item == 1) {
            blackCount++;
        } else if (item == -1) {
            whiteCount++;
        }
    }

    // 计算黑子得分（负分）
    int blackMask = 0, whiteMask = 0;
    for (auto item: maskTable) {
        if (blackCount >= item.stoneNum) {
            blackMask += isContainType(line, item.typeVec) ? item.mask : 0;
        }
    }
    // 计算白子得分（正分）
    for (auto &item: line) item = -item; // 反转棋子
    for (auto item: maskTable) {
        if (whiteCount >= item.stoneNum) {
            whiteMask += isContainType(line, item.typeVec) ? item.mask : 0;
        }
    }

    return whiteMask - blackMask;
}

int GobangModel::calculateMapMask(vector<vector<int> > board) {
    int allMask = 0;
    vector<int> line;

    // 横向
    for (int row = 0; row < boardWidth; row++) {
        allMask += calculateLineMask(gameBoard[row]);
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
        maskSort(make_pair(index, mask));
    }

    if (status.stoneType == 1)
        std::sort(maskSort.begin(), maskSort.end(),
                  [] (pair<int, int> v1, pair<int, int> v2) {
            return v1.second > v2.second;
        });
    else
        std::sort(maskSort.begin(), maskSort.end(),
                  [] (pair<int, int> v1, pair<int, int> v2) {
            return v1.second < v2.second;
        });

    for (auto item: maskSort) status.clickVec.push_back(clickVec[item.first]);

    return maskSort[0].second;
}

bool GobangModel::getNewStatus(Status &oldStatus, Status &newStatus) {
    if (oldStatus.nowIndex == (int)oldStatus.clickVec.size()) return false;

    vector<vector<int> > newBoard = oldStatus.board;
    int row = oldStatus.clickVec[oldStatus.nowIndex].first;
    int col = oldStatus.clickVec[oldStatus.nowIndex].second;
    newBoard[row][col] = oldStatus.stoneType;
    oldStatus.nowIndex++;

    newStatus = Status(-oldStatus.stoneType, newBoard);
    return true;
}

pair<int, int> GobangModel::alphaBetaCut(vector<vector<int> > board) {
    int level = 2;
    int alpha = -10000000, beta = 10000000;
    vector<pair<int, int> > bestClickVec;

    Status rootStatus(-1, board);

    stack<Status> statusStack;
    statusStack.push(rootStatus);

    int alphaCut = 0, betaCut = 0;
    while (!statusStack.empty()) {
        int stackSize = statusStack.size();
        Status nowStatus = statusStack.top();
        nowStatus.nodeMask = stackSize == level ?
                    calculateMapMask(nowStatus.board) : nowStatus.nodeMask;
        int nowMask = nowStatus.nodeMask;

        // 获取新状态，深度遍历
        Status newStatus;
        if (stackSize != level && getNewStatus(nowStatus, newStatus)) {
            statusStack.pop();
            statusStack.push(nowStatus);
            statusStack.push(newStatus);
            continue;
        }

        // 递归退栈规则
        statusStack.pop();
        Status parent = statusStack.top();
        switch (statusStack.size() + 1) {
        case 5:
            // alpha剪枝
            if (alpha != -10000000 && nowMask <= alpha) {
                statusStack.pop();
                alphaCut++;
                continue;
            }
            beta = nowMask < beta ? nowMask : beta;
            parent.nodeMask = beta;
            break;
        case 4:
            // beta剪枝
            if (beta != 10000000 && nowMask >= beta) {
                statusStack.pop();
                betaCut++;
                continue;
            }
            alpha = nowMask > alpha ? nowMask : alpha;
            parent.nodeMask = alpha;
            break;
        case 3:
            // alpha剪枝
            if (alpha != -10000000 && nowMask <= alpha) {
                statusStack.pop();
                if (nowMask == alpha) {
                    Status root = statusStack.top();
                    bestClickVec.push_back(root.clickVec[parent.nowIndex - 1]);
                }
                alphaCut++;
                continue;
            }
            beta = nowMask < beta ? nowMask : beta;
            parent.nodeMask = beta;
            break;
        case 2:
            if (nowMask == alpha) {
                bestClickVec.push_back(parent.clickVec[parent.nowIndex - 1]);
            } else if (nowMask > alpha) {
                alpha = nowMask;
                parent.nodeMask = alpha;
                bestClickVec.clear();
                bestClickVec.push_back(parent.clickVec[parent.nowIndex - 1]);
            }
            break;
        case 1:
            statusStack.pop();
            continue;
        }

        statusStack.pop();
        statusStack.push(parent);
    }

    //srand(time(NULL));
    //return bestClickVec[rand() % bestClickVec.size()];
    return bestClickVec[0];
}
