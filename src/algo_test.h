#ifndef ALGOTEST_H
#define ALGOTEST_H

#include "polygon.h"
#include <QtTest/QtTest>

class AlgoTest : public QObject
{
    Q_OBJECT
private slots:

    void testInside() {
        points_t poly = {{0, 0}, {0, 2}, {2, 2}, {1, 0}};
        QVERIFY(inside({1, 1}, poly));
        QVERIFY(!inside({2,0}, poly));
        poly = {{0, 0}, {1, 0}, {2, 2}, {0, 2}};
        QVERIFY(inside({1, 1}, poly));
        QVERIFY(! inside({2, 0}, poly));
    }

    void testIsClockwise() {
        QVERIFY(is_clockwise({{0, 0}, {0, 1}, {1, 1}}));
        QVERIFY(!is_clockwise({{0, 0}, {2, 0}, {2, 2}, {0, 2}}));
    }

    void testSortPoly() {
        points_t out = {{0,0}, {0,8}, {8,8}, {8,0}};
        points_t in1 = {{1,1},{1,2},{2,1}};
        points_t in2 = {{4,4},{4,6},{7,5},{5,5}};
        polys_t polys = {in2, in1, out};
        sort_poly(polys);
        points_t expected_out = {{0,0},{8,0},{8,8},{0,8}};
        QCOMPARE(polys[0], expected_out);
        for (std::size_t i = 1; i < polys.size(); i++) {
            QVERIFY(is_clockwise(polys[i]));
        }
    }

    void testLineIntr() {
        auto intr1 = line_intr({0,0},{0,2},{-1,1},{1,1});
        QCOMPARE(intr1.value(), QPoint(0, 1));
        auto intr2 = line_intr({0,0},{3,3},{-1,2},{3,0});
        QCOMPARE(intr2.value(), QPoint(1,1));
        auto intr3 = line_intr({0,0}, {0,1}, {0,2}, {2,2});
        QCOMPARE(intr3, std::nullopt);
        auto intr4 = line_intr({6,2}, {6,-2}, {4,0}, {0,0});
        QCOMPARE(intr4, std::nullopt);
    }

    void testPolyIntrNoInnerLoop() {
        polys_t win = {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}};
        polys_t src = {{{2,-2}, {2, 2}, {6, 2}, {6,-2}}};
        auto intrs = polys_interset(src, win);
        // 应该为 {(2,0), (4,2)}
        qDebug() << intrs;
    }

    void testPolyIntrWithInnerLoop() {
        polys_t win = {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}, {{1,1}, {1,3}, {3,3}, {3,1}}};
        polys_t src = {{{2,-2}, {2, 2}, {6, 2}, {6,-2}}};
        auto intrs = polys_interset(src, win);
        // 应为 {(2,0), (2,1), (3,2), (4,2)}
        qDebug() << intrs;
    }

    void testIntrList() {
        polys_t win = {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}};
        polys_t src = {{{2,-2}, {2, 2}, {6, 2}, {6,-2}}};
        auto intrs = polys_interset(src, win);
        auto lisrc = intr_list(src, true, intrs);
        auto liwin = intr_list(win, false, intrs);
        qDebug() << lisrc << liwin;
    }

    void testIntrListWithInnerLoop() {
        polys_t win = {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}, {{1,1}, {1,3}, {3,3}, {3,1}}};
        polys_t src = {{{2,-2}, {2, 2}, {6, 2}, {6,-2}}};
        auto intrs = polys_interset(src, win);
        auto lisrc = intr_list(src, true, intrs);
        auto liwin = intr_list(win, false, intrs);
        qDebug() << lisrc << liwin;
    }

    void testLink() {
        polys_t win = {{{0, 0}, {0, 4}, {4, 4}, {4, 0}}};
        polys_t src = {{{2,-2}, {2, 2}, {6, 2}, {6,-2}}};
        const auto intrs = polys_interset(src, win);
        auto lisrc = intr_list(src, true, intrs);
        auto liwin = intr_list(win, false, intrs);
        set_enter_flag(win, lisrc, liwin);
        link(lisrc, liwin);
        qDebug() << "wait!";
    }

    void testRingInsidePolygon() {
        const polys_t poly = {{{0,0}, {0,16}, {8,16}, {8,0}}, {{2,4}, {6,4}, {6,8}, {2,8}}};
        points_t ring = {{2,10}, {2,12}, {4,12}};
        QVERIFY(inside(ring, poly));
        ring = {{3,5}, {3,6}, {5,6}, {5,5}};
        QVERIFY(! inside(ring, poly));
        ring = {{3,7}, {3,9}, {5,9}, {5,7}};
        QVERIFY(! inside(ring, poly));
        ring = {{-2,-2},{-2,-1},{-1,-1},{-1,-2}};
        QVERIFY(! inside(ring, poly));
    }
};

#endif // ALGOTEST_H
