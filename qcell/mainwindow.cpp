#include <time.h>
#include <QPainter>
#include <QTimer>
#include <QResizeEvent>
#include <QStyleOption>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    srand(static_cast<unsigned>(time(nullptr)));
    resize(1239, 720);
    setStyleSheet("background-color:black;");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start(200);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onTimer()
{
    if (!m_Freeze)
    {
        m_CellMgr.update();
    }
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    auto btn = event->button();
    if (btn != Qt::RightButton)
        return;

    int x = event->x();
    int y = event->y();

    int rand_max = cell_size * 8;
    for (int i = 0; i < 32; i++)
    {
        int dx = rand() % (rand_max * 2) - rand_max;
        int dy = rand() % (rand_max * 2) - rand_max;
        auto* cell = getCellByPoint(x + dx, y + dy);
        if (cell)
        {
            cell->alive = true;
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();

    m_Width = size.width();
    m_Height = size.height();

    int xcount = (m_Width - padding * 2) / cell_size;
    int ycount = (m_Height - padding * 2) / cell_size;

    m_CellMgr.reset(xcount, ycount);

    m_Grids.clear();

    for (int x = 0; x <= xcount; x++)
    {
        m_Grids.push_back({padding + cell_size * x, padding});
        m_Grids.push_back({padding + cell_size * x, padding + cell_size * ycount});
    }

    for (int y = 0; y <= ycount; y++)
    {
        m_Grids.push_back({padding, padding + cell_size * y});
        m_Grids.push_back({padding + cell_size * xcount, padding + cell_size * y});
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    int key = event->key();
    if (key == Qt::Key_Escape)
    {
        m_Freeze = !m_Freeze;
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setPen(QPen(m_Freeze ? QColor(80, 80, 0) : QColor(50, 50, 50), 1));
    painter.drawLines(m_Grids);

    QBrush brush(m_Freeze ? QColor(100, 100, 100) : QColor(0, 120, 0));

    int xcount = m_CellMgr.getWidth();
    int ycount = m_CellMgr.getHeight();
    for (int y = 0; y < ycount; y++)
    {
        for (int x = 0; x < xcount; x++)
        {
            auto& cell = m_CellMgr.getCell(x, y);
            if (cell.alive)
            {
                painter.fillRect(padding + cell_size * x + 1, padding + cell_size * y + 1, cell_size - 2, cell_size - 2, brush);
            }
        }
    }
}

CellBox* MainWindow::getCellByPoint(int x, int y)
{
    if (x < padding || y < padding)
        return nullptr;

    x = (x - padding) / cell_size;
    y = (y - padding) / cell_size;

    if (x >= m_CellMgr.getWidth() || y >= m_CellMgr.getHeight())
        return nullptr;

    return &m_CellMgr.getCell(x, y);
}
