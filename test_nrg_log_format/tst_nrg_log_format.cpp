#include <QtCore/QString>
#include <QtTest/QtTest>

#include <nrg_log_format.h>

using namespace Log_viewer;

// Register our own types.
Q_DECLARE_METATYPE(QSharedPointer<Log_viewer::Log_item>)
Q_DECLARE_METATYPE(QSharedPointer<Log_viewer::NRG_log_item>)
Q_DECLARE_METATYPE(Log_viewer::Log_type)
Q_DECLARE_METATYPE(Log_viewer::Column_type)

class Test_nrg_log_format : public QObject
{
    Q_OBJECT

public:
    Test_nrg_log_format();

private Q_SLOTS:
    void test_match();
    void test_match_data();
    void test_shall_fail_match();
    void test_shall_fail_match_data();
    void test_create_log_item();
    void test_create_log_item_data();
    void test_log_columns();
    void test_log_columns_data();

    void initTestCase();
    void cleanupTestCase();

private:
    NRG_log_format* m_format;
};

Test_nrg_log_format::Test_nrg_log_format()
{
}

void Test_nrg_log_format::test_match()
{
    QFETCH(QString, Correct_log_format);
    QVERIFY2(m_format->match(Correct_log_format), "Failed to match line to format.");
}

void Test_nrg_log_format::test_match_data()
{
    QTest::addColumn<QString>("Correct_log_format");

    {
        QString line = "2010-09-08 12:06:12,079864\t";
        // add start + end of text
        line.insert(0, "\2");
        line.append("\3\r\n");
        QTest::newRow("Minimum required log text to match") << line;
    }
    {
        QString line = "2010-09-08 12:06:12,079864\tINFO\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)";
        line.insert(0, "\2");
        line.append("\3\r\n");
        QTest::newRow("Whole log line") << line;
    }
    {
        QString line = "2010-09-08 12:06:12,079864\tINFO\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)";
        line.insert(0, "\2");
        line.append("\3\r\n");
        line.append("this\t is some extra text\t that came afterwards.");
        QTest::newRow("Log line + extra") << line;
    }

}

void Test_nrg_log_format::test_shall_fail_match()
{
    QFETCH(QString, Incorrect_log_format);

    QVERIFY2(!m_format->match(Incorrect_log_format), "Log format match passed, but should fail.");
}

void Test_nrg_log_format::test_shall_fail_match_data()
{
    QTest::addColumn<QString>("Incorrect_log_format");

    {
        QString line = "2010-09-08 12:06:12,079864";
        line.insert(0, "\2");
        line.append("\3\r\n");
        QTest::newRow("Incorrect nrg log line, missing TAB at end") << line;
    }

    {
        QString line = "2010/09/08 12:06:12,079864\t";
        line.insert(0, "\2");
        line.append("\3\r\n");
        QTest::newRow("Incorrect nrg log line, date not right formated") << line;
    }

    {
        QString line = "2010-09-08 12:06:12.079864\t";
        line.insert(0, "\2");
        line.append("\3\r\n");
        QTest::newRow("Incorrect nrg log line, ms separator wrong") << line;
    }

    {
        QString line = "2010-09-08 12:06:12.079864\t";
        QTest::newRow("Incorrect nrg log line, missing start of text") << line;
    }
}


