#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QMainWindow>
#include "cell_mgr.h"

namespace Ui {
class MainWindow;
}

const int cell_size = 10;
const int padding = 10;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onTimer();

private:
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *) override;

    CellBox* getCellByPoint(int x, int y);

private:
    int m_Width = 0;
    int m_Height = 0;
    bool m_Freeze = false;
    CellMgr m_CellMgr;
    QVector<QPoint> m_Grids;
};

#endif // MAINWINDOW_H
