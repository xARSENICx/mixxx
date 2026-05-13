#include "qml/qmllegacylibraryitem.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QHeaderView>
#include <QLabel>
#include <QMetaEnum>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QSplitter>
#include <QStyle>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>
#include <cmath>

#include "control/controlproxy.h"
#include "control/controlpushbutton.h"
#include "library/library.h"
#include "mixer/playermanager.h"
#include "moc_qmllegacylibraryitem.cpp"
#include "qml/qmlconfigproxy.h"
#include "qml/qmllibraryproxy.h"
#include "skin/legacy/skincontext.h"
#include "waveform/overviewtype.h"
#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"
#include "widget/wsearchlineedit.h"

namespace mixxx {
namespace qml {

QmlLegacyLibraryItem::~QmlLegacyLibraryItem() = default;

QmlLegacyLibraryItem::QmlLegacyLibraryItem(QQuickItem* parent)
        : QQuickPaintedItem(parent),
          m_pRootWidget(std::make_unique<QWidget>()) {
    setAntialiasing(false);
    setOpaquePainting(true);

    // Configure for input handling
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);

    m_pRootWidget->setAutoFillBackground(true);
    m_pRootWidget->setAttribute(Qt::WA_DontShowOnScreen);
    m_pRootWidget->setObjectName(QStringLiteral("LibraryContainer"));
    // 1. Create splitter layout
    auto* pSplitter = new QSplitter(m_pRootWidget.get());

    // 2. Sidebar page (search + sidebar)
    auto* pSidebarPage = new QWidget(pSplitter);
    auto* pSidebarLayout = new QVBoxLayout(pSidebarPage);
    pSidebarLayout->setContentsMargins(0, 0, 0, 0);

    UserSettingsPointer pConfig = QmlConfigProxy::get();
    m_pSearchLineEdit = new WSearchLineEdit(pSidebarPage, pConfig);
    m_pSidebar = new WLibrarySidebar(pSidebarPage);

    pSidebarLayout->addWidget(m_pSearchLineEdit);
    pSidebarLayout->addWidget(m_pSidebar);

    // 3. Library (main content area)
    m_pLibraryWidget = new WLibrary(pSplitter);
    m_pLibraryWidget->setObjectName(QStringLiteral("LibraryContainer"));
    applyLegacyLibrarySkinConfiguration();

    // 4. Add to splitter
    pSplitter->addWidget(pSidebarPage);
    pSplitter->addWidget(m_pLibraryWidget);
    pSplitter->setSizes({200, 600});

    // 5. Root layout
    auto* pRootLayout = new QVBoxLayout(m_pRootWidget.get());
    pRootLayout->setContentsMargins(0, 0, 0, 0);
    pRootLayout->addWidget(pSplitter);

    // 6. Initialize the WaveformOverviewType ControlPushButton BEFORE binding
    //    the library, because bindLibraryWidget creates OverviewDelegate which
    //    reads this CO in its constructor. In legacy mode DlgPrefWaveform
    //    creates this CO, but that dialog is never constructed in QML mode.
    initializeOverviewTypeControl();

    // 7. Bind to Library singleton
    Library* pLibrary = QmlLibraryProxy::get();
    if (pLibrary) {
        pLibrary->bindSearchboxWidget(m_pSearchLineEdit);
        pLibrary->bindSidebarWidget(m_pSidebar);
        pLibrary->bindLibraryWidget(m_pLibraryWidget, QmlLibraryProxy::getKeyboard());

        // 8. Trigger repaints on visual changes and refresh input tracking for
        //    views that are created or swapped after the initial bind.
        connect(pLibrary, &Library::switchToView, this, [this]() {
            enableEmbeddedWidgetInputTracking();
            applyLegacyScrollbarStyles();
            connectSortBypass();
            repaintEmbeddedViews();
        });
        connect(pLibrary, &Library::showTrackModel, this, [this]() {
            enableEmbeddedWidgetInputTracking();
            applyLegacyScrollbarStyles();
            connectSortBypass();
            repaintEmbeddedViews();
        });

        // Initialize default view to Tracks collection to avoid black screen
        pLibrary->searchTracksInCollection();
    } else {
        qWarning() << "QmlLegacyLibraryItem: Library singleton not available!";
    }

