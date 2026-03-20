#include "library/findduplicates/dlgfindduplicates.h"

#include <QAction>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include "moc_dlgfindduplicates.cpp"

namespace {

// Dummy data representing grouped duplicate results.
// Each group shares an AcoustID; different masterings are sub-rows.
struct DummyTrack {
    QString group;
    QString title;
    QString artist;
    QString format;
    QString bitrate;
    QString quality;
    bool isCanonical;
};

const QList<DummyTrack> kDummyData = {
        // Group 1
        {QStringLiteral("AcoustID-A1B2C3"),
                QStringLiteral("Get Lucky"),
                QStringLiteral("Daft Punk"),
                QStringLiteral("FLAC"),
                QStringLiteral("1411 kbps"),
                QStringLiteral("★★★★★"),
                true},
        {QStringLiteral("AcoustID-A1B2C3"),
                QStringLiteral("Get Lucky"),
                QStringLiteral("Daft Punk"),
                QStringLiteral("MP3"),
                QStringLiteral("320 kbps"),
                QStringLiteral("★★★★"),
                false},
        {QStringLiteral("AcoustID-A1B2C3"),
                QStringLiteral("Get Lucky"),
                QStringLiteral("Daft Punk"),
                QStringLiteral("MP3"),
                QStringLiteral("128 kbps"),
                QStringLiteral("★★"),
                false},
        // Group 2
        {QStringLiteral("AcoustID-D4E5F6"),
                QStringLiteral("Strobe"),
                QStringLiteral("deadmau5"),
                QStringLiteral("WAV"),
                QStringLiteral("1411 kbps"),
                QStringLiteral("★★★★★"),
                true},
        {QStringLiteral("AcoustID-D4E5F6"),
                QStringLiteral("Strobe"),
                QStringLiteral("deadmau5"),
                QStringLiteral("AAC"),
                QStringLiteral("256 kbps"),
                QStringLiteral("★★★"),
                false},
        // Group 3
        {QStringLiteral("AcoustID-G7H8I9"),
                QStringLiteral("Windowlicker"),
                QStringLiteral("Aphex Twin"),
                QStringLiteral("FLAC"),
                QStringLiteral("1411 kbps"),
                QStringLiteral("★★★★★"),
                true},
        {QStringLiteral("AcoustID-G7H8I9"),
                QStringLiteral("Windowlicker"),
                QStringLiteral("Aphex Twin"),
                QStringLiteral("OGG"),
                QStringLiteral("192 kbps"),
                QStringLiteral("★★★"),
                false},
        {QStringLiteral("AcoustID-G7H8I9"),
                QStringLiteral("Windowlicker"),
                QStringLiteral("Aphex Twin"),
                QStringLiteral("MP3"),
                QStringLiteral("128 kbps"),
                QStringLiteral("★★"),
                false},
};

} // anonymous namespace

DlgFindDuplicates::DlgFindDuplicates(QWidget* parent)
        : QWidget(parent),
          m_pLayout(nullptr),
          m_pHeaderLabel(nullptr),
          m_pInfoLabel(nullptr),
          m_pTable(nullptr) {
    setupUi();
    populateDummyData();
}

void DlgFindDuplicates::setupUi() {
    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(12, 12, 12, 12);
    m_pLayout->setSpacing(8);

    // Header
    m_pHeaderLabel = new QLabel(tr("Find Doubles — Mastering-Aware Duplicate Detection"), this);
    QFont headerFont = m_pHeaderLabel->font();
    headerFont.setPointSize(headerFont.pointSize() + 4);
    headerFont.setBold(true);
    m_pHeaderLabel->setFont(headerFont);
    m_pLayout->addWidget(m_pHeaderLabel);

    // Info label
    m_pInfoLabel = new QLabel(
            tr("Tracks grouped by AcoustID fingerprint. The ★ canonical version "
               "(highest quality) is highlighted. Right-click for actions."),
            this);
    m_pInfoLabel->setWordWrap(true);
    m_pLayout->addWidget(m_pInfoLabel);

    // Table
    m_pTable = new QTableWidget(this);
    m_pTable->setColumnCount(6);
    m_pTable->setHorizontalHeaderLabels(
            {tr("Group"), tr("Title"), tr("Artist"), tr("Format"), tr("Bitrate"), tr("Quality")});
    m_pTable->horizontalHeader()->setStretchLastSection(true);
    m_pTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_pTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pTable->setAlternatingRowColors(true);
    m_pTable->verticalHeader()->hide();
    m_pTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_pTable,
            &QTableWidget::customContextMenuRequested,
            this,
            &DlgFindDuplicates::showContextMenu);
    m_pLayout->addWidget(m_pTable);

    // Bottom button bar
    auto* pButtonLayout = new QHBoxLayout();
    pButtonLayout->addStretch();

    auto* pKeepBestBtn = new QPushButton(tr("Keep Best Quality for All Groups"), this);
    pKeepBestBtn->setToolTip(tr("Automatically keep the highest-quality version in each group"));
    connect(pKeepBestBtn, &QPushButton::clicked, this, &DlgFindDuplicates::slotKeepBestAll);
    pButtonLayout->addWidget(pKeepBestBtn);

    m_pLayout->addLayout(pButtonLayout);

    setLayout(m_pLayout);
}

