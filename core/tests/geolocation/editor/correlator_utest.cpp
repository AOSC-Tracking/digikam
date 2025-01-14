/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-01-17
 * Description : Test parsing gpx data.
 *
 * SPDX-FileCopyrightText: 2010-2014 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "correlator_utest.h"

// Qt includes

#include <QDateTime>
#include <QUrl>
#include <QSignalSpy>

// Local includes

#include "digikam_debug.h"
#include "trackreader.h"
#include "track_correlator.h"
#include "dtestdatadir.h"

using namespace Digikam;

/**
 * @brief Return the path of the directory containing the test data
 */
QString GetTestDataDirectory()
{
    QString filesPath = DTestDataDir::TestData(QString::fromUtf8("core/tests/geolocation/editor"))
                           .root().path() + QLatin1Char('/');
    qCDebug(DIGIKAM_TESTS_LOG) << "Test Data Dir:" << filesPath;

    return filesPath;
}

/**
 * @brief Dummy test that does nothing
 */
void TestGPXParsing::testNoOp()
{
}

/**
 * @brief Test correlator
 */
void TestGPXParsing::testCorrelator1()
{
    QUrl testDataDir = QUrl::fromLocalFile(GetTestDataDirectory() + QLatin1Char('/') + QLatin1String("gpxfile-1.gpx"));

    QList<QUrl> fileList;
    fileList << testDataDir;

    TrackManager myParser;

    QSignalSpy spyTrackFiles(&myParser, SIGNAL(signalTracksChanged(QList<TrackManager::TrackChanges>)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllTrackFilesReady()));

    myParser.loadTrackFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }

    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyTrackFiles.count(), 1);

    const TrackManager::Track& file1 = myParser.getTrack(0);
    QVERIFY(!file1.points.isEmpty());

    // items to correlate:
    TrackCorrelator::Correlation::List itemsToCorrelate;
    {
        TrackCorrelator::Correlation myItem;
        myItem.dateTime = TrackReader::ParseTime(QLatin1String("2009-07-26T18:00:00Z"));
        itemsToCorrelate << myItem;
    }

    TrackCorrelator correlator(&myParser);
    QSignalSpy spyItemsFinished(&correlator, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&correlator, SIGNAL(signalItemsCorrelated(Digikam::TrackCorrelator::Correlation::List)));
    QVERIFY(spyItemsCorrelated.isValid());

    TrackCorrelator::CorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    correlator.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    Digikam::TrackCorrelator::Correlation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<Digikam::TrackCorrelator::Correlation::List>();
    QCOMPARE(myCorrelatedItems.count(), 1);
    QCOMPARE(myCorrelatedItems.first().coordinates, GeoCoordinates::fromGeoUrl(QLatin1String("geo:18,7,0")));
    QCOMPARE(myCorrelatedItems.first().nSatellites, 3);
    QCOMPARE(myCorrelatedItems.first().hDop, 2.5);
    QCOMPARE(myCorrelatedItems.first().speed, 3.14);
}

/**
 * @brief Test interpolation
 */
void TestGPXParsing::testInterpolation()
{
    QUrl testDataDir = QUrl::fromLocalFile(GetTestDataDirectory() + QLatin1Char('/') + QLatin1String("gpxfile-1.gpx"));

    QList<QUrl> fileList;
    fileList << testDataDir;

    TrackManager myParser;

    QSignalSpy spyTrackFiles(&myParser, SIGNAL(signalTracksChanged(QList<TrackManager::TrackChanges>)));
    QSignalSpy spyAllDone(&myParser, SIGNAL(signalAllTrackFilesReady()));

    myParser.loadTrackFiles(fileList);

    // wait until the files are loaded:
    while (spyAllDone.isEmpty())
    {
        QTest::qWait(100);
    }

    QCOMPARE(spyAllDone.count(), 1);
    QCOMPARE(spyTrackFiles.count(), 1);

    const TrackManager::Track& file1 = myParser.getTrack(0);
    QVERIFY(!file1.points.isEmpty());

    // items to correlate:
    TrackCorrelator::Correlation::List itemsToCorrelate;
    {
        TrackCorrelator::Correlation myItem;
        myItem.dateTime = TrackReader::ParseTime(QLatin1String("2009-11-29T17:00:30Z"));
        itemsToCorrelate << myItem;
    }

    TrackCorrelator correlator(&myParser);

    QSignalSpy spyItemsFinished(&correlator, SIGNAL(signalAllItemsCorrelated()));
    QVERIFY(spyItemsFinished.isValid());
    QSignalSpy spyItemsCorrelated(&correlator, SIGNAL(signalItemsCorrelated(Digikam::TrackCorrelator::Correlation::List)));
    QVERIFY(spyItemsCorrelated.isValid());

    TrackCorrelator::CorrelationOptions correlationOptions;
    correlationOptions.maxGapTime = 0;
    correlationOptions.interpolate = true;
    correlationOptions.interpolationDstTime = 31;
    correlator.correlate(itemsToCorrelate, correlationOptions);

    while (spyItemsFinished.isEmpty())
    {
        QTest::qWait(100);
    }
    QCOMPARE(spyItemsFinished.count(), 1);
    QCOMPARE(spyItemsCorrelated.count(), 1);

    Digikam::TrackCorrelator::Correlation::List myCorrelatedItems = spyItemsCorrelated.first().first().value<Digikam::TrackCorrelator::Correlation::List>();
    QCOMPARE(myCorrelatedItems.count(), 1);
    QCOMPARE(myCorrelatedItems.first().coordinates, GeoCoordinates::fromGeoUrl(QLatin1String("geo:17.5,0.5,3")));
    QCOMPARE(myCorrelatedItems.first().nSatellites, -1);
    QCOMPARE(myCorrelatedItems.first().hDop, -1.0);
}

QTEST_GUILESS_MAIN(TestGPXParsing)

#include "moc_correlator_utest.cpp"
