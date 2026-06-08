import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"
import "../../../qml/Mixxx/Controls" as MixxxControls

Item {
    id: root

    required property string group
    readonly property bool useSecondaryDeckColors: root.group === "[Channel3]" || root.group === "[Channel4]"
    readonly property color overviewBackgroundColor: useSecondaryDeckColors ? LateNightTheme.secondaryOverviewBackgroundColor : LateNightTheme.primaryOverviewBackgroundColor
    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: root.overviewBackgroundColor
            border.color: "#0a0a0a"
            border.width: 1

            Rectangle {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 1
                color: "#333333"
            }

            MixxxControls.WaveformOverview {
                id: waveformOverview

                anchors.fill: parent
                anchors.margins: 1
                group: root.group
                colorLow: "#ff0000"
                colorMid: "#00ff00"
                colorHigh: "#0000ff"
                renderer: Mixxx.WaveformOverview.Renderer.RGB
            }
        }

        // Visual-only Week 4 placeholder for the legacy deck settings grid.
        Rectangle {
            Layout.preferredWidth: 76
            Layout.fillHeight: true
            color: "#19191a"
            border.color: "#0a0a0a"
            border.width: 1

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: "#2a2a2a"
            }

            DeckSettingsPlaceholder {
                anchors.fill: parent
            }
        }
    }
}