    // 9. [PoC hack] Apply the LateNight classic stylesheet so the embedded
    //    QWidget tree renders branch arrows, preview button icons, and other
    //    SVG-based decorations that are normally applied by LegacySkinParser.
    //    TODO(GSoC): Replace with the QQuickAsyncImageProvider "skin:" scheme
    //    and QML palette bindings once the library panel is ported to QML.
    applyLegacyStylesheet();
    repolishEmbeddedWidgets();
    enableEmbeddedWidgetInputTracking();
    applyLegacyScrollbarStyles();
    connectSortBypass();

    const QString previewDeckGroup = PlayerManager::groupForPreviewDeck(0);
    m_pPreviewDeckPlay = std::make_unique<ControlProxy>(
            previewDeckGroup,
            QStringLiteral("play"),
            this,
            ControlFlag::NoAssertIfMissing);
    m_pPreviewDeckTrackLoaded = std::make_unique<ControlProxy>(
            previewDeckGroup,
            QStringLiteral("track_loaded"),
            this,
            ControlFlag::NoAssertIfMissing);
    m_pPreviewDeckPlay->connectValueChanged(this, [this](double) {
        repaintEmbeddedViews();
    });
    m_pPreviewDeckTrackLoaded->connectValueChanged(this, [this](double) {
        repaintEmbeddedViews();
    });

