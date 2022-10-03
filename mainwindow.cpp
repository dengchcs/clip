#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QPainter>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->swithBtn, &QPushButton::clicked, this, [&](){
        auto msg = new QMessageBox(this);
        if (buf_points.empty()) {
            if (e_poly == PolyType::Source) {
                e_poly = PolyType::Window;
                msg->setText("已转换为裁剪多边形输入模式");
            } else {
                e_poly = PolyType::Source;
                msg->setText("已转换为主多边形输入模式");
            }
        } else {
            msg->setText("请先闭合多边形");
        }
        msg->setStandardButtons(QMessageBox::Ok);
        msg->exec();
    });
    connect(ui->doIntrBtn, &QPushButton::clicked, this, [&](){
        auto msg = new QMessageBox(this);
        if (buf_points.empty()) {
            intrpoly = weiler_atherton(window, source);
            update();
            msg->setText("已绘制裁剪结果");
        } else {
            msg->setText("请先闭合多边形");
        }
        msg->setStandardButtons(QMessageBox::Ok);
        msg->exec();
    });
    connect(ui->clearBtn, &QPushButton::clicked, this, [&]() {
        source.clear();
        window.clear();
        buf_points.clear();
        intrpoly.clear();
        e_poly = type_default;
        update();
    });
    setFixedSize(800, 800);
    e_poly = type_default;
    setWindowTitle("左键添加点; 右键闭合环");
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
            if (e_poly == PolyType::Source) {
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

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    auto set_painter = [&](const auto color, int width = 2) {
        QPen pen;
        pen.setColor(color);
        pen.setWidth(width);
        painter.setPen(pen);
    };

    // 绘制三种多边形的颜色图示
    auto draw_legend = [&](int startx, const char *name) {
        painter.drawLine(QPoint{startx, 30}, {startx + 20, 30});
        painter.drawText(QPoint{startx + 25, 35}, name);
    };
    set_painter(src_color, 4);
    draw_legend(400, "主多边形");
    set_painter(win_color, 4);
    draw_legend(500, "裁剪多边形");
    set_painter(int_color, 4);
    draw_legend(600, "裁剪结果");

    // 画一个环, 可以选择是否闭合
    auto draw_poly = [&](const points_t& points, bool close) {
        for (auto &&p : points) {
            painter.drawText(p, "(" + QString::number(p.x()) + "," + QString::number(p.y()) + ")");
        }
        const auto npoints = points.size();
        if (npoints > 0) {
            painter.drawText(points.front() - QPoint{30, 0}, "起点");
        }
        for (std::size_t i = 0; i + 1 < npoints; i++) {
            painter.drawLine(points[i], points[i+1]);
        }
        if (close && npoints >= 3) {
            painter.drawLine(points.back(), points.front());
        }
    };
    set_painter(src_color);
    for (auto&& poly : source) {
        draw_poly(poly, true);
    }
    set_painter(win_color);
    for (auto&& poly : window) {
        draw_poly(poly, true);
    }
    set_painter(e_poly == PolyType::Source ? src_color : win_color);
    draw_poly(buf_points, false);

    // 绘制裁剪结果
    set_painter(int_color, 4);
    for (auto &&poly : intrpoly) {
        for (std::size_t i = 0; i < poly.size(); i++) {
            painter.drawLine(poly[i], poly[(i+1) % poly.size()]);
        }
    }
    set_painter(int_color, 1);
    for (auto &&poly : intrpoly) {
        for (auto &&p : poly) {
            if (p.e_type == PointType::Vert) continue;
            painter.drawText(p, p.e_type == PointType::In ? "IN" : "OUT");
        }
    }
}
