#include <QtTest>

#include <iostream>

#include <axis.h>
#include <table.h>

// add necessary includes here

class AdvancedViewsTest : public QObject
{
    Q_OBJECT

public:
    AdvancedViewsTest();
    ~AdvancedViewsTest();

private slots:
    void testAxisAppend();
    void testAxisGet();
    void testAxisVisualGet();
    void testAxisLength();
    void testAxisVisualLength();
    void testAxisRemoveAt();
    void testAxisVisualRemoveAt();

    void testTableBoundingRect();
    void testTableCellsInRect();
};

AdvancedViewsTest::AdvancedViewsTest()
{}

AdvancedViewsTest::~AdvancedViewsTest()
{}

void AdvancedViewsTest::testAxisAppend()
{
    Axis axis;
    axis.append(100);
    QVERIFY(axis.m_ranges.size() == 1);
    QCOMPARE(axis.m_ranges[0], Range(1, 100));
    axis.append(100);
    QVERIFY(axis.m_ranges.size() == 1);
    QCOMPARE(axis.m_ranges[0], Range(2, 100));
    axis.append(50);
    QVERIFY(axis.m_ranges.size() == 2);
    QCOMPARE(axis.m_ranges[0], Range(2, 100));
    QCOMPARE(axis.m_ranges[1], Range(1, 50));
}

void AdvancedViewsTest::testAxisGet()
{
    Axis axis;
    axis.append(100);
    axis.append(100);
    axis.append(50);
    QVERIFY(axis.get(0));
    QVERIFY(*axis.get(0) == AxisGetResult(0, 0, 100));
    QVERIFY(axis.get(1));
    QVERIFY(axis.get(1) == AxisGetResult(1, 100, 100));
    QVERIFY(axis.get(2));
    QVERIFY(axis.get(2) == AxisGetResult(2, 200, 50));
    QVERIFY(!axis.get(3));
    QVERIFY(!axis.get(-1));
}

void AdvancedViewsTest::testAxisVisualGet()
{
    Axis axis;
    axis.append(100);
    axis.append(100);
    axis.append(50);
    QVERIFY(axis.visualGet(0) == AxisGetResult(0, 0, 100));
    QVERIFY(axis.visualGet(50) == AxisGetResult(0, 0, 100));
    QVERIFY(axis.visualGet(100) == AxisGetResult(1, 100, 100));
    QVERIFY(axis.visualGet(150) == AxisGetResult(1, 100, 100));
    QVERIFY(axis.visualGet(200) == AxisGetResult(2, 200, 50));
    QVERIFY(axis.visualGet(225) == AxisGetResult(2, 200, 50));
    QVERIFY(!axis.visualGet(250));
}

void AdvancedViewsTest::testAxisLength()
{
    Axis axis;
    QCOMPARE(axis.length(), 0);
    axis.append(100);
    QCOMPARE(axis.length(), 1);
    axis.append(50);
    QCOMPARE(axis.length(), 2);
    axis.append(100);
    QCOMPARE(axis.length(), 3);
    axis.append(100);
    QCOMPARE(axis.length(), 4);
}

void AdvancedViewsTest::testAxisVisualLength()
{
    Axis axis;
    QCOMPARE(axis.visualLength(), 0);
    axis.append(100);
    QCOMPARE(axis.visualLength(), 100);
    axis.append(50);
    QCOMPARE(axis.visualLength(), 150);
    axis.append(100);
    QCOMPARE(axis.visualLength(), 250);
    axis.append(100);
    QCOMPARE(axis.visualLength(), 350);
}

