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

    property string activeBackgroundSuffix: ""
    property string activeIconSuffix: ""
    property color activeColor: "transparent"
    property bool activeState: false

    property bool stretchIcon: false

    readonly property url effectiveBackgroundSource: {
        var src = backgroundSource.toString();
        if (root.activeState && activeBackgroundSuffix.length > 0 && src.endsWith(".svg")) {
            return src.substring(0, src.length - 4) + "_" + activeBackgroundSuffix + ".svg";
        }
        return backgroundSource;
    }

    readonly property url effectiveIconSource: {
        var src = iconSource.toString();
        if (root.activeState && activeIconSuffix.length > 0 && src.endsWith(".svg")) {
            return src.substring(0, src.length - 4) + "_" + activeIconSuffix + ".svg";
        }
        return iconSource;
    }

    implicitWidth: 26
    implicitHeight: 26

    Rectangle {
        anchors.fill: parent
        anchors.margins: 1
        radius: 2
        color: root.activeColor
        visible: root.activeState && root.activeColor.toString() !== "#00000000" && root.activeColor.toString() !== "transparent"
    }

    Image {
        anchors.fill: parent
        source: root.effectiveBackgroundSource
        fillMode: Image.Stretch
        opacity: root.contentOpacity
    }

    Image {
        id: iconImage
        anchors.fill: root.stretchIcon ? parent : undefined
        anchors.centerIn: root.stretchIcon ? undefined : parent
        width: root.stretchIcon ? parent.width : Math.min(parent.width, sourceSize.width > 0 ? sourceSize.width / 2 : parent.width)
        height: root.stretchIcon ? parent.height : Math.min(parent.height, sourceSize.height > 0 ? sourceSize.height / 2 : parent.height)
        source: root.effectiveIconSource
        fillMode: root.stretchIcon ? Image.Stretch : Image.PreserveAspectFit
        opacity: root.contentOpacity
        visible: root.effectiveIconSource.toString().length > 0
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
