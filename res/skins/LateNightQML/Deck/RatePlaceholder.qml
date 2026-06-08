import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"

Item {
    id: root

    required property string group
    property bool showRateControlButtons: true

    readonly property var deckPlayer: Mixxx.PlayerManager.getPlayer(root.group)
    readonly property bool isLoaded: deckPlayer?.isLoaded ?? false

    implicitWidth: 90
    implicitHeight: 202
    readonly property color rateTextColor: LateNightTheme.accentColor

    Mixxx.ControlProxy {
        id: bpmProxy
        group: root.group
        key: "visual_bpm"
    }

    Mixxx.ControlProxy {
        id: rateProxy
        group: root.group
        key: "rate"
    }

    Mixxx.ControlProxy {
        id: rateDirProxy
        group: root.group
        key: "rate_dir"
    }

    Item {
        id: deckRateSeparator

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 2

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: "#0c0c0c"
        }

        Rectangle {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 1
            color: "#333333"
        }
    }

    ColumnLayout {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: deckRateSeparator.right
        anchors.leftMargin: 2
        spacing: 3

        Text {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            text: root.isLoaded ? bpmProxy.value.toFixed(2) : "--.--"
            font.family: "Open Sans"
            font.pixelSize: 20
            font.bold: true
            color: root.rateTextColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            Layout.fillWidth: true
            Layout.preferredHeight: 17
            text: {
                if (!root.isLoaded) {
                    return "0.00";
                }
                return (rateProxy.value * rateDirProxy.value * 100).toFixed(2);
            }
            font.family: "Open Sans"
            font.pixelSize: 13
            color: root.rateTextColor
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 62
            Layout.preferredHeight: 22
            spacing: 0

            LegacyIconButton {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 22
                backgroundSource: LateNightTheme.legacyTopRegionButton("medium")
                iconSource: LateNightTheme.assetDeckSyncButton
                contentOpacity: 0.82
            }

            LegacyIconButton {
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                backgroundSource: LateNightTheme.legacyTopRegionButton("square")
                iconSource: LateNightTheme.assetDeckLeaderButton
                contentOpacity: 0.72
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 2

            Item {
                id: sliderContainer
                Layout.preferredWidth: 54
                Layout.fillHeight: true
                Layout.minimumHeight: 118

                Text {
                    x: 1
                    y: sliderTrack.y - 3
                    text: "-"
                    font.family: "Open Sans"
                    font.pixelSize: 12
                    color: root.rateTextColor
                }

                Text {
                    width: 8
                    x: parent.width - width - 1
                    y: sliderTrack.y - 4
                    text: "8"
                    font.family: "Open Sans"
                    font.pixelSize: 12
                    color: root.rateTextColor
                    horizontalAlignment: Text.AlignRight
                }

                Text {
                    x: 1
                    y: sliderTrack.y + sliderTrack.height - height + 2
                    text: "+"
                    font.family: "Open Sans"
                    font.pixelSize: 12
                    color: root.rateTextColor
                }

                Text {
                    width: 8
                    x: parent.width - width - 1
                    y: sliderTrack.y + sliderTrack.height - height + 2
                    text: "8"
                    font.family: "Open Sans"
                    font.pixelSize: 12
                    color: root.rateTextColor
                    horizontalAlignment: Text.AlignRight
                }

                Image {
                    id: sliderTrack
                    anchors.centerIn: parent
                    width: 40
                    height: Math.min(119, parent.height - 4)
                    source: LateNightTheme.assetDeckRateSliderBackground
                    fillMode: Image.PreserveAspectFit
                    opacity: 0.82
                }

                Image {
                    id: sliderHandle
                    width: 34
                    height: 18
                    source: LateNightTheme.assetDeckRateSliderHandle
                    fillMode: Image.PreserveAspectFit
                    x: (sliderContainer.width - width) / 2
                    y: (sliderContainer.height - height) / 2
                }
            }

            ColumnLayout {
                Layout.preferredWidth: 26
                Layout.alignment: Qt.AlignVCenter
                spacing: 2
                visible: root.showRateControlButtons

                Repeater {
                    model: [
                        LateNightTheme.assetDeckMinusButton,
                        LateNightTheme.assetDeckArrowLeftUpButton,
                        LateNightTheme.assetDeckArrowRightDownButton,
                        LateNightTheme.assetDeckPlusButton
                    ]

                    delegate: LegacyIconButton {
                        Layout.preferredWidth: 26
                        Layout.preferredHeight: 26
                        backgroundSource: LateNightTheme.legacyTopRegionButton("square")
                        iconSource: modelData
                        contentOpacity: 0.76
                    }
                }
            }
        }
    }
}