void AdvancedViewsTest::testAxisRemoveAt()
{
    Axis axis;
    axis.append(100);
    axis.append(50);
    axis.append(100);
    axis.append(100);

    std::vector<Range> test;
    test = {Range(1, 100), Range(1, 50), Range(2, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(!axis.removeAt(-1));
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(!axis.removeAt(4));
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.removeAt(3));
    test = {Range(1, 100), Range(1, 50), Range(1, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.removeAt(1));
    test = {Range(2, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.removeAt(1));
    test = {Range(1, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.removeAt(0));
    test = {};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));
}

void AdvancedViewsTest::testAxisVisualRemoveAt()
{
    Axis axis;
    axis.append(100);
    axis.append(50);
    axis.append(100);
    axis.append(100);

    std::vector<Range> test;
    test = {Range(1, 100), Range(1, 50), Range(2, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(!axis.visualRemoveAt(-1));
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(!axis.visualRemoveAt(350));
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.visualRemoveAt(250));
    test = {Range(1, 100), Range(1, 50), Range(1, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.visualRemoveAt(100));
    test = {Range(2, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.visualRemoveAt(100));
    test = {Range(1, 100)};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));

    QVERIFY(axis.visualRemoveAt(0));
    test = {};
    QVERIFY(std::equal(axis.m_ranges.begin(), axis.m_ranges.end(), test.begin()));
}

void AdvancedViewsTest::testTableBoundingRect()
{
    Table table;
    QCOMPARE(table.boundingRect(), QRect(0, 0, 0, 0));
    table.m_xAxis.append(100);
    table.m_yAxis.append(50);
    QCOMPARE(table.boundingRect(), QRect(0, 0, 100, 50));
}

void AdvancedViewsTest::testTableCellsInRect()
{
    Table table;
    table.m_xAxis.append(100);
    table.m_xAxis.append(100);
    table.m_yAxis.append(50);
    table.m_yAxis.append(50);
    table.m_yAxis.append(50);

    std::vector<Cell> test;
    std::vector<Cell> cells;

    QCOMPARE(table.boundingRect(), QRect(0, 0, 200, 150));
    cells = table.cellsInVisualRect(QRect(0, 0, 200, 150));
    test = {Cell(0, 0, QRect(0, 0, 100, 50)), Cell(0, 1, QRect(100, 0, 100, 50)),
            Cell(1, 0, QRect(0, 50, 100, 50)), Cell(1, 1, QRect(100, 50, 100, 50)),
            Cell(2, 0, QRect(0, 100, 100, 50)), Cell(2, 1, QRect(100, 100, 100, 50))};
    QVERIFY(std::equal(cells.begin(), cells.end(), test.begin()));

    cells = table.cellsInVisualRect(QRect(-20, -20, 220, 170));
    test = {Cell(0, 0, QRect(0, 0, 100, 50)), Cell(0, 1, QRect(100, 0, 100, 50)),
            Cell(1, 0, QRect(0, 50, 100, 50)), Cell(1, 1, QRect(100, 50, 100, 50)),
            Cell(2, 0, QRect(0, 100, 100, 50)), Cell(2, 1, QRect(100, 100, 100, 50))};
    QVERIFY(std::equal(cells.begin(), cells.end(), test.begin()));

    cells = table.cellsInVisualRect(QRect(10, 10, 190, 140));
    test = {Cell(0, 0, QRect(0, 0, 100, 50)), Cell(0, 1, QRect(100, 0, 100, 50)),
            Cell(1, 0, QRect(0, 50, 100, 50)), Cell(1, 1, QRect(100, 50, 100, 50)),
            Cell(2, 0, QRect(0, 100, 100, 50)), Cell(2, 1, QRect(100, 100, 100, 50))};
    QVERIFY(std::equal(cells.begin(), cells.end(), test.begin()));

    cells = table.cellsInVisualRect(QRect(25, 25, 100, 100));
    test = {Cell(0, 0, QRect(0, 0, 100, 50)), Cell(0, 1, QRect(100, 0, 100, 50)),
            Cell(1, 0, QRect(0, 50, 100, 50)), Cell(1, 1, QRect(100, 50, 100, 50))};
    QVERIFY(std::equal(cells.begin(), cells.end(), test.begin()));
}

QTEST_APPLESS_MAIN(AdvancedViewsTest)

#include "tst_advancedviews.moc"