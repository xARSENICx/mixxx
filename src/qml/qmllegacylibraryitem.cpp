#include "qml/qmllegacylibraryitem.h"

#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSplitter>
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
bool forwardMouseEventToWidget(QWidget* root, QMouseEvent* event, QQuickItem* item) {
    if (!root) {
        return false;
    }

    QWidget* child = root->childAt(event->position().toPoint());
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
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this)) {
        update(); // Trigger repaint
    } else {
        QQuickPaintedItem::mousePressEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseReleaseEvent(QMouseEvent* event) {
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this)) {
        update();
    } else {
        QQuickPaintedItem::mouseReleaseEvent(event);
    }
}

void QmlLegacyLibraryItem::mouseMoveEvent(QMouseEvent* event) {
    if (forwardMouseEventToWidget(m_pRootWidget.get(), event, this)) {
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

} // namespace qml
} // namespace mixxx
