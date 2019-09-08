#include "gobangui.h"
#include "ui_gobangui.h"
#include "windows.h"

GobangUI::GobangUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GobangUI)
{
    ui->setupUi(this);
    this->setMaximumSize(750, 604);
    this->setMinimumSize(750, 604);
    this->setWindowTitle("Gobang Game");
    this->setWindowIcon(QIcon("://icon/gobang.png"));

    drawOrigin = QPoint(0, 0);
    boardOrigin = QPoint(drawOrigin.rx() + 24, drawOrigin.ry() + 24);

    ui->btn_pvp->setIcon(QIcon("://icon/pvp.png"));
    ui->btn_pvc->setIcon(QIcon("://icon/pvc.png"));
    ui->btn_reset->setIcon(QIcon("://icon/reset.png"));
    ui->btn_exit->setIcon(QIcon("://icon/exit.png"));
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
    painter.setBrush(QColor("#ffffff"));
    painter.drawRect(0, 0, 750, 604);
    painter.drawImage(drawOrigin, boardImg);

    // 行棋方表示
    switch (gobangModel.nowPlayer) {
    case GobangModel::BLACK: painter.setBrush(QColor("#000000")); break;
    case GobangModel::WHITE: painter.setBrush(QColor("#ffffff")); break;
    }
    painter.drawEllipse(QRect(660, 180, 2*stoneRadius, 2*stoneRadius));

    // 棋子绘制
    if (gobangModel.gameStatus != GobangModel::READY) {
        for (int row = 0; row < boardWidth; row++) {
            for (int col = 0; col < boardWidth; col++) {
                QRect rect(boardOrigin.rx() + (2*col-1)* stoneRadius,
                           boardOrigin.ry() + (2*row-1)* stoneRadius,
                           2*stoneRadius - 4, 2*stoneRadius - 4);
                switch (gobangModel.gameBoard[row][col]) {
                case 0: break;
                case 1: // 黑棋
                    painter.setBrush(QColor("#000000"));
                    painter.drawEllipse(rect);
                    break;
                case -1: // 白棋
                    painter.setBrush(QColor("#ffffff"));
                    painter.drawEllipse(rect);
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
            switch (gobangModel.gameMode) {
            case GobangModel::PVP:
                gobangModel.gameOneStep(row, col, gobangModel.nowPlayer);
                repaint();
                Sleep(500);
                break;

            case GobangModel::PVC:
                gobangModel.gameOneStep(row, col, GobangModel::BLACK);
                repaint();
                gobangModel.aiDropStone();
                repaint();
                break;
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
    QMessageBox::information(this, "", "PVP GAME START! PLEASE DROP STONE!");
    repaint();
}

void GobangUI::on_btn_pvc_clicked()
{
    gobangModel.startPVC();
    QMessageBox::information(this, "", "PVC GAME START! PLEASE DROP STONE!");
    repaint();
}

void GobangUI::on_btn_reset_clicked()
{
    switch (gobangModel.gameMode) {
    case GobangModel::PVP:
        gobangModel.startPVP();
        QMessageBox::information(this, "", "PVP GAME START! PLEASE DROP STONE!");
        break;
    case GobangModel::PVC:
        gobangModel.startPVC();
        QMessageBox::information(this, "", "PVC GAME START! PLEASE DROP STONE!");
        break;
    }

    repaint();
}

void GobangUI::on_btn_exit_clicked()
{
    this->close();
}
