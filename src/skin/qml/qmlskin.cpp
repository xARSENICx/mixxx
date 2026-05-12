#include "skin/qml/qmlskin.h"

#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QQuickWidget>
#include <QScreen>
#include <QSettings>
#include <QUrl>

#include "coreservices.h"
#include "qml/asyncimageprovider.h"

namespace {

const QString kSkinManifestFileName(QStringLiteral("skin.ini"));
const QString kMainQmlFileName(QStringLiteral("main.qml"));
const QString kSkinGroup(QStringLiteral("Skin"));
const QString kNameKey(QStringLiteral("name"));
const QString kDescriptionKey(QStringLiteral("description"));
const QString kMinPixelWidthKey(QStringLiteral("min_pixel_width"));
const QString kMinPixelHeightKey(QStringLiteral("min_pixel_height"));

} // namespace

namespace mixxx {
namespace skin {
namespace qml {

// static
SkinPointer QmlSkin::fromDirectory(const QDir& dir) {
    if (dir.exists(kSkinManifestFileName) && dir.exists(kMainQmlFileName)) {
        return std::make_shared<QmlSkin>(QFileInfo(dir.absolutePath()));
    }
    return nullptr;
}

QmlSkin::QmlSkin(const QFileInfo& path)
        : m_path(path) {
    DEBUG_ASSERT(isValid());
}

bool QmlSkin::isValid() const {
    return !m_path.filePath().isEmpty() &&
            m_path.exists() &&
            QFileInfo::exists(mainQmlFilePath()) &&
            QFileInfo::exists(skinIniFile().filePath());
}

QFileInfo QmlSkin::path() const {
    DEBUG_ASSERT(isValid());
    return m_path;
}

QPixmap QmlSkin::preview(const QString&) const {
    QPixmap preview(m_path.absoluteFilePath() + QStringLiteral("/skin_preview.png"));
    if (!preview.isNull()) {
        return preview;
    }
    preview.load(":/images/skin_preview_placeholder.png");
    return preview;
}

QString QmlSkin::name() const {
    DEBUG_ASSERT(isValid());
    return m_path.fileName();
}

QString QmlSkin::description() const {
    DEBUG_ASSERT(isValid());
    QSettings skinSettings(skinIniFile().absoluteFilePath(), QSettings::IniFormat);
    skinSettings.beginGroup(kSkinGroup);
    return skinSettings.value(kDescriptionKey).toString();
}

QList<QString> QmlSkin::colorschemes() const {
    return {};
}

bool QmlSkin::fitsScreenSize(const QScreen& screen) const {
    DEBUG_ASSERT(isValid());
    QSettings skinSettings(skinIniFile().absoluteFilePath(), QSettings::IniFormat);
    skinSettings.beginGroup(kSkinGroup);
    const int minPixelWidth = skinSettings.value(kMinPixelWidthKey).toInt();
    const int minPixelHeight = skinSettings.value(kMinPixelHeightKey).toInt();
    if (minPixelWidth <= 0 || minPixelHeight <= 0) {
        return true;
    }
    const QSize screenSize = screen.size();
    return minPixelWidth <= screenSize.width() &&
            minPixelHeight <= screenSize.height();
}

LaunchImage* QmlSkin::loadLaunchImage(QWidget*, UserSettingsPointer) const {
    return nullptr;
}

QWidget* QmlSkin::loadSkin(QWidget* pParent,
        UserSettingsPointer,
        QSet<ControlObject*>*,
        mixxx::CoreServices* pCoreServices) const {
    VERIFY_OR_DEBUG_ASSERT(pCoreServices) {
        return nullptr;
    }

    QQuickStyle::setStyle("Basic");
    qputenv("QT_QUICK_TABLEVIEW_COMPAT_VERSION", "6.4");

    QQuickWidget* pWidget = new QQuickWidget(pParent);
    pWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QQmlEngine* pEngine = pWidget->engine();
    VERIFY_OR_DEBUG_ASSERT(pEngine) {
        delete pWidget;
        return nullptr;
    }

    pEngine->addImportPath(QStringLiteral(":/mixxx.org/imports"));

    QQuickAsyncImageProvider* pImageProvider = new mixxx::qml::AsyncImageProvider(
            pCoreServices->getTrackCollectionManager());
    pEngine->addImageProvider(mixxx::qml::AsyncImageProvider::kProviderName, pImageProvider);

    pWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    pWidget->setSource(QUrl::fromLocalFile(mainQmlFilePath()));
    return pWidget;
}

QString QmlSkin::mainQmlFilePath() const {
    return m_path.absoluteFilePath() + QStringLiteral("/") + kMainQmlFileName;
}

QFileInfo QmlSkin::skinIniFile() const {
    return QFileInfo(m_path.absoluteFilePath() + QStringLiteral("/") + kSkinManifestFileName);
}

} // namespace qml
} // namespace skin
} // namespace mixxx
