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

bool has_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4) {
    // TODO
    return false;
}

QPoint get_intr(const QPoint& p1, const QPoint& p2, const QPoint& p3, const QPoint& p4) {
    // TODO
    return {};
}
