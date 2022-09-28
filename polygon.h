#ifndef POLYGON_H
#define POLYGON_H

#include <algorithm>
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
    IntrPoint(bool enter, index_t nsrc, index_t nwin) {
        is_enter = enter;
        next_src = nsrc;
        next_win = nwin;
    }
};

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
 */
bool has_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);

/**
 * @brief 求交点. 此函数不检查交点的存在性. 不考虑边重合的情况
 * @param p1 线段一端点1
 * @param p2 线段一端点2
 * @param p3 线段二端点1
 * @param p4 线段二端点2
 * @return 交点
 */
QPoint get_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4);

#endif // POLYGON_H
