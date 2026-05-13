#include "qml/qmllegacylibraryitem.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>

#include "library/library.h"
#include "moc_qmllegacylibraryitem.cpp"
#include "qml/qmlconfigproxy.h"
#include "qml/qmllibraryproxy.h"
#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"
#include "widget/wsearchlineedit.h"

namespace mixxx {
namespace qml {

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

    // 4. Add to splitter
    pSplitter->addWidget(pSidebarPage);
    pSplitter->addWidget(m_pLibraryWidget);
    pSplitter->setSizes({200, 600});

    // 5. Root layout
    auto* pRootLayout = new QVBoxLayout(m_pRootWidget.get());
    pRootLayout->setContentsMargins(0, 0, 0, 0);
    pRootLayout->addWidget(pSplitter);

    // 6. Bind to Library singleton
    Library* pLibrary = QmlLibraryProxy::get();
    if (pLibrary) {
        pLibrary->bindSearchboxWidget(m_pSearchLineEdit);
        pLibrary->bindSidebarWidget(m_pSidebar);
        pLibrary->bindLibraryWidget(m_pLibraryWidget, QmlLibraryProxy::getKeyboard());

        // 7. Trigger repaints on visual changes
        connect(pLibrary, &Library::switchToView, this, [this]() { update(); });
        connect(pLibrary, &Library::showTrackModel, this, [this]() { update(); });

        // Initialize default view to Tracks collection to avoid black screen
        pLibrary->searchTracksInCollection();
    } else {
        qWarning() << "QmlLegacyLibraryItem: Library singleton not available!";
    }

    // 8. [PoC hack] Apply the LateNight classic stylesheet so the embedded
    //    QWidget tree renders branch arrows, preview button icons, and other
    //    SVG-based decorations that are normally applied by LegacySkinParser.
    //    TODO(GSoC): Replace with the QQuickAsyncImageProvider "skin:" scheme
    //    and QML palette bindings once the library panel is ported to QML.
    applyLegacyStylesheet();

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
// Forward a mouse event to the correct child widget.
// pGrabbedWidget: if non-null, the widget that received the press event;
// subsequent move/release events bypass childAt() and go directly to it,
// emulating Qt's native implicit mouse grab during drag operations.
bool forwardMouseEventToWidget(QWidget* root,
        QMouseEvent* event,
        QQuickItem* item,
        QWidget* pGrabbedWidget = nullptr) {
    if (!root) {
        return false;
    }

    QWidget* child = pGrabbedWidget ? pGrabbedWidget : root->childAt(event->position().toPoint());
    if (!child) {
        child = root;
    }

    QPoint childPos = child->mapFrom(root, event->position().toPoint());

    QMouseEvent mappedEvent(
            event->type(),
            childPos,
            event->globalPosition(),
            event->button(),
            event->buttons(),
            event->modifiers());

    bool accepted = QApplication::sendEvent(child, &mappedEvent);
    event->setAccepted(accepted);

    if (child->testAttribute(Qt::WA_SetCursor)) {
        item->setCursor(child->cursor());
    } else {
        item->unsetCursor();
    }

    return accepted;
}

bool forwardWheelEventToWidget(QWidget* root, QWheelEvent* event) {
    if (!root) {
        return false;
    }

    QWidget* child = root->childAt(event->position().toPoint());
    if (!child) {
        child = root;
    }

    QPoint childPos = child->mapFrom(root, event->position().toPoint());

    QWheelEvent mappedEvent(
            childPos,
            event->globalPosition(),
            event->pixelDelta(),
            event->angleDelta(),
            event->buttons(),
            event->modifiers(),
            event->phase(),
            event->inverted());

    bool accepted = QApplication::sendEvent(child, &mappedEvent);
    event->setAccepted(accepted);
    return accepted;
}

bool forwardHoverEventToWidget(QWidget* root, QHoverEvent* event, QQuickItem* item) {
    if (!root) {
        return false;
    }

    QWidget* child = root->childAt(event->position().toPoint());
    if (!child) {
        child = root;
    }

    QPoint childPos = child->mapFrom(root, event->position().toPoint());
    QPoint oldChildPos = child->mapFrom(root, event->oldPos());

    QHoverEvent mappedEvent(
            event->type(),
            childPos,
            event->globalPosition(),
            oldChildPos,
            event->modifiers());

    bool accepted = QApplication::sendEvent(child, &mappedEvent);
    event->setAccepted(accepted);

    if (child->testAttribute(Qt::WA_SetCursor)) {
        item->setCursor(child->cursor());
    } else {
        item->unsetCursor();
    }

    return accepted;
}
} // namespace

void QmlLegacyLibraryItem::mousePressEvent(QMouseEvent* event) {
    // Record which child widget received the press so that the entire
    // drag gesture (move + release) is routed to the same widget.
    QWidget* child = m_pRootWidget
            ? m_pRootWidget->childAt(event->position().toPoint())
            : nullptr;
    m_pGrabbedWidget = child ? child : m_pRootWidget.get();

    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this, m_pGrabbedWidget)) {
        update();
    } else {
        m_pGrabbedWidget = nullptr;
        QQuickPaintedItem::mousePressEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseReleaseEvent(QMouseEvent* event) {
    QWidget* grabbed = m_pGrabbedWidget;
    m_pGrabbedWidget = nullptr; // Clear before forwarding
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this, grabbed)) {
        update();
    } else {
        QQuickPaintedItem::mouseReleaseEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseMoveEvent(QMouseEvent* event) {
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this, m_pGrabbedWidget)) {
        update();
    } else {
        QQuickPaintedItem::mouseMoveEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseDoubleClickEvent(QMouseEvent* event) {
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this)) {
        update();
    } else {
        QQuickPaintedItem::mouseDoubleClickEvent(event);
    }
}

void QmlLegacyLibraryItem::wheelEvent(QWheelEvent* event) {
    if (forwardWheelEventToWidget(m_pRootWidget.get(), event)) {
        update();
    } else {
        QQuickPaintedItem::wheelEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverEnterEvent(QHoverEvent* event) {
    if (forwardHoverEventToWidget(m_pRootWidget.get(), event, this)) {
        update();
    } else {
        QQuickPaintedItem::hoverEnterEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverMoveEvent(QHoverEvent* event) {
    if (forwardHoverEventToWidget(m_pRootWidget.get(), event, this)) {
        update();
    } else {
        QQuickPaintedItem::hoverMoveEvent(event);
    }
}

void QmlLegacyLibraryItem::hoverLeaveEvent(QHoverEvent* event) {
    if (forwardHoverEventToWidget(m_pRootWidget.get(), event, this)) {
        update();
    } else {
        QQuickPaintedItem::hoverLeaveEvent(event);
    }
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
    const QString skinsRoot = resourcePath + QStringLiteral("skins/");
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

    m_pRootWidget->setStyleSheet(style);
    qDebug() << "QmlLegacyLibraryItem: applied LateNight legacy stylesheet";
}

} // namespace qml
} // namespace mixxx