void Test_nrg_log_format::test_create_log_item()
{
    QFETCH(QString, Log_lines);
    QFETCH(Log_type, Expected_type);

    // setup a QSignalSpy to react on log_found, register Log_item and NRG_log_item, see Q_DECLARE_METATYPE
    // at the top of this file as well.
    qRegisterMetaType<QSharedPointer<Log_item> >("QSharedPointer<Log_item>");
    qRegisterMetaType<QSharedPointer<NRG_log_item> >("QSharedPointer<NRG_log_item>");
    QSignalSpy spy(m_format, SIGNAL(log_found(QSharedPointer<Log_item>)));

    m_format->on_line_found(Log_lines);

    // make sure the signal was emitted exactly one time
    QCOMPARE(spy.count(), 1);
    // take the first signal
    QList<QVariant> arguments = spy.takeFirst();

    QSharedPointer<Log_item> log_item = qvariant_cast<QSharedPointer<Log_item> >(arguments.at(0));
    QVERIFY2(!log_item.isNull(), "Log_item returned from signal log_found is NULL.");

    QCOMPARE(log_item->get_application(),   QString("Time_service"));
    QCOMPARE(log_item->get_date(),          QString("2010-09-08 12:06:12,079864"));
    QCOMPARE(log_item->get_file(),          QString("ntp_algorithms_impl.h(125)"));
    QCOMPARE(log_item->get_line(),          QString(""));
    QCOMPARE(log_item->get_log_type(),      Expected_type);
    QCOMPARE(log_item->get_module(),        QString("VRS"));
    QCOMPARE(log_item->get_origin(),        QString("Test case"));
    QCOMPARE(log_item->get_text(),          QString("This is a info message"));
    QCOMPARE(log_item->get_thread(),        QString("Time_service.VR_Sensor_LPN2(439)(8840)"));
    QCOMPARE(log_item->get_time(),          QString(""));
}

void Test_nrg_log_format::test_create_log_item_data()
{
    QTest::addColumn<QString>("Log_lines");
    QTest::addColumn<Log_type>("Expected_type");

    QTest::newRow("Log type = None")
            << "2010-09-08 12:06:12,079864\t\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_none;
    QTest::newRow("Log type = Info")
            << "2010-09-08 12:06:12,079864\tINFO\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_info;
    QTest::newRow("Log type = Warning")
            << "2010-09-08 12:06:12,079864\tWARNING\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_warning;
    QTest::newRow("Log type = Error")
            << "2010-09-08 12:06:12,079864\tERROR\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_error;
    QTest::newRow("Log type = Fatal")
            << "2010-09-08 12:06:12,079864\tFATAL\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_fatal;
    QTest::newRow("Log type = Trace")
            << "2010-09-08 12:06:12,079864\tTRACE\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_trace;
    QTest::newRow("Log type = Debug")
            << "2010-09-08 12:06:12,079864\tDEBUG\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_debug;
    QTest::newRow("Log type = Always")
            << "2010-09-08 12:06:12,079864\tOFF\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_always;
    QTest::newRow("Log type = Unknown")
            << "2010-09-08 12:06:12,079864\tTEST\tVRS\tTime_service\tThis is a info message\tTime_service.VR_Sensor_LPN2(439)(8840)\tntp_algorithms_impl.h(125)"
            << lt_unknown;
}

void Test_nrg_log_format::test_log_columns()
{
    QFETCH(int, Columns);
    QFETCH(Column_type, Expected);

    QCOMPARE(m_format->get_columns().size(), 9);
    QCOMPARE(m_format->get_columns().at(Columns)->type, Expected);
}

void Test_nrg_log_format::test_log_columns_data()
{
    QTest::addColumn<int>("Columns");
    QTest::addColumn<Column_type>("Expected");

    QTest::newRow("Index") << 0 << column_index;
    QTest::newRow("Type") << 1 << column_type;
    QTest::newRow("Text") << 2 << column_text;
    QTest::newRow("Date") << 3 << column_date;
    QTest::newRow("Application") << 4 << column_application;
    QTest::newRow("Module") << 5 << column_module;
    QTest::newRow("File") << 6 << column_file;
    QTest::newRow("Thread") << 7 << column_thread;
    QTest::newRow("Origin") << 8 << column_origin;
}

void Test_nrg_log_format::initTestCase()
{
    m_format = new NRG_log_format("Test case");
}

void Test_nrg_log_format::cleanupTestCase()
{
    delete m_format;
}

QTEST_APPLESS_MAIN(Test_nrg_log_format);

#include "tst_nrg_log_format.moc"
