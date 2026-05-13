#pragma once

#include <QPointer>
#include <QQmlEngine>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QWidget>
#include <memory>

class ControlProxy;
class ControlPushButton;
class QAbstractItemView;
class QHeaderView;
class QScrollBar;
class WLibrary;
class WLibrarySidebar;
class WSearchLineEdit;

namespace mixxx {
namespace qml {

class QmlLegacyLibraryItem : public QQuickPaintedItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(LegacyLibraryItem)

  public:
    explicit QmlLegacyLibraryItem(QQuickItem* parent = nullptr);
    ~QmlLegacyLibraryItem() override;

    void paint(QPainter* pPainter) override;

  protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void hoverEnterEvent(QHoverEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;

    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

  private:
    void updateWidgetSize();
    void applyLegacyStylesheet();
    void initializeOverviewTypeControl();
    void applyLegacyLibrarySkinConfiguration();
    void enableEmbeddedWidgetInputTracking();
    QWidget* widgetAtRootPos(const QPoint& rootPos) const;
    QAbstractItemView* parentItemView(QWidget* widget) const;
    QHeaderView* parentHeaderView(QWidget* widget) const;
    QWidget* eventTargetFor(QWidget* widget) const;
    bool isHeaderResizeHandle(QHeaderView* header, const QPoint& rootPos) const;
    void maybeApplyHeaderSortFallback(QHeaderView* header, const QPoint& rootPos);
    bool sendMouseToWidget(QMouseEvent* event, QWidget* target);
    void sendSyntheticMouseMoveToWidget(QWidget* target,
            const QPoint& rootPos,
            const QPointF& globalPos,
            Qt::KeyboardModifiers modifiers,
            Qt::MouseButtons buttons = Qt::NoButton);
    bool sendWheelToWidget(QWheelEvent* event);
    bool sendHoverToWidget(QHoverEvent* event);
    void updateHoverTarget(QWidget* target, const QPoint& rootPos, Qt::KeyboardModifiers modifiers);
    void syncCursorFromWidget(QWidget* target, const QPoint& rootPos);
    void repaintEmbeddedViews();
    void repolishEmbeddedWidgets();
    void applyLegacyScrollbarStyles();
    void applyLegacyScrollbarStyle(QScrollBar* scrollBar);
    void connectSortBypass();

    std::unique_ptr<QWidget> m_pRootWidget;

    // Non-owning pointers (owned by m_pRootWidget's widget tree)
    WLibrary* m_pLibraryWidget = nullptr;
    WLibrarySidebar* m_pSidebar = nullptr;
    WSearchLineEdit* m_pSearchLineEdit = nullptr;

    // Track the offscreen QWidget mouse state explicitly. Since the visible
    // native window is QQuickWindow, QWidget's implicit grab/cursor machinery
    // cannot escape the hidden widget tree on its own.
    QPointer<QWidget> m_pPressedWidget;
    QPointer<QWidget> m_pGrabbedWidget;
    QPointer<QWidget> m_pLastHoverWidget;
    QPointer<QHeaderView> m_pPressedHeader;
    QPointF m_lastHoverRootPos;
    QPoint m_pressRootPos;
    int m_pressedHeaderSection = -1;
    int m_pressedHeaderSortSection = -1;
    Qt::SortOrder m_pressedHeaderSortOrder = Qt::AscendingOrder;
    Qt::MouseButtons m_pressedButtons = Qt::NoButton;

    std::unique_ptr<ControlProxy> m_pPreviewDeckPlay;
    std::unique_ptr<ControlProxy> m_pPreviewDeckTrackLoaded;

    // Owns the [Waveform],WaveformOverviewType ControlPushButton that is
    // normally created by DlgPrefWaveform. In QML mode that dialog does not
    // exist, so we create and own the CO here so that OverviewDelegate can
    // read the correct overview type (RGB by default).
    std::unique_ptr<ControlPushButton> m_pOverviewTypeControl;
};

} // namespace qml
} // namespace mixxx
