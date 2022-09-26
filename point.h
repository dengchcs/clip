#ifndef POINT_H
#define POINT_H

#include <cmath>
struct point {
    int x;
    int y;
    double distance(const point& p) const {
        double dx = x - p.x, dy = y - p.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

point operator+(const point& p1, const point& p2) {
    return point{p1.x + p2.x, p1.y + p2.y};
}

bool operator==(const point& p1, const point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

#endif // POINT_H
