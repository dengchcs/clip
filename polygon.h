#ifndef POLYGON_H
#define POLYGON_H

#include <algorithm>
#include <optional>
#include <vector>
#include <QPoint>

using points_t = std::vector<QPoint>;
using polys_t = std::vector<points_t>;

// 顶点在多边形中的下标: 第loop个环、第vert个顶点
typedef struct {
    int loop;
    int vert;
} index_t;

class IntrPoint : public QPoint {
public:
    bool is_enter;
    index_t next_src, next_win;
    IntrPoint(QPoint p, bool enter, index_t nsrc, index_t nwin) : QPoint(p) {
        is_enter = enter;
        next_src = nsrc;
        next_win = nwin;
    }
};

using intrs_t = std::vector<IntrPoint>;


/**
 * @brief 使用射线法判断点是否在多边形内
 */
bool inside(const QPoint& point, const points_t& poly);

bool is_clockwise(const points_t& poly);

/**
 * @brief 对一个可能带内环的多边形的顶点排序. 外环逆时针, 内环顺时针. 第一个元素为外环
 *        不支持环相交的情况
 */
void sort_poly(polys_t& polys);

/**
 * @brief 判断两条线段是否有交点
 * @param p1 线段一端点1
 * @param p2 线段一端点2
 * @param p3 线段二端点1
 * @param p4 线段二端点2
 * @return -1表示无交点, 否则表示[0,1]间的线段一参数值
 */
double has_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);

/**
 * @brief 求交点. 不考虑边重合的情况
 * @param p1 线段一端点1
 * @param p2 线段一端点2
 * @param p3 线段二端点1
 * @param p4 线段二端点2
 * @return 若线段相交则返回交点,否则返回nullopt
 */
std::optional<QPoint> line_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);

/**
 * @brief 求两个多边形的交点
 * @param src 主多边形
 * @param win 裁剪多边形
 * @return
 */
intrs_t polys_interset(const polys_t& src, const polys_t& win);

#endif // POLYGON_H
