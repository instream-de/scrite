/****************************************************************************
**
** Copyright (C) TERIFLIX Entertainment Spaces Pvt. Ltd. Bengaluru
** Author: Prashanth N Udupa (prashanth.udupa@teriflix.com)
**
** This code is distributed under GPL v3. Complete text of the license
** can be found here: https://www.gnu.org/licenses/gpl-3.0.txt
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STATISTICSREPORT_P_H
#define STATISTICSREPORT_P_H

#include "pdfexportablegraphicsscene.h"
#include "statisticsreport.h"

class Screenplay;
class StatisticsReport;
class ScreenplayTextDocument;

class StatisticsReportPage : public PdfExportableGraphicsScene
{
public:
    StatisticsReportPage(StatisticsReport *parent = nullptr);
    ~StatisticsReportPage();
};

class StatisticsReportKeyNumbers : public QGraphicsRectItem
{
public:
    StatisticsReportKeyNumbers(const StatisticsReport *report, QGraphicsItem *parent = nullptr);
    ~StatisticsReportKeyNumbers();
};

class StatisticsReportTimeline : public QGraphicsRectItem
{
public:
    StatisticsReportTimeline(qreal suggestedWidth, const StatisticsReport *report,
                             QGraphicsItem *parent = nullptr);
    ~StatisticsReportTimeline();

private:
    QGraphicsRectItem *createTimelineItem(const StatisticsReport *report,
                                          QGraphicsRectItem *container) const;
    QGraphicsRectItem *createDistributionItems(QGraphicsRectItem *container,
                                               const QList<StatisticsReport::Distribution> &dist,
                                               qreal heightHint, qreal totalPixelLength,
                                               bool lightenFillColors = false) const;
    QGraphicsRectItem *createScreenplayTracks(const StatisticsReport *report,
                                              QGraphicsRectItem *container, qreal heightHint,
                                              qreal totalPixelLength);
    qreal evalPixelLength(const QList<StatisticsReport::Distribution> &dist) const;
    QGraphicsRectItem *createScenePullouts(const StatisticsReport *report,
                                           QGraphicsRectItem *container,
                                           QGraphicsRectItem *sceneItemsContainer) const;

    QList<QPair<QString, QList<int>>> evalCharacterPresence(const StatisticsReport *report) const;
    QList<QPair<QString, QList<int>>> evalLocationPresence(const StatisticsReport *report) const;
    QList<QPair<QString, QList<int>>>
    evalPresence(const StatisticsReport *report, const QStringList &allNames,
                 std::function<int(const Scene *, const QString &)> determinePresenceFunc) const;

    QGraphicsRectItem *
    createCharacterPresenceGraph(const StatisticsReport *report, QGraphicsItem *container,
                                 const QGraphicsRectItem *sceneItemsContainer) const;
    QGraphicsRectItem *
    createLocationPresenceGraph(const StatisticsReport *report, QGraphicsItem *container,
                                const QGraphicsRectItem *sceneItemsContainer) const;
    QGraphicsRectItem *
    createPresenceGraph(const QList<QPair<QString, QList<int>>> &presence,
                        std::function<QColor(const QString &)> evalColorFunc,
                        std::function<QString(const QString &, const QList<int> &)> evalLabelFunc,
                        const StatisticsReport *report, QGraphicsItem *container,
                        const QGraphicsRectItem *sceneItems, bool useCurvedPath = true) const;

    QGraphicsRectItem *createSeparator(QGraphicsItem *container, const QString &label,
                                       const QColor &color) const;
};

class StatisticsReportDialogueActionRatio : public QGraphicsRectItem
{
public:
    StatisticsReportDialogueActionRatio(const StatisticsReport *report,
                                        QGraphicsItem *parent = nullptr);
    ~StatisticsReportDialogueActionRatio();
};

class StatisticsReportSceneHeadingStats : public QGraphicsRectItem
{
public:
    StatisticsReportSceneHeadingStats(const StatisticsReport *report,
                                      QGraphicsItem *parent = nullptr);
    ~StatisticsReportSceneHeadingStats();
};

class StatisticsReportGraphVLegend : public QGraphicsRectItem
{
public:
    StatisticsReportGraphVLegend(QGraphicsItem *parent = nullptr);
    ~StatisticsReportGraphVLegend();

    void setFont(const QFont &val) { m_font = val; }
    QFont font() const { return m_font; }

    void place(const QRectF &rect, Qt::Alignment alignment = Qt::AlignRight);
    void add(const QColor &color, const QString &label);
    void add(const QString &label)
    {
        return this->add(StatisticsReport::pickColor(this->childItems().size(), false), label);
    }

private:
    QFont m_font;
};

#endif // STATISTICSREPORT_P_H
