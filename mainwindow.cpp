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
        if (e_poly == PolyType::Source) {
            e_poly = PolyType::Window;
            msg->setText("已转换为裁剪多边形输入模式");
        } else {
            e_poly = PolyType::Source;
            msg->setText("已转换为主多边形输入模式");
        }
        msg->setStandardButtons(QMessageBox::Ok);
        msg->exec();
    });
    connect(ui->doIntrBtn, &QPushButton::clicked, this, [&](){
        intrpt = polys_interset(source, window);
        update();
        auto msg = new QMessageBox(this);
        msg->setText("已绘制裁剪多边形");
        msg->setStandardButtons(QMessageBox::Ok);
        msg->exec();
    });
    connect(ui->clearBtn, &QPushButton::clicked, this, [&]() {
        source.clear();
        window.clear();
        buf_points.clear();
        intrpt.clear();
        update();
    });
    setFixedSize(800, 800);
    e_poly = PolyType::Source;
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
    QPen pen;
    pen.setWidth(2);
    auto set_color = [&](const auto color) {
        pen.setColor(color);
        painter.setPen(pen);
    };
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
    set_color(src_color);
    for (std::size_t i = 0; i < source.size(); i++) {
        draw_poly(source[i], true);
    }
    set_color(win_color);
    for (std::size_t i = 0; i < window.size(); i++) {
        draw_poly(window[i], true);
    }
    set_color(e_poly == PolyType::Source ? src_color : win_color);
    draw_poly(buf_points, false);

    pen.setColor(int_color);
    pen.setWidth(8);
    painter.setPen(pen);
    for (auto &&p : intrpt) {
        painter.drawPoint(p);
    }
    pen.setWidth(2);
    set_color(int_color);
    for (auto &&p : intrpt) {
        QRect rect(p.x(), p.y(), 100, 20);
        painter.drawText(rect, "(" + QString::number(p.x()) + "," + QString::number(p.y()) + ")");
    }
}
