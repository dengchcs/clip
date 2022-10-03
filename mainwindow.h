#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "polygon.h"

#include <QMainWindow>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class PolyType {
    Source,
    Window
};

constexpr auto src_color = Qt::red;
constexpr auto win_color = Qt::darkGreen;
constexpr auto int_color = Qt::black;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
private:
    Ui::MainWindow *ui;
    points_t buf_points;             // 还未闭合的一个环
    polys_t source;    // 已经闭合的主多边形的环
    polys_t window;    // 已经闭合的裁剪多边形的环
    std::vector<mixpts_t> intrpoly;
    PolyType e_poly;
};
#endif // MAINWINDOW_H
