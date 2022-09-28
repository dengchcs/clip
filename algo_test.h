#ifndef ALGOTEST_H
#define ALGOTEST_H

#include "polygon.h"
#include <QtTest/QtTest>

class AlgoTest : public QObject
{
    Q_OBJECT
private slots:
    void demoTest() {
        QVERIFY(true);
        qDebug() << "demo test\n";
    }

    void testInside() {
        points_t poly = {{0, 0}, {0, 2}, {2, 2}, {1, 0}};
        QVERIFY(inside({1, 1}, poly));
        QVERIFY(!inside({2,0}, poly));
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
};

#endif // ALGOTEST_H
