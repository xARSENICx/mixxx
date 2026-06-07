import QtQuick
import QtQuick.Layouts
import "../LateNightTheme"

Item {
    id: root

    property string valueText: "4"
    property int preferredWidth: 78

    implicitWidth: preferredWidth
    implicitHeight: 26

    Image {
        anchors.fill: parent
        source: LateNightTheme.legacySubRegionButton("wide")
        fillMode: Image.Stretch
        opacity: 0.74
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 2
        spacing: 2

        Text {
            Layout.fillWidth: true
            text: root.valueText
            font.family: "Open Sans"
            font.pixelSize: 13
            font.bold: true
            color: LateNightTheme.accentColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Column {
            Layout.preferredWidth: 14
            Layout.alignment: Qt.AlignVCenter
            spacing: -2

            Text {
                width: parent.width
                text: "▲"
                font.pixelSize: 8
                color: LateNightTheme.textColorMuted
                horizontalAlignment: Text.AlignHCenter
            }

            Text {
                width: parent.width
                text: "▼"
                font.pixelSize: 8
                color: LateNightTheme.textColorMuted
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
