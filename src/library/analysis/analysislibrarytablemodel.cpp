#include "library/analysis/analysislibrarytablemodel.h"

#include <QDateTime>

#include "moc_analysislibrarytablemodel.cpp"
#include "util/datetime.h"

namespace {

inline QString recentFilter(uint days) {
    // Create a user-formatted query equal to SQL query
    // datetime_added > datetime('now', '-<days> days')
    QDateTime dt(QDateTime::currentDateTimeUtc());
    dt = dt.addDays(-static_cast<qint64>(days));
    const QString dateStr = QLocale::system().toString(dt.date(), QLocale::ShortFormat);
    // FIXME alternatively, use "added:-days" and add the respective
    // literal parser to DateAddedFilterNode
    return QStringLiteral("added:>%1").arg(dateStr);
}

} // anonymous namespace

AnalysisLibraryTableModel::AnalysisLibraryTableModel(QObject* parent,
        TrackCollectionManager* pTrackCollectionManager)
        : LibraryTableModel(parent, pTrackCollectionManager, "mixxx.db.model.prepare") {
    // Default to showing recent tracks.
    setExtraFilter(recentFilter(7));
}

void AnalysisLibraryTableModel::showRecentSongs(uint days) {
    // Search with the recent filter.
    setExtraFilter(recentFilter(days));
    select();
}

void AnalysisLibraryTableModel::showAllSongs() {
    // Clear the recent filter.
    setExtraFilter({});
    select();
}

void AnalysisLibraryTableModel::searchCurrentTrackSet(
        const QString& text, bool useRecentFilter, uint days) {
    setExtraFilter(useRecentFilter ? recentFilter(days) : QString{});
    search(text);
}
