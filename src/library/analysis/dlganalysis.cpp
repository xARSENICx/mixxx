#include "library/analysis/dlganalysis.h"

#include "analyzer/analyzerprogress.h"
#include "analyzer/analyzerscheduledtrack.h"
#include "library/analysis/ui_dlganalysis.h"
#include "library/dao/trackschema.h"
#include "library/library.h"
#include "moc_dlganalysis.cpp"
#include "util/assert.h"
#include "widget/wanalysislibrarytableview.h"
#include "widget/wlibrary.h"

DlgAnalysis::DlgAnalysis(WLibrary* parent,
        UserSettingsPointer pConfig,
        Library* pLibrary)
        : QWidget(parent),
          m_pConfig(pConfig),
          m_bAnalysisActive(false) {
    setupUi(this);
    m_songsButtonGroup.addButton(radioButtonRecentlyAdded);
    m_songsButtonGroup.addButton(radioButtonAllSongs);

    m_uiRecentDays = m_pConfig->getValue<uint>(
            ConfigKey("[Analysis]", "RecentDays"), 7);
    spinBoxRecentDays->setRange(1, 3650);
    spinBoxRecentDays->setValue(m_uiRecentDays);
    spinBoxRecentDays->setSuffix(tr(" days"));

    const QString maxText = QStringLiteral("3650 days");
    const int minWidth = spinBoxRecentDays->fontMetrics().horizontalAdvance(maxText) + 20;
    spinBoxRecentDays->setStyleSheet(QString("min-width: %1px;").arg(minWidth));

    m_pAnalysisLibraryTableView = new WAnalysisLibraryTableView(
            this,
            pConfig,
            pLibrary,
            parent->getTrackTableBackgroundColorOpacity());
    connect(m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::loadTrack,
            this,
            &DlgAnalysis::loadTrack);
    connect(m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::loadTrackToPlayer,
            this,
            &DlgAnalysis::loadTrackToPlayer);

    connect(m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::trackSelected,
            this,
            &DlgAnalysis::trackSelected);

    QBoxLayout* box = qobject_cast<QBoxLayout*>(layout());
    VERIFY_OR_DEBUG_ASSERT(box) { // Assumes the form layout is a QVBox/QHBoxLayout!
    }
    else {
        box->removeWidget(m_pTrackTablePlaceholder);
        m_pTrackTablePlaceholder->hide();
        box->insertWidget(1, m_pAnalysisLibraryTableView);
    }

    m_pAnalysisLibraryTableModel = new AnalysisLibraryTableModel(
            this, pLibrary->trackCollectionManager());
    m_pAnalysisLibraryTableView->loadTrackModel(m_pAnalysisLibraryTableModel);

    connect(radioButtonRecentlyAdded,
            &QRadioButton::clicked,
            this,
            &DlgAnalysis::slotShowRecentSongs);
    connect(radioButtonAllSongs,
            &QRadioButton::clicked,
            this,
            &DlgAnalysis::slotShowAllSongs);

    connect(spinBoxRecentDays,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &DlgAnalysis::slotRecentDaysChanged);
    // Don't click those radio buttons now reduce skin loading time.
    // 'RecentlyAdded' is clicked in onShow()

    connect(pushButtonAnalyze,
            &QPushButton::clicked,
            this,
            &DlgAnalysis::analyze);
    pushButtonAnalyze->setEnabled(false);

    connect(pushButtonSelectAll,
            &QPushButton::clicked,
            this,
            &DlgAnalysis::selectAll);

    connect(m_pAnalysisLibraryTableView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &DlgAnalysis::tableSelectionChanged);

    connect(pLibrary,
            &Library::setTrackTableFont,
            m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::setTrackTableFont);
    connect(pLibrary,
            &Library::setTrackTableRowHeight,
            m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::setTrackTableRowHeight);
    connect(pLibrary,
            &Library::setSelectedClick,
            m_pAnalysisLibraryTableView,
            &WAnalysisLibraryTableView::setSelectedClick);

    slotAnalysisActive(m_bAnalysisActive);
}

void DlgAnalysis::onShow() {
    if (!radioButtonRecentlyAdded->isChecked() &&
            !radioButtonAllSongs->isChecked()) {
        radioButtonRecentlyAdded->click();
    }
    // Refresh table
    // There might be new tracks dropped to other views
    m_pAnalysisLibraryTableModel->select();
}

bool DlgAnalysis::hasFocus() const {
    return m_pAnalysisLibraryTableView->hasFocus();
}

void DlgAnalysis::setFocus() {
    m_pAnalysisLibraryTableView->setFocus();
}

