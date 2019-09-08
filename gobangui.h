#ifndef GOBANGUI_H
#define GOBANGUI_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include <QPoint>

#include "gobangmodel.h"

namespace Ui {
class GobangUI;
}

class GobangUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit GobangUI(QWidget *parent = 0);
    ~GobangUI();

private slots:
    void on_btn_pvp_clicked();

    void on_btn_pvc_clicked();

private:
    const int boardWidth = 15;
    GobangModel gobangModel;

    QPoint drawOrigin;
    QPoint boardOrigin;
    int stoneRadius = 20;

    QImage boardImg;
    QImage blackStoneImg;
    QImage whiteStoneImg;

    Ui::GobangUI *ui;
    void paintEvent(QPaintEvent *event);
    bool getClickRowCol(QPoint clickPos, int &row, int &col);
    void mousePressEvent(QMouseEvent *event);
    void checkWinDead();
};

#endif // GOBANGUI_H
