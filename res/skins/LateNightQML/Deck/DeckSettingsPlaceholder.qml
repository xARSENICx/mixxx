import QtQuick
import QtQuick.Layouts
import "../LateNightTheme"

Item {
    id: root

    implicitWidth: 76
    implicitHeight: 63

    ColumnLayout {
        anchors.fill: parent
        spacing: 2

        Row {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 12
            spacing: 8

            Repeater {
                model: 5

                delegate: Rectangle {
                    width: 3
                    height: 3
                    radius: 1.5
                    color: "#918273"
                    opacity: 0.85
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            columns: 3
            rowSpacing: 0
            columnSpacing: 0

            Repeater {
                model: [
                    LateNightTheme.assetDeckSlipButton,
                    LateNightTheme.assetDeckQuantizeButton,
                    LateNightTheme.assetDeckKeyMatchButton,
                    LateNightTheme.assetDeckEjectButton,
                    LateNightTheme.assetDeckRepeatButton,
                    LateNightTheme.assetDeckKeylockButton
                ]

                delegate: LegacyIconButton {
                    Layout.preferredWidth: 21
                    Layout.preferredHeight: 18
                    backgroundSource: LateNightTheme.legacyTopRegionButton("square")
                    iconSource: modelData
                    contentOpacity: 0.72
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
