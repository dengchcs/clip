#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMouseEvent>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum PolyType {
    PolySource,
    PolyWindow
};

constexpr auto src_color = Qt::red;
constexpr auto win_color = Qt::green;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
private:
    using polys_t = std::vector<std::vector<QPoint>>;
    using points_t = std::vector<QPoint>;
    Ui::MainWindow *ui;
    points_t buf_points;             // 还未闭合的一个环
    polys_t source;    // 已经闭合的主多边形的环
    polys_t window;    // 已经闭合的裁剪多边形的环
    PolyType e_poly;
};
#endif // MAINWINDOW_H
