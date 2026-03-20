#include "library/findduplicates/findduplicatesfeature.h"

#include "controllers/keyboard/keyboardeventfilter.h"
#include "library/findduplicates/dlgfindduplicates.h"
#include "library/library.h"
#include "moc_findduplicatesfeature.cpp"
#include "widget/wlibrary.h"

namespace {

const QString kViewName = QStringLiteral("FindDuplicates");

} // anonymous namespace

FindDuplicatesFeature::FindDuplicatesFeature(
        Library* pLibrary,
        UserSettingsPointer pConfig)
        : LibraryFeature(pLibrary, pConfig, QStringLiteral("findduplicates")),
          m_title(tr("Find Doubles")),
          m_pSidebarModel(make_parented<TreeItemModel>(this)),
          m_pDlgFindDuplicates(nullptr) {
}

void FindDuplicatesFeature::bindLibraryWidget(WLibrary* libraryWidget,
        KeyboardEventFilter* keyboard) {
    m_pDlgFindDuplicates = new DlgFindDuplicates(libraryWidget);
    m_pDlgFindDuplicates->installEventFilter(keyboard);
    libraryWidget->registerView(kViewName, m_pDlgFindDuplicates);
}

TreeItemModel* FindDuplicatesFeature::sidebarModel() const {
    return m_pSidebarModel;
}

void FindDuplicatesFeature::activate() {
    emit switchToView(kViewName);
    emit enableCoverArtDisplay(true);
}
