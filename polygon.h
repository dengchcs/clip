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

constexpr index_t null_ind = {-1,-1};

class IntrPoint : public QPoint {
public:
    bool is_enter;
    bool is_intr{false};
    index_t next_src, next_win;
    IntrPoint(const QPoint& p, bool enter, bool intr, index_t nsrc, index_t nwin) : QPoint(p) {
        is_enter = enter;
        is_intr = intr;
        next_src = nsrc;
        next_win = nwin;
    }
};
using intrs_t = std::vector<IntrPoint>;


enum class PointType {
    Out,
    In,
    Vert,
    Unknown
};

/**
 * @brief 用于WA算法中存储顶点和交点
 */
class MixPoint : public QPoint {
public:
    PointType e_type;
    int ind_other;
    MixPoint(const QPoint& p, PointType type, int ind) : QPoint(p) {
        e_type = type;
        ind_other = ind;
    }
    MixPoint(const IntrPoint& intr) : QPoint(intr) {
        e_type = PointType::Unknown;
        ind_other = -1;
    }
    bool same_as(const MixPoint& other) const {
        return x() == other.x() && y() == other.y();
    }
};
using mixpts_t = std::vector<MixPoint>;

/**
 * @brief 使用射线法判断点是否在多边形内
 */
bool inside(const QPoint& point, const points_t& poly);

/**
 * @brief 点是否在多边形内, 要求多边形已排好序
 */
bool inside(const QPoint& point, const polys_t& polys);

/**
 * @brief 环是否在多边形内, 要求多边形已排好序
 */
bool inside(const points_t& points, const polys_t& polys);

/**
 * @brief 环是否是顺时针方向的
 */
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
 * @brief 线和环是否相交
 */
bool has_intr(const QPoint& p1, const QPoint& p2, const points_t& poly);

/**
 * @brief 环和环是否相交
 */
bool has_intr(const points_t& points, const points_t& poly);

/**
 * @brief 环和多边形是否相交
 */
bool has_intr(const points_t& points, const polys_t& polys);

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

/**
 * @brief 将交点和多边形顶点按序排列,生成列表
 * @param polys 主/裁剪多边形
 * @param is_src 是否是主多边形
 * @param intrs 主多边形和裁剪多边形的交点列表
 * @return
 */
mixpts_t intr_list(const polys_t& polys, bool is_src, const intrs_t& intrs);

/**
 * @brief 设置求得的顶点表中的交点出入位
 * @param win 裁剪多边形
 * @param lisrc 主多边形顶点表
 * @param liwin 裁剪多边形顶点表
 */
void set_enter_flag(const polys_t& win, mixpts_t& lisrc, mixpts_t& liwin);

/**
 * @brief 在两个已经列表间标记交点在对方列表中的下标
 * @param lisrc
 * @param liwin
 */
void link(mixpts_t& lisrc, mixpts_t& liwin);

std::vector<mixpts_t> weiler_atherton(polys_t& win, polys_t& src);

#endif // POLYGON_H
