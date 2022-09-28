#include "polygon.h"

bool inside(const QPoint& point, const points_t& poly) {
    int cnt = 0;
    const std::size_t size = poly.size();
    for (std::size_t i = 0; i < poly.size(); i++) {
        auto p1 = poly[i], p2 = poly[(i+1)%size];
        if (p1.y() == p2.y()) continue;
        if (point.y() < std::min(p1.y(), p2.y())) continue;
        if (point.y() >= std::max(p1.y(), p2.y())) continue;
        double intrx = p1.x() + (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y() + 0.0);
        if (intrx > point.x()) cnt++;
    }
    return (cnt % 2) == 1;
}

bool is_clockwise(const points_t& poly) {
    if (poly.size() < 3) {
        return false;
    }
    int sum = 0;
    for (std::size_t i = 0; i + 1 < poly.size(); i++) {
        sum += (poly[i+1].x() - poly[i].x()) * (poly[i+1].y() + poly[i].y());
    }
    sum += (poly[0].x() - poly.back().x()) * (poly[0].y() + poly.back().y());
    return sum > 0;
}

void sort_poly(polys_t& polys) {
    if (polys.empty()) return;
    std::size_t index = 0;
    for (std::size_t i = 1; i < polys.size(); i++) {
        if (inside(polys[0][0], polys[i])) {
            index = i;
            break;
        }
    }
    std::swap(polys[0], polys[index]);
    if (is_clockwise(polys[0])) std::reverse(polys[0].begin() + 1, polys[0].end());
    for (std::size_t i = 1; i < polys.size(); i++) {
        if (! is_clockwise(polys[i])) std::reverse(polys[i].begin() + 1, polys[i].end());
    }
}

double has_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4) {
    const int x1 = p1.x(), x2 = p2.x(), x3 = p3.x(), x4 = p4.x();
    const int y1 = p1.y(), y2 = p2.y(), y3 = p3.y(), y4 = p4.y();
    int fenmu = (x2-x1)*(y4-y3) - (x4-x3)*(y2-y1);
    if (fenmu == 0) return -1;
    int tfenzi = (x3-x1)*(y4-y3) - (x4-x3)*(y3-y1);
    double t = tfenzi / (fenmu + 0.0);
    if (t < 0 || t > 1) return -1;
    int ufenzi = (x3-x1)*(y2-y1) - (x2-x1)*(y3-y1);
    double u = ufenzi / (fenmu + 0.0);
    if (u < 0 || u > 1) return -1;
    return t;
}

std::optional<QPoint> line_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4) {
    const double t = has_intr(p1, p2, p3, p4);
    if (t < -0.5) return std::nullopt;
    else return p1 + t * (p2 - p1);
}

intrs_t polys_interset(const polys_t& src, const polys_t& win) {
    intrs_t result;
    const auto nrsrc = src.size(), nrwin = win.size();    // 环的个数
    if (nrsrc == 0 || nrwin == 0) return result;
    // 下标 i:主多边形环, j:裁剪多边形环, k:主多边形环顶点, m:裁剪多边形环顶点
    // 数量 nr:环的数量 np:当前环的顶点数量
    for (std::size_t i = 0; i < nrsrc; i++) {
        const auto npsrc = src[i].size();    // 环的顶点数量
        for (std::size_t j = 0; j < nrwin; j++) {
            const auto npwin = win[j].size();    // 环的顶点数量
            for (std::size_t k = 0; k < npsrc; k++) {
                const auto knext = (k + 1) % npsrc;
                const auto p1 = src[i][k], p2 = src[i][knext];
                for (std::size_t m = 0; m < npwin; m++) {
                    const auto mnext = (m + 1) % npwin;
                    const auto p3 = win[j][m], p4 = win[j][mnext];
                    const auto point = line_intr(p1, p2, p3, p4);
                    if (! point.has_value()) continue;
                    const index_t next_src = {(int)i, (int)knext};
                    const index_t next_win = {(int)j, (int)mnext};
                    // todo: 判断是否为入点
                    IntrPoint intr(point.value(), false, next_src, next_win);
                    result.push_back(intr);
                }
            }
        }
    }
    return result;
}
