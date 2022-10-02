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
            msg->setText("已绘制裁剪多边形");
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
        update();
    });
    setFixedSize(800, 800);
    e_poly = PolyType::Window;
    setWindowTitle("左键添加点; 右键闭合环");
    ui->colorText->setText("主多边形: 红色; 裁剪多边形: 绿色; 结果: 黑色");
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
    auto draw_poly = [&](const points_t& points, bool close) {
        for (auto &&p : points) {
            QRect rect(p.x(), p.y(), 100, 20);
            painter.drawText(rect, "(" + QString::number(p.x()) + "," + QString::number(p.y()) + ")");
        }
        const auto npoints = points.size();
        if (npoints > 0) {
            QRect rect(points.front().x() - 20, points.front().y(), 100, 20);
            painter.drawText(rect, "起点");
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
            QRect rect(p.x(), p.y(), 100, 20);
            painter.drawText(rect, p.e_type == PointType::In ? "IN" : "OUT");
        }
    }
}