    // 9. Periodic repaint timer (~30 fps) to flush async widget repaints
    //    (hover state changes, scrollbar animations, delegate updates) to the
    //    QML scene.  The QWidget tree repaints into its backing store silently
    //    because WA_DontShowOnScreen suppresses normal screen updates.
    //    TODO(GSoC): Replace with an event-filter on the root widget that
    //    intercepts QEvent::UpdateRequest so we only repaint when needed.
    auto* pRepaintTimer = new QTimer(this);
    connect(pRepaintTimer, &QTimer::timeout, this, [this]() { update(); });
    pRepaintTimer->start(33); // ~30 fps
}

void QmlLegacyLibraryItem::paint(QPainter* pPainter) {
    if (!m_pRootWidget) {
        return;
    }

    updateWidgetSize();
    m_pRootWidget->render(pPainter);
}

void QmlLegacyLibraryItem::geometryChange(
        const QRectF& newGeometry,
        const QRectF& oldGeometry) {
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
    updateWidgetSize();
    update();
}

namespace {
constexpr int kHeaderResizeCursorMargin = 4;

QPointF widgetScenePos(QWidget* target, QWidget* root, const QPoint& rootPos) {
    return QPointF(target->mapFrom(root, rootPos));
}
} // namespace

QWidget* QmlLegacyLibraryItem::widgetAtRootPos(const QPoint& rootPos) const {
    if (!m_pRootWidget) {
        return nullptr;
    }

    QWidget* widget = m_pRootWidget->childAt(rootPos);
    if (!widget) {
        return m_pRootWidget.get();
    }

    while (QWidget* child = widget->childAt(widget->mapFrom(m_pRootWidget.get(), rootPos))) {
        if (child == widget) {
            break;
        }
        widget = child;
    }
    return widget;
}

QAbstractItemView* QmlLegacyLibraryItem::parentItemView(QWidget* widget) const {
    for (QWidget* current = widget; current; current = current->parentWidget()) {
        if (auto* view = qobject_cast<QAbstractItemView*>(current)) {
            return view;
        }
    }
    return nullptr;
}

QHeaderView* QmlLegacyLibraryItem::parentHeaderView(QWidget* widget) const {
    for (QWidget* current = widget; current; current = current->parentWidget()) {
        if (auto* header = qobject_cast<QHeaderView*>(current)) {
            return header;
        }
    }
    return nullptr;
}

QWidget* QmlLegacyLibraryItem::eventTargetFor(QWidget* widget) const {
    if (!widget) {
        return m_pRootWidget.get();
    }

    // QHeaderView IS-A QAbstractItemView, but sort-click handling lives on the
    // header itself, not its internal viewport. Do NOT redirect header clicks.
    if (qobject_cast<QHeaderView*>(widget)) {
        return widget;
    }

    if (auto* view = parentItemView(widget)) {
        // For table/list views, redirect to viewport so delegates get events.
        if (widget == view) {
            QWidget* viewport = view->viewport();
            return viewport ? viewport : widget;
        }
    }
    return widget;
}

bool QmlLegacyLibraryItem::sendMouseToWidget(QMouseEvent* event, QWidget* target) {
    if (!m_pRootWidget || !target) {
        return false;
    }

    const QPoint rootPos = event->position().toPoint();
    updateHoverTarget(target, rootPos, event->modifiers());

    const QPointF targetPos = widgetScenePos(target, m_pRootWidget.get(), rootPos);
    QMouseEvent mappedEvent(
            event->type(),
            targetPos,
            event->scenePosition(),
            event->globalPosition(),
            event->button(),
            event->buttons(),
            event->modifiers(),
            event->source());

    QApplication::sendEvent(target, &mappedEvent);
    event->setAccepted(mappedEvent.isAccepted());
    m_pressedButtons = event->buttons();

    syncCursorFromWidget(target, rootPos);
    return mappedEvent.isAccepted();
}

void QmlLegacyLibraryItem::sendSyntheticMouseMoveToWidget(QWidget* target,
        const QPoint& rootPos,
        const QPointF& globalPos,
        Qt::KeyboardModifiers modifiers,
        Qt::MouseButtons buttons) {
    if (!m_pRootWidget || !target) {
        return;
    }

    const QPointF targetPos = widgetScenePos(target, m_pRootWidget.get(), rootPos);
    QMouseEvent moveEvent(
            QEvent::MouseMove,
            targetPos,
            QPointF(rootPos),
            globalPos,
            Qt::NoButton,
            buttons,
            modifiers);
    QApplication::sendEvent(target, &moveEvent);
    syncCursorFromWidget(target, rootPos);
}

bool QmlLegacyLibraryItem::sendWheelToWidget(QWheelEvent* event) {
    if (!m_pRootWidget) {
        return false;
    }

    QWidget* target = eventTargetFor(widgetAtRootPos(event->position().toPoint()));
    if (!target) {
        return false;
    }

    const QPoint rootPos = event->position().toPoint();
    updateHoverTarget(target, rootPos, event->modifiers());

    QWheelEvent mappedEvent(
            widgetScenePos(target, m_pRootWidget.get(), rootPos),
            event->globalPosition(),
            event->pixelDelta(),
            event->angleDelta(),
            event->buttons(),
            event->modifiers(),
            event->phase(),
            event->inverted());

    QApplication::sendEvent(target, &mappedEvent);
    event->setAccepted(mappedEvent.isAccepted());
    syncCursorFromWidget(target, rootPos);
    return mappedEvent.isAccepted();
}

bool QmlLegacyLibraryItem::sendHoverToWidget(QHoverEvent* event) {
    if (!m_pRootWidget) {
        return false;
    }

    QWidget* target = eventTargetFor(widgetAtRootPos(event->position().toPoint()));
    if (!target) {
        return false;
    }

    const QPoint rootPos = event->position().toPoint();
    updateHoverTarget(target, rootPos, event->modifiers());

    const QPoint targetPos = target->mapFrom(m_pRootWidget.get(), rootPos);
    const QPoint oldTargetPos = target->mapFrom(m_pRootWidget.get(), event->oldPos());
    QHoverEvent mappedEvent(
            event->type(),
            targetPos,
            event->globalPosition(),
            oldTargetPos,
            event->modifiers());

    QApplication::sendEvent(target, &mappedEvent);
    event->setAccepted(mappedEvent.isAccepted());
    m_lastHoverRootPos = event->position();

    // QTableView::entered(), which PreviewButtonDelegate uses to open the
    // real QPushButton editor, is driven by mouse tracking rather than
    // QHoverEvent delivery. Mirror QQuick hover as a no-button mouse move so
    // item-view delegates see the same path they get in a native QWidget skin.
    sendSyntheticMouseMoveToWidget(target, rootPos, event->globalPosition(), event->modifiers());

    syncCursorFromWidget(target, rootPos);
    return mappedEvent.isAccepted();
}

void QmlLegacyLibraryItem::updateHoverTarget(
        QWidget* target,
        const QPoint& rootPos,
        Qt::KeyboardModifiers modifiers) {
    if (!target || target == m_pLastHoverWidget) {
        return;
    }

    if (m_pLastHoverWidget) {
        QEvent leaveEvent(QEvent::Leave);
        QApplication::sendEvent(m_pLastHoverWidget, &leaveEvent);
    }

    const QPointF targetPos = widgetScenePos(target, m_pRootWidget.get(), rootPos);
    Q_UNUSED(modifiers);
    QEnterEvent enterEvent(targetPos,
            QPointF(rootPos),
            QPointF(target->mapToGlobal(targetPos.toPoint())));
    QApplication::sendEvent(target, &enterEvent);
    m_pLastHoverWidget = target;
}

bool QmlLegacyLibraryItem::isHeaderResizeHandle(QHeaderView* header, const QPoint& rootPos) const {
    if (!header || !m_pRootWidget) {
        return false;
    }

    const QPoint headerPos = header->mapFrom(m_pRootWidget.get(), rootPos);
    const int logicalIndex = header->logicalIndexAt(headerPos);
    if (logicalIndex < 0) {
        return false;
    }

    const int sectionStart = header->sectionViewportPosition(logicalIndex);
    const int sectionEnd = sectionStart + header->sectionSize(logicalIndex);
    const int cursorPos = header->orientation() == Qt::Horizontal ? headerPos.x() : headerPos.y();
    return std::abs(cursorPos - sectionStart) <= kHeaderResizeCursorMargin ||
            std::abs(cursorPos - sectionEnd) <= kHeaderResizeCursorMargin;
}

void QmlLegacyLibraryItem::maybeApplyHeaderSortFallback(
        QHeaderView* header, const QPoint& rootPos) {
    if (!header || header != m_pPressedHeader || m_pressedHeaderSection < 0) {
        return;
    }
    if (isHeaderResizeHandle(header, rootPos)) {
        return;
    }
    if ((rootPos - m_pressRootPos).manhattanLength() > QApplication::startDragDistance()) {
        return;
    }

    const QPoint headerPos = header->mapFrom(m_pRootWidget.get(), rootPos);
    const int releaseSection = header->logicalIndexAt(headerPos);
    if (releaseSection != m_pressedHeaderSection || !header->sectionsClickable()) {
        return;
    }

    if (header->sortIndicatorSection() != m_pressedHeaderSortSection ||
            header->sortIndicatorOrder() != m_pressedHeaderSortOrder) {
        // sortIndicatorChanged already fired during the press/release cycle,
        // so the native sort path already ran. No fallback needed.
        qDebug() << "QmlLegacyLibraryItem: sort indicator already changed by"
                    " native header handling, skipping fallback";
        return;
    }

    const Qt::SortOrder order = header->sortIndicatorSection() == releaseSection
            ? (header->sortIndicatorOrder() == Qt::AscendingOrder
                              ? Qt::DescendingOrder
                              : Qt::AscendingOrder)
            : Qt::AscendingOrder;

    qDebug() << "QmlLegacyLibraryItem: applying sort fallback on section"
             << releaseSection << "order" << order;
    header->setSortIndicator(releaseSection, order);
    header->update();
}

void QmlLegacyLibraryItem::syncCursorFromWidget(QWidget* target, const QPoint& rootPos) {
    if (!target) {
        unsetCursor();
        return;
    }

    if (auto* header = parentHeaderView(target)) {
        if (isHeaderResizeHandle(header, rootPos)) {
            setCursor(header->orientation() == Qt::Horizontal
                            ? Qt::SplitHCursor
                            : Qt::SplitVCursor);
            return;
        }
    }

    for (QWidget* current = target; current; current = current->parentWidget()) {
        if (current->testAttribute(Qt::WA_SetCursor)) {
            setCursor(current->cursor());
            return;
        }
    }
    unsetCursor();
}

void QmlLegacyLibraryItem::repaintEmbeddedViews() {
    if (!m_pRootWidget) {
        return;
    }

    const auto views = m_pRootWidget->findChildren<QAbstractItemView*>();
    for (QAbstractItemView* view : views) {
        if (view->viewport()) {
            view->viewport()->update();
        }
        view->update();
    }
    update();
}

void QmlLegacyLibraryItem::applyLegacyScrollbarStyle(QScrollBar* scrollBar) {
    if (!scrollBar) {
        return;
    }

    const QString scrollBarStyle = QStringLiteral(R"MIXXXQSS(
QScrollBar {
  border: 0px solid #585858;
  background: #000;
  border-radius: 2px;
  padding: 1px;
  color: #999999;
}
QScrollBar:horizontal {
  min-width: 12px;
  height: 15px;
  border-top-left-radius: 0px;
  border-top-right-radius: 0px;
  background-color: #000;
}
QScrollBar:vertical {
  min-height: 12px;
  width: 15px;
  border-top-left-radius: 0px;
  border-bottom-left-radius: 0px;
  color: #b3b3b3;
  background-color: #000;
}
QScrollBar::groove:horizontal {
  height: 15px;
  background-color: #000;
  border: 0px;
}
QScrollBar::groove:vertical {
  width: 15px;
  background-color: #000;
  border: 0px;
}
QScrollBar::handle:horizontal {
  min-width: 25px;
  border-radius: 2px;
  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #725309, stop:1 #412f05);
}
QScrollBar::handle:vertical {
  min-height: 25px;
  border-radius: 2px;
  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #725309, stop:1 #412f05);
}
QScrollBar::add-page, QScrollBar::sub-page {
  min-width: 15px;
  min-height: 15px;
  background-color: #000;
  border-radius: 2px;
}
QScrollBar::add-line, QScrollBar::sub-line {
  width: 0px;
  height: 0px;
  border: 0px;
}
)MIXXXQSS");

    scrollBar->setAttribute(Qt::WA_StyledBackground, true);
    scrollBar->setAutoFillBackground(true);
    scrollBar->setStyleSheet(scrollBarStyle);
    if (QStyle* style = scrollBar->style()) {
        style->unpolish(scrollBar);
        style->polish(scrollBar);
    }
    scrollBar->ensurePolished();
    scrollBar->update();
}

void QmlLegacyLibraryItem::applyLegacyScrollbarStyles() {
    if (!m_pRootWidget) {
        return;
    }

    const auto scrollBars = m_pRootWidget->findChildren<QScrollBar*>();
    for (QScrollBar* scrollBar : scrollBars) {
        applyLegacyScrollbarStyle(scrollBar);
    }
}

void QmlLegacyLibraryItem::repolishEmbeddedWidgets() {
    if (!m_pRootWidget) {
        return;
    }

    QList<QWidget*> widgets = m_pRootWidget->findChildren<QWidget*>();
    widgets.prepend(m_pRootWidget.get());
    for (QWidget* widget : widgets) {
        if (QStyle* style = widget->style()) {
            style->unpolish(widget);
            style->polish(widget);
        }
        widget->ensurePolished();
        widget->update();
    }
}

void QmlLegacyLibraryItem::enableEmbeddedWidgetInputTracking() {
    if (!m_pRootWidget) {
        return;
    }

    QList<QWidget*> widgets = m_pRootWidget->findChildren<QWidget*>();
    widgets.prepend(m_pRootWidget.get());
    for (QWidget* widget : widgets) {
        widget->setMouseTracking(true);
        widget->setAttribute(Qt::WA_Hover, true);
        widget->setAttribute(Qt::WA_NoMousePropagation, false);
    }
}

void QmlLegacyLibraryItem::connectSortBypass() {
    if (!m_pRootWidget) {
        return;
    }

    // WTrackTableView::applySortingIfVisible() bails because isVisible()
    // returns false for our WA_DontShowOnScreen widget tree. We bypass this
    // by directly connecting each table header's sortIndicatorChanged signal
    // to QTableView::sortByColumn (public, inherited), which does the actual
    // model sort. Use Qt::UniqueConnection so this is idempotent across
    // repeated calls from view-switch signals.
    const auto tableViews = m_pRootWidget->findChildren<QTableView*>();
    for (QTableView* tableView : tableViews) {
        QHeaderView* header = tableView->horizontalHeader();
        if (!header) {
            continue;
        }
        connect(header,
                &QHeaderView::sortIndicatorChanged,
                tableView,
                &QTableView::sortByColumn,
                Qt::UniqueConnection);
    }
}

void QmlLegacyLibraryItem::mousePressEvent(QMouseEvent* event) {
    const QPoint rootPos = event->position().toPoint();
    QWidget* target = eventTargetFor(widgetAtRootPos(rootPos));
    sendSyntheticMouseMoveToWidget(target, rootPos, event->globalPosition(), event->modifiers());
    target = eventTargetFor(widgetAtRootPos(rootPos));
    m_pPressedWidget = target;
    m_pGrabbedWidget = target;
    m_pressedButtons = event->buttons();
    m_pressRootPos = rootPos;
    m_pPressedHeader.clear();
    m_pressedHeaderSection = -1;
    if (event->button() == Qt::LeftButton) {
        if (auto* header = parentHeaderView(target)) {
            if (!isHeaderResizeHandle(header, rootPos)) {
                m_pPressedHeader = header;
                m_pressedHeaderSection = header->logicalIndexAt(
                        header->mapFrom(m_pRootWidget.get(), rootPos));
                m_pressedHeaderSortSection = header->sortIndicatorSection();
                m_pressedHeaderSortOrder = header->sortIndicatorOrder();
            }
        }
    }

    if (sendMouseToWidget(event, target)) {
        repaintEmbeddedViews();
    } else {
        m_pPressedWidget.clear();
        m_pGrabbedWidget.clear();
        QQuickPaintedItem::mousePressEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseReleaseEvent(QMouseEvent* event) {
    const QPoint rootPos = event->position().toPoint();
    QWidget* target = m_pGrabbedWidget
            ? m_pGrabbedWidget.data()
            : eventTargetFor(widgetAtRootPos(rootPos));
    const bool accepted = sendMouseToWidget(event, target);
    if (event->button() == Qt::LeftButton) {
        maybeApplyHeaderSortFallback(parentHeaderView(target), rootPos);
    }
    if (accepted) {
        repaintEmbeddedViews();
    } else {
        QQuickPaintedItem::mouseReleaseEvent(event);
    }
    m_pPressedWidget.clear();
    m_pGrabbedWidget.clear();
    m_pPressedHeader.clear();
    m_pressedHeaderSection = -1;
    m_pressedButtons = Qt::NoButton;
}

void QmlLegacyLibraryItem::mouseMoveEvent(QMouseEvent* event) {
    QWidget* target = m_pGrabbedWidget
            ? m_pGrabbedWidget.data()
            : eventTargetFor(widgetAtRootPos(event->position().toPoint()));
    if (sendMouseToWidget(event, target)) {
        update();
    } else {
        QQuickPaintedItem::mouseMoveEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseDoubleClickEvent(QMouseEvent* event) {
    const QPoint rootPos = event->position().toPoint();
    QWidget* target = eventTargetFor(widgetAtRootPos(rootPos));
    sendSyntheticMouseMoveToWidget(target, rootPos, event->globalPosition(), event->modifiers());
    target = eventTargetFor(widgetAtRootPos(rootPos));
    m_pPressedWidget = target;
    m_pGrabbedWidget = target;
    if (sendMouseToWidget(event, target)) {
        repaintEmbeddedViews();
    } else {
        QQuickPaintedItem::mouseDoubleClickEvent(event);
    }
}

void QmlLegacyLibraryItem::wheelEvent(QWheelEvent* event) {
    if (sendWheelToWidget(event)) {
        update();
    } else {
        QQuickPaintedItem::wheelEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverEnterEvent(QHoverEvent* event) {
    if (sendHoverToWidget(event)) {
        update();
    } else {
        QQuickPaintedItem::hoverEnterEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverMoveEvent(QHoverEvent* event) {
    if (sendHoverToWidget(event)) {
        update();
    } else {
        QQuickPaintedItem::hoverMoveEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverLeaveEvent(QHoverEvent* event) {
    if (m_pLastHoverWidget) {
        QEvent leaveEvent(QEvent::Leave);
        QApplication::sendEvent(m_pLastHoverWidget, &leaveEvent);
        m_pLastHoverWidget.clear();
    }
    unsetCursor();
    update();
    QQuickPaintedItem::hoverLeaveEvent(event);
}

void QmlLegacyLibraryItem::updateWidgetSize() {
    if (!m_pRootWidget) {
        return;
    }

    const QSize widgetSize(
            qMax(1, qRound(width())),
            qMax(1, qRound(height())));
    if (m_pRootWidget->size() == widgetSize) {
        return;
    }

    m_pRootWidget->resize(widgetSize);
    m_pRootWidget->ensurePolished();
}

void QmlLegacyLibraryItem::applyLegacyLibrarySkinConfiguration() {
    if (!m_pLibraryWidget) {
        return;
    }

    const QString resourcePath = QmlConfigProxy::get()->getResourcePath();
    const QString lateNightSkinPath = resourcePath + QStringLiteral("skins/LateNight");

    SkinContext context(QmlConfigProxy::get(), lateNightSkinPath + QStringLiteral("/skin.xml"));
    context.setSkinBasePath(lateNightSkinPath);

    QDomDocument document(QStringLiteral("QmlLegacyLibraryItemLibrarySetup"));
    const QString libraryXml = QStringLiteral(
            "<Library>"
            "<ShowButtonText>false</ShowButtonText>"
            "<TrackTableBackgroundColorOpacity>0.175</TrackTableBackgroundColorOpacity>"
            "<SignalColor>#e7c413</SignalColor>"
            "</Library>");
    const QDomDocument::ParseResult parseResult = document.setContent(libraryXml);
    if (!parseResult) {
        qWarning() << "QmlLegacyLibraryItem: failed to parse library skin setup"
                   << parseResult.errorMessage << parseResult.errorLine
                   << parseResult.errorColumn;
        return;
    }

    m_pLibraryWidget->setup(document.documentElement(), context);
}

// [PoC hack] Loads style_classic.qss from the LateNight skin directory and
// applies it to the root widget so that the embedded QWidget tree picks up
// SVG branch arrows, preview button icons, and colour tokens.
//
// The legacy QSS uses a custom "skins:" URL scheme that only LegacySkinParser
// knows how to resolve.  We emulate it with a simple string replacement that
// expands "skins:" to the absolute skins/ directory path.
//
// TODO(GSoC): This whole method can be deleted once the library panel is
// ported to QML. At that point styling is handled by the "skin:" image
// provider (QQuickAsyncImageProvider subclass) and pure QML property bindings,
// which is the architecture described in the GSoC proposal.
void QmlLegacyLibraryItem::applyLegacyStylesheet() {
    const QString resourcePath =
            QmlConfigProxy::get()->getResourcePath();
    const QString skinsRoot = QDir::fromNativeSeparators(
            resourcePath + QStringLiteral("skins/"));
    const QString lateNightSkinRoot = QDir::fromNativeSeparators(
            resourcePath + QStringLiteral("skins/LateNight"));
    QDir::setSearchPaths(QStringLiteral("skins"), {skinsRoot});
    QDir::setSearchPaths(QStringLiteral("skin"), {lateNightSkinRoot});
    const QString styleFilePath =
            skinsRoot + QStringLiteral("LateNight/style_classic.qss");

    QFile styleFile(styleFilePath);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "QmlLegacyLibraryItem: could not open" << styleFilePath
                   << "- library will have no custom styling";
        return;
    }

    QString style = QString::fromUtf8(styleFile.readAll());

    // Resolve the "skins:" URL alias used throughout the QSS file.
    // LegacySkinParser does the same replacement in processStyleNodes().
    style.replace(QStringLiteral("url(skins:"),
            QStringLiteral("url(") + skinsRoot);
    style.replace(QStringLiteral("url(\"skins:"),
            QStringLiteral("url(\"") + skinsRoot);
    style.replace(QStringLiteral("url('skins:"),
            QStringLiteral("url('") + skinsRoot);

    // In the offscreen bridge Qt sometimes falls back to the SVG viewBox size
    // for QHeaderView sort subcontrols. Pin the indicator size to match the
    // native LateNight header instead of rendering a tiny dot.
    style.append(QStringLiteral(
            "\nWTrackTableViewHeader::up-arrow,"
            "\nWTrackTableViewHeader::down-arrow {"
            "\n  width: 14px;"
            "\n  height: 14px;"
            "\n}"));

    m_pRootWidget->setStyleSheet(style);
    qDebug() << "QmlLegacyLibraryItem: applied LateNight legacy stylesheet";
}

void QmlLegacyLibraryItem::initializeOverviewTypeControl() {
    // In legacy mode, DlgPrefWaveform creates a ControlPushButton for
    // [Waveform],WaveformOverviewType and seeds it from the config file.
    // In QML mode that dialog is never constructed, so the CO does not
    // exist. OverviewDelegate tries to read it and falls back to 0
    // (= Filtered), which explains the yellow single-colour overviews.
    //
    // We create the CO here, before WLibrary delegates are constructed
    // (bindLibraryWidget), so the delegate sees the correct RGB default.
    UserSettingsPointer pConfig = QmlConfigProxy::get();
    const ConfigKey overviewTypeCfgKey(
            QStringLiteral("[Waveform]"),
            QStringLiteral("WaveformOverviewType"));

    m_pOverviewTypeControl = std::make_unique<ControlPushButton>(overviewTypeCfgKey);
    m_pOverviewTypeControl->setStates(
            QMetaEnum::fromType<mixxx::OverviewType>().keyCount());
    m_pOverviewTypeControl->setReadOnly();

    // Seed from config, defaulting to RGB.
    mixxx::OverviewType overviewType = pConfig->getValue<mixxx::OverviewType>(
            overviewTypeCfgKey, mixxx::OverviewType::RGB);
    m_pOverviewTypeControl->forceSet(static_cast<double>(overviewType));
    qDebug() << "QmlLegacyLibraryItem: initialized WaveformOverviewType CO to"
             << static_cast<int>(overviewType);
}

} // namespace qml
} // namespace mixxx