void DlgFindDuplicates::populateDummyData() {
    m_pTable->setRowCount(kDummyData.size());

    QString lastGroup;
    for (int row = 0; row < kDummyData.size(); ++row) {
        const auto& track = kDummyData[row];

        // Only show group ID on first row of each group
        QString displayGroup;
        if (track.group != lastGroup) {
            displayGroup = track.group;
            lastGroup = track.group;
        }

        auto* groupItem = new QTableWidgetItem(displayGroup);
        auto* titleItem = new QTableWidgetItem(track.title);
        auto* artistItem = new QTableWidgetItem(track.artist);
        auto* formatItem = new QTableWidgetItem(track.format);
        auto* bitrateItem = new QTableWidgetItem(track.bitrate);
        auto* qualityItem = new QTableWidgetItem(track.quality);

        if (track.isCanonical) {
            QFont boldFont = titleItem->font();
            boldFont.setBold(true);
            titleItem->setFont(boldFont);
            artistItem->setFont(boldFont);
            formatItem->setFont(boldFont);
            bitrateItem->setFont(boldFont);
            qualityItem->setFont(boldFont);
            // Highlight canonical row
            QColor highlight(76, 175, 80, 40); // subtle green
            groupItem->setBackground(highlight);
            titleItem->setBackground(highlight);
            artistItem->setBackground(highlight);
            formatItem->setBackground(highlight);
            bitrateItem->setBackground(highlight);
            qualityItem->setBackground(highlight);
        }

        m_pTable->setItem(row, 0, groupItem);
        m_pTable->setItem(row, 1, titleItem);
        m_pTable->setItem(row, 2, artistItem);
        m_pTable->setItem(row, 3, formatItem);
        m_pTable->setItem(row, 4, bitrateItem);
        m_pTable->setItem(row, 5, qualityItem);
    }

    m_pTable->resizeColumnsToContents();
}

void DlgFindDuplicates::showContextMenu(const QPoint& pos) {
    QMenu menu(this);

    auto* pKeepAction = menu.addAction(tr("Keep This Version"));
    connect(pKeepAction, &QAction::triggered, this, &DlgFindDuplicates::slotKeepThisVersion);

    auto* pRemoveAction = menu.addAction(tr("Remove Duplicate"));
    connect(pRemoveAction, &QAction::triggered, this, &DlgFindDuplicates::slotRemoveDuplicate);

    menu.addSeparator();

    auto* pCompareAction = menu.addAction(tr("Compare Masterings"));
    connect(pCompareAction, &QAction::triggered, this, &DlgFindDuplicates::slotCompareMasterings);

    menu.exec(m_pTable->viewport()->mapToGlobal(pos));
}

void DlgFindDuplicates::onShow() {
    // Placeholder — would refresh data from FingerprintDAO
}

bool DlgFindDuplicates::hasFocus() const {
    return m_pTable && m_pTable->hasFocus();
}

void DlgFindDuplicates::setFocus() {
    if (m_pTable) {
        m_pTable->setFocus();
    }
}

void DlgFindDuplicates::onSearch(const QString& text) {
    Q_UNUSED(text);
    // Placeholder — would filter table rows
}

// --- Stub slots (buttons do nothing yet) ---

void DlgFindDuplicates::slotKeepThisVersion() {
    // TODO(CMRT): Mark selected row as canonical, demote others in group
}

void DlgFindDuplicates::slotRemoveDuplicate() {
    // TODO(CMRT): Remove selected duplicate from library
}

void DlgFindDuplicates::slotCompareMasterings() {
    // TODO(CMRT): Open diff-view showing offset, quality scores, format diffs
}

void DlgFindDuplicates::slotKeepBestAll() {
    // TODO(CMRT): For each group, keep highest quality version
}
