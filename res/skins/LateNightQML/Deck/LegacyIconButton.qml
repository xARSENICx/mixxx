import QtQuick
import "../LateNightTheme"

Item {
    id: root

    property url backgroundSource: LateNightTheme.legacySubRegionButton("square")
    property url iconSource: ""
    property string label: ""
    property color labelColor: LateNightTheme.textColorMuted
    property int labelPixelSize: 11
    property real contentOpacity: 0.82

    implicitWidth: 26
    implicitHeight: 26

    Image {
        anchors.fill: parent
        source: root.backgroundSource
        fillMode: Image.Stretch
        opacity: root.contentOpacity
    }

    Image {
        anchors.centerIn: parent
        width: Math.min(parent.width, sourceSize.width > 0 ? sourceSize.width / 2 : parent.width)
        height: Math.min(parent.height, sourceSize.height > 0 ? sourceSize.height / 2 : parent.height)
        source: root.iconSource
        fillMode: Image.PreserveAspectFit
        opacity: root.contentOpacity
        visible: root.iconSource.toString().length > 0
    }

    Text {
        anchors.centerIn: parent
        text: root.label
        font.family: "Open Sans"
        font.pixelSize: root.labelPixelSize
        font.bold: true
        color: root.labelColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: root.label.length > 0
    }
}
