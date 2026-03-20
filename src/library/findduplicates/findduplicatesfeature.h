#pragma once

#include <QObject>
#include <QVariant>

#include "library/libraryfeature.h"
#include "library/treeitemmodel.h"
#include "preferences/usersettings.h"
#include "util/parented_ptr.h"

class DlgFindDuplicates;

class FindDuplicatesFeature : public LibraryFeature {
    Q_OBJECT
  public:
    FindDuplicatesFeature(Library* pLibrary,
            UserSettingsPointer pConfig);
    ~FindDuplicatesFeature() override = default;

    QVariant title() override {
        return m_title;
    }

    void bindLibraryWidget(WLibrary* libraryWidget,
            KeyboardEventFilter* keyboard) override;

    TreeItemModel* sidebarModel() const override;

  public slots:
    void activate() override;

  private:
    const QString m_title;
    parented_ptr<TreeItemModel> m_pSidebarModel;
    DlgFindDuplicates* m_pDlgFindDuplicates;
};
