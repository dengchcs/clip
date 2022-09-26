#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QPainter>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(800, 800);
    e_poly = PolyType::PolySource;
    setWindowTitle("左键添加点; 右键闭合环; 双击在多边形/窗口间切换");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    const auto button = event->button();
    if (button == Qt::LeftButton) {
        buf_points.push_back(event->pos());
        update();
    } else if (button == Qt::RightButton) {
        if (buf_points.size() >= 3) {
            if (e_poly == PolySource) {
                source.push_back(buf_points);
            } else {
                window.push_back(buf_points);
            }
            buf_points.clear();
            update();
        } else {
            auto err_msg = new QMessageBox(this);
            err_msg->setWindowTitle("error");
            err_msg->setText("请至少输入三个点");
            err_msg->setStandardButtons(QMessageBox::Ok);
            err_msg->exec();
        }
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent*) {
    buf_points.pop_back();  // 双击会触发单击时添加点的功能, 这里undo一下
    auto msg = new QMessageBox(this);
    if (e_poly == PolySource) {
        e_poly = PolyWindow;
        msg->setText("已转换为裁剪多边形输入模式");
    } else {
        e_poly = PolySource;
        msg->setText("已转换为主多边形输入模式");
    }
    msg->setStandardButtons(QMessageBox::Ok);
    msg->exec();
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    auto draw_poly = [&](const points_t& points, bool close) {
        for (auto &&p : points) {
            QRect rect(p.x(), p.y(), 100, 20);
            painter.drawText(rect, "(" + QString::number(p.x()) + "," + QString::number(p.y()) + ")");
        }
        for (std::size_t i = 0; i + 1< points.size(); i++) {
            painter.drawLine(points[i], points[i+1]);
        }
        if (close && points.size() >= 3) {
            painter.drawLine(points.back(), points.front());
        }
    };
    painter.setPen(src_color);
    for (std::size_t i = 0; i < source.size(); i++) {
        draw_poly(source[i], true);
    }
    painter.setPen(win_color);
    for (std::size_t i = 0; i < window.size(); i++) {
        draw_poly(window[i], true);
    }
    painter.setPen(e_poly == PolySource ? src_color : win_color);
    draw_poly(buf_points, false);
}
