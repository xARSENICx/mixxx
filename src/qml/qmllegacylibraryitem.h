#pragma once

#include <QQmlEngine>
#include <QQuickPaintedItem>
#include <QWidget>
#include <memory>

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
    ~QmlLegacyLibraryItem() override = default;

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

    std::unique_ptr<QWidget> m_pRootWidget;

    // Non-owning pointers (owned by m_pRootWidget's widget tree)
    WLibrary* m_pLibraryWidget = nullptr;
    WLibrarySidebar* m_pSidebar = nullptr;
    WSearchLineEdit* m_pSearchLineEdit = nullptr;
};

} // namespace qml
} // namespace mixxx
