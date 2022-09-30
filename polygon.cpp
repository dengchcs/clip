#include "polygon.h"

#include <stack>
#include <unordered_set>
#include <map>

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

bool inside(const QPoint& point, const polys_t& polys) {
    if (! inside(point, polys[0])) {
        return false;
    }
    for (std::size_t i = 1; i < polys.size(); i++) {
        if (inside(point, polys[i])) {
            return false;
        }
    }
    return true;
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
                    IntrPoint intr(point.value(), false, true, next_src, next_win);
                    result.push_back(intr);
                }
            }
        }
    }
    return result;
}

mixpts_t intr_list(const polys_t& polys, bool is_src, const intrs_t& intrs) {
    mixpts_t result;
    const auto nloop = polys.size();
    for (std::size_t i = 0; i < nloop; i++) {   // 环编号
        const auto nvert = polys[i].size();
        for (std::size_t j = 0; j < nvert; j++) {   // 顶点编号
            const auto jnext = (j + 1) % nvert;
            const auto p1 = polys[i][j];
            MixPoint vert{p1, PointType::Vert, -1};
            result.push_back(vert);
            intrs_t online;
            for (auto&& p : intrs) {
                bool on_src = is_src && p.next_src.loop == (int)i && p.next_src.vert == (int)jnext;
                bool on_win = !is_src && p.next_win.loop == (int)i && p.next_win.vert == (int)jnext;
                if (!(on_src || on_win)) continue;
                online.push_back(p);
            }
            std::sort(online.begin(), online.end(), [&](const QPoint& int1, const QPoint& int2){
                return (int1 - p1).manhattanLength() < (int2 - p1).manhattanLength();
            });
            for (auto&& p : online) {
                result.push_back(p);
            }
        }
        MixPoint vert{polys[i].front(), PointType::Vert, -1};
        result.push_back(vert);
    }
    return result;
}

void set_enter_flag(const polys_t& win, mixpts_t& lisrc, mixpts_t& liwin) {
    bool flag = false;  // 第一个点肯定是非交点, 所以这里的初值无所谓
    for (std::size_t i = 0; i < lisrc.size(); i++) {
        if (lisrc[i].e_type == PointType::Vert) {
            flag = inside(lisrc[i], win);
        } else {
            flag = !flag;
            lisrc[i].e_type = flag ? PointType::In : PointType::Out;
        }
    }

    for (std::size_t i = 0; i < liwin.size(); i++) {
        if (liwin[i].e_type == PointType::Vert) continue;
        for (std::size_t j = 0; j < lisrc.size(); j++) {
            if (lisrc[j].x() == liwin[i].x() && lisrc[j].y() == liwin[i].y()) {
                liwin[i].e_type = lisrc[j].e_type;
                break;
            }
        }
    }
}

void link(mixpts_t& lisrc, mixpts_t& liwin) {
    for (std::size_t i = 0; i < lisrc.size(); i++) {
        if (lisrc[i].e_type == PointType::Vert) continue;
        for (std::size_t j = 0; j < liwin.size(); j++) {
            if (liwin[j].e_type == PointType::Vert) continue;
            if (lisrc[i].same_as(liwin[j])) {
                lisrc[i].ind_other = j;
                liwin[j].ind_other = i;
                break;
            }
        }
    }
}

std::vector<mixpts_t> weiler_atherton(polys_t& win, polys_t& src) {
    sort_poly(win);
    sort_poly(src);
    const auto intrs = polys_interset(src, win);
    auto lisrc = intr_list(src, true, intrs);
    auto liwin = intr_list(win, false, intrs);
    set_enter_flag(win, lisrc, liwin);
    link(lisrc, liwin);

    std::map<std::size_t, std::size_t> start_src, start_win;    // 环在顶点表中起始/终止点的下标对应关系
    for (std::size_t i = 0; i < lisrc.size(); ) {
        const auto start = i;
        while(! lisrc[++i].same_as(lisrc[start]));
        start_src.insert({i, start});
        i++;
    }
    for (std::size_t i = 0; i < liwin.size(); ) {
        const auto start = i;
        while(! liwin[++i].same_as(liwin[start]));
        start_win.insert({i, start});
        i++;
    }
    std::unordered_set<std::size_t> unvisitied_src;
    for (std::size_t i = 0; i < lisrc.size(); i++) {
        if (lisrc[i].e_type != PointType::Vert) {
            unvisitied_src.insert(i);
        }
    }
    std::vector<mixpts_t> result;
    while(! unvisitied_src.empty()) {
        // 选取一个未处理的交点作为起点
        const auto index = *unvisitied_src.begin();
        auto cur_intr = lisrc[index];   // 从主多边形的某个交点出发

        bool cur_src = cur_intr.e_type == PointType::In;   // 是否在主多边形表中追踪
        std::size_t cur_ind = cur_src ? index : cur_intr.ind_other; // 在追踪多边形中的下标
        if (!cur_src) cur_intr.ind_other = index;
        mixpts_t poly;
        do {
            // 交点为入点 ==> 在主多边形表内跟踪
            // 交点为出点 ==> 在裁剪多边形表内跟踪
            // 直至跟踪到下一个交点
            unvisitied_src.erase(cur_src ? cur_ind : cur_intr.ind_other);
            poly.push_back(cur_intr);
            const mixpts_t& track = cur_intr.e_type == PointType::In ? lisrc : liwin;
            // 在主多边形上&是入点 || 在裁剪多边形上&是出点 ==> 继续在当前多边形上跟踪
            // 交点不可能位于顶点表的末尾, 所以不需要担心这里的+1
            std::size_t nextind = (cur_intr.e_type == PointType::In) == cur_src ? cur_ind + 1 : cur_intr.ind_other + 1;
            // 跟踪到顶点直接加入, 直至跟踪到又一个交点
            while (track[nextind].e_type == PointType::Vert) {
                poly.push_back(track[nextind]);
                if (cur_src && start_src.find(nextind) != start_src.end()) {
                    nextind = start_src.find(nextind)->second;
                } else if (!cur_src && start_win.find(nextind) != start_win.end()) {
                    nextind = start_win.find(nextind)->second;
                }
                nextind++;
            }
            if ((track[nextind].e_type == PointType::In) != cur_src) {  // 换一个多边形追踪
                cur_src = !cur_src;
                cur_ind = track[nextind].ind_other;
                cur_intr = cur_intr.e_type == PointType::In ? liwin[cur_ind] : lisrc[cur_ind];
            } else {
                cur_ind = nextind;
                cur_intr = track[nextind];
            }
        } while(! cur_intr.same_as(lisrc[index]));
        result.push_back(poly);
    }
    return result;
}
