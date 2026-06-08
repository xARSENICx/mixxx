import QtQuick
import QtQuick.Layouts
import "../LateNightTheme"

Item {
    id: root

    implicitWidth: 158
    implicitHeight: 20

    RowLayout {
        anchors.fill: parent
        spacing: 0

        LegacyIconButton {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 20
            backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
            iconSource: LateNightTheme.assetDeckVinylControl0
            contentOpacity: 0.72
        }

        LegacyIconButton {
            Layout.preferredWidth: 46
            Layout.preferredHeight: 20
            backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
            label: "ABS"
            labelPixelSize: 9
            labelColor: LateNightTheme.textColorMuted
            contentOpacity: 0.72
        }

        LegacyIconButton {
            Layout.preferredWidth: 32
            Layout.preferredHeight: 20
            backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
            label: "CUE"
            labelPixelSize: 9
            labelColor: LateNightTheme.textColorMuted
            contentOpacity: 0.72
        }

        LegacyIconButton {
            Layout.preferredWidth: 35
            Layout.preferredHeight: 20
            backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
            label: "PASS"
            labelPixelSize: 9
            labelColor: LateNightTheme.textColorMuted
            contentOpacity: 0.72
        }
    }
}
