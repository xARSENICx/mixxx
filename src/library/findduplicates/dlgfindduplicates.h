#pragma once

#include <QWidget>

#include "library/libraryview.h"

class QLabel;
class QTableWidget;
class QVBoxLayout;

class DlgFindDuplicates : public QWidget, public virtual LibraryView {
    Q_OBJECT
  public:
    explicit DlgFindDuplicates(QWidget* parent);
    ~DlgFindDuplicates() override = default;

    void onShow() override;
    bool hasFocus() const override;
    void setFocus() override;
    void onSearch(const QString& text) override;

  private slots:
    void slotKeepThisVersion();
    void slotRemoveDuplicate();
    void slotCompareMasterings();
    void slotKeepBestAll();

  private:
    void setupUi();
    void populateDummyData();
    void showContextMenu(const QPoint& pos);

    QVBoxLayout* m_pLayout;
    QLabel* m_pHeaderLabel;
    QLabel* m_pInfoLabel;
    QTableWidget* m_pTable;
};
