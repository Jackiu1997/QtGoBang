#include "gobangui.h"
#include "ui_gobangui.h"
#include "windows.h"

GobangUI::GobangUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GobangUI)
{
    ui->setupUi(this);
    this->setMaximumSize(740, 604);
    this->setMinimumSize(740, 604);
    this->setWindowTitle("Gobang Game");
    this->setWindowIcon(QIcon("://icon/gobang.png"));

    drawOrigin = QPoint(0, 0);
    boardOrigin = QPoint(drawOrigin.rx() + 24, drawOrigin.ry() + 24);

    boardImg = QImage("://img/board.png");
}

GobangUI::~GobangUI()
{
    delete ui;
}

void GobangUI::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawImage(drawOrigin, boardImg);

    if (gobangModel.gameStatus != GobangModel::READY) {
        for (int row = 0; row < boardWidth; row++) {
            for (int col = 0; col < boardWidth; col++) {
                QRect rect(boardOrigin.rx() + (2*col-1)* stoneRadius,
                           boardOrigin.ry() + (2*row-1)* stoneRadius,
                           2*stoneRadius - 4, 2*stoneRadius - 4);
                switch (gobangModel.gameBoard[row][col]) {
                case 0:
                    break;
                case 1: // 黑棋
                    painter.setBrush(QColor("#000000"));
                    painter.drawEllipse(rect);
                    break;
                case -1: // 白棋
                    painter.setBrush(QColor("#ffffff"));
                    painter.drawEllipse(rect);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void GobangUI::mousePressEvent(QMouseEvent *event)
{
    if (gobangModel.gameStatus == GobangModel::PLAYING) {
        QPoint clickPos = event->pos();
        // 左键点击
        int row = -1, col = -1;
        bool clickArea = getClickRowCol(clickPos, row, col);

        if (clickArea) {
            gobangModel.placeStone(row, col);
            repaint();
            Sleep(500);

            if (gobangModel.gameMode == GobangModel::PVC) {
                gobangModel.aiDropStone();
                repaint();
            }
        }
    }

    checkWinDead();
}

bool GobangUI::getClickRowCol(QPoint clickPos, int &row, int &col) {
    col = (clickPos.rx() - boardOrigin.rx() + stoneRadius) / (2*stoneRadius);
    row = (clickPos.ry() - boardOrigin.ry() + stoneRadius) / (2*stoneRadius);

    if (row >=0 && row < boardWidth && col >= 0 && col < boardWidth) {
        return true;
    } else return false;
}

void GobangUI::checkWinDead() {
    if (gobangModel.gameStatus == GobangModel::WIN) {
        switch (gobangModel.nowPlayer) {
        case GobangModel::BLACK:
            QMessageBox::information(this, "", "White Win The Game!");
            break;
        case GobangModel::WHITE:
            QMessageBox::information(this, "", "Black Win The Game!");
            break;
        }
    } else if (gobangModel.gameStatus == GobangModel::DEAD) {
        QMessageBox::information(this, "", "It Is A Dead Game!");
    }
}

void GobangUI::on_btn_pvp_clicked()
{
    gobangModel.startPVP();
    repaint();
}

void GobangUI::on_btn_pvc_clicked()
{
    gobangModel.startPVC();
    repaint();
}