void DlgAnalysis::onSearch(const QString& text) {
    if (radioButtonRecentlyAdded->isChecked()) {
        m_pAnalysisLibraryTableModel->setExtraFilter(getRecentFilter());
    } else {
        m_pAnalysisLibraryTableModel->setExtraFilter(QString());
    }
    m_pAnalysisLibraryTableModel->search(text);
}

void DlgAnalysis::tableSelectionChanged(const QItemSelection&,
        const QItemSelection&) {
    bool tracksSelected = m_pAnalysisLibraryTableView->selectionModel()->hasSelection();
    pushButtonAnalyze->setEnabled(tracksSelected || m_bAnalysisActive);
}

void DlgAnalysis::selectAll() {
    m_pAnalysisLibraryTableView->selectAll();
}

void DlgAnalysis::analyze() {
    if (m_bAnalysisActive) {
        emit stopAnalysis();
    } else {
        QList<AnalyzerScheduledTrack> tracks;

        QModelIndexList selectedIndexes = m_pAnalysisLibraryTableView->selectionModel()->selectedRows();
        for (const auto& selectedIndex : std::as_const(selectedIndexes)) {
            TrackId trackId(m_pAnalysisLibraryTableModel->getFieldVariant(
                    selectedIndex, ColumnCache::COLUMN_LIBRARYTABLE_ID));
            if (trackId.isValid()) {
                tracks.append(trackId);
            }
        }
        emit analyzeTracks(tracks);
    }
}

void DlgAnalysis::slotAnalysisActive(bool bActive) {
    m_bAnalysisActive = bActive;
    if (bActive) {
        pushButtonAnalyze->setChecked(true);
        pushButtonAnalyze->setText(tr("Stop Analysis"));
        pushButtonAnalyze->setEnabled(true);
        labelProgress->setEnabled(true);
    } else {
        pushButtonAnalyze->setChecked(false);
        pushButtonAnalyze->setText(tr("Analyze"));
        labelProgress->setText("");
        labelProgress->setEnabled(false);
    }
}

void DlgAnalysis::onTrackAnalysisSchedulerProgress(
        AnalyzerProgress, int finishedCount, int totalCount) {
    if (labelProgress->isEnabled()) {
        int totalProgressPercent = 0;
        if (totalCount > 0) {
            totalProgressPercent = (finishedCount * 100) / totalCount;
            if (totalProgressPercent > 100) {
                totalProgressPercent = 100;
            }
        }
        labelProgress->setText(tr("Analyzing %1/%2")
                                       .arg(QString::number(finishedCount),
                                               QString::number(totalCount)) +
                QStringLiteral(" (%3%)").arg(
                        QString::number(totalProgressPercent)));
    }
}

void DlgAnalysis::onTrackAnalysisSchedulerFinished() {
    slotAnalysisActive(false);
}

void DlgAnalysis::slotShowRecentSongs() {
    spinBoxRecentDays->setEnabled(true);
    m_pAnalysisLibraryTableModel->setExtraFilter(getRecentFilter());
    m_pAnalysisLibraryTableModel->select();
}

void DlgAnalysis::slotRecentDaysChanged(int days) {
    m_uiRecentDays = static_cast<uint>(days);
    m_pConfig->setValue(ConfigKey("[Analysis]", "RecentDays"), m_uiRecentDays);

    if (radioButtonRecentlyAdded->isChecked()) {
        m_pAnalysisLibraryTableModel->setExtraFilter(getRecentFilter());
        m_pAnalysisLibraryTableModel->select();
    }
}

void DlgAnalysis::slotShowAllSongs() {
    spinBoxRecentDays->setEnabled(false);
    m_pAnalysisLibraryTableModel->setExtraFilter(QString());
    m_pAnalysisLibraryTableModel->select();
}

QString DlgAnalysis::getRecentFilter() const {
    // Return a relative query string, e.g. "added:>-7d"
    // This is handled by DateAddedFilterNode
    return QStringLiteral("added:>%1d").arg(-static_cast<int>(m_uiRecentDays));
}

void DlgAnalysis::installEventFilter(QObject* pFilter) {
    QWidget::installEventFilter(pFilter);
    m_pAnalysisLibraryTableView->installEventFilter(pFilter);
}

void DlgAnalysis::saveCurrentViewState() {
    m_pAnalysisLibraryTableView->saveCurrentViewState();
}

bool DlgAnalysis::restoreCurrentViewState() {
    return m_pAnalysisLibraryTableView->restoreCurrentViewState();
}
