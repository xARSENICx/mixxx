import QtQuick
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"

Item {
    id: root

    required property string group
    property string keyText: "--"

    implicitWidth: 111
    implicitHeight: 20

    Mixxx.ControlProxy {
        id: keyProxy
        group: root.group
        key: "key"
    }

    Mixxx.ControlProxy {
        id: visualKeyDistanceProxy
        group: root.group
        key: "visual_key_distance"
    }

    function keyToOpenKeyNumber(key) {
        switch (Math.round(key)) {
        case 1:
        case 22:
            return 1;
        case 8:
        case 17:
            return 2;
        case 3:
        case 24:
            return 3;
        case 10:
        case 19:
            return 4;
        case 5:
        case 14:
            return 5;
        case 12:
        case 21:
            return 6;
        case 7:
        case 16:
            return 7;
        case 2:
        case 23:
            return 8;
        case 9:
        case 18:
            return 9;
        case 4:
        case 13:
            return 10;
        case 11:
        case 20:
            return 11;
        case 6:
        case 15:
            return 12;
        default:
            return 0;
        }
    }

    function scaledKey(key, steps) {
        const roundedKey = Math.round(key);
        if (roundedKey <= 0) {
            return 0;
        }
        const isMajor = roundedKey > 0 && roundedKey < 13;
        const base = isMajor ? 1 : 13;
        const tonic = roundedKey - base;
        return ((tonic + steps + 12) % 12) + base;
    }

    function colorForKey(key) {
        const openKeyNumber = keyToOpenKeyNumber(key);
        const palette = Mixxx.Config.keyColorPalette;
        if (openKeyNumber <= 0 || !palette || palette.length < openKeyNumber) {
            return LateNightTheme.accentColor;
        }
        return palette[openKeyNumber - 1];
    }

    readonly property real keyDistance: visualKeyDistanceProxy.value
    readonly property real splitPoint: Math.max(0, Math.min(1, keyDistance < 0 ? keyDistance + 1 : keyDistance))
    readonly property color stripTopColor: keyDistance < 0 ? colorForKey(keyProxy.value) : colorForKey(scaledKey(keyProxy.value, 1))
    readonly property color stripBottomColor: keyDistance < 0 ? colorForKey(scaledKey(keyProxy.value, -1)) : colorForKey(keyProxy.value)

    LegacyIconButton {
        x: 0
        y: 0
        width: 26
        height: 20
        backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
        label: "♪"
        labelPixelSize: 14
        labelColor: LateNightTheme.textColor
        contentOpacity: 0.78
    }

    Rectangle {
        x: 26
        y: 0
        width: 4
        height: 20

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height * root.splitPoint
            color: root.stripTopColor
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: parent.height - parent.height * root.splitPoint
            color: root.stripBottomColor
        }
    }

    Rectangle {
        x: 30
        y: 0
        width: root.width - 50
        height: 20
        color: "#181818"
        border.color: "#303030"
        border.width: 1

        Text {
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            text: root.keyText.length > 0 ? root.keyText : "--"
            font.family: "Open Sans"
            font.pixelSize: 12
            font.bold: true
            color: LateNightTheme.textColor
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Column {
        x: root.width - 20
        y: 0
        width: 20
        height: 20
        spacing: 0

        LegacyIconButton {
            width: 20
            height: 10
            backgroundSource: LateNightTheme.legacyTopRegionButton("square")
            iconSource: LateNightTheme.assetDeckKeyUpButton
            contentOpacity: 0.72
        }

        LegacyIconButton {
            width: 20
            height: 10
            backgroundSource: LateNightTheme.legacyTopRegionButton("square")
            iconSource: LateNightTheme.assetDeckKeyDownButton
            contentOpacity: 0.72
        }
    }
}
