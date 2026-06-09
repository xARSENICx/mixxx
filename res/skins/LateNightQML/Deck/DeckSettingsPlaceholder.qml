import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"

Item {
    id: root

    property string group: ""

    readonly property var deckPlayer: Mixxx.PlayerManager.getPlayer(root.group)
    readonly property var currentTrack: deckPlayer?.currentTrack

    readonly property bool isDeck12: root.group === "[Channel1]" || root.group === "[Channel2]"
    readonly property color starsColor: isDeck12 ? ColorScheme.starsColor12 : ColorScheme.starsColor34

    implicitWidth: 76
    implicitHeight: 63

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 5
        spacing: 2

        Item {
            id: starsContainer
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 75
            Layout.preferredHeight: 15

            Row {
                id: starsRow
                anchors.centerIn: parent
                spacing: 0

                Repeater {
                    model: 5

                    delegate: Item {
                        id: starItem
                        width: 15
                        height: 15

                        readonly property bool isFilled: root.currentTrack && root.currentTrack.stars > index

                        // Star shape
                        Shape {
                            anchors.fill: parent
                            antialiasing: true
                            visible: starItem.isFilled

                            ShapePath {
                                fillColor: root.starsColor
                                strokeColor: "transparent"
                                fillRule: ShapePath.WindingFill

                                startX: 15.0
                                startY: 7.5
                                PathLine { x: 1.4324; y: 11.9084 }
                                PathLine { x: 9.8176; y: 0.3671 }
                                PathLine { x: 9.8176; y: 14.6329 }
                                PathLine { x: 1.4324; y: 3.0916 }
                                PathLine { x: 15.0; y: 7.5 }
                            }
                        }

                        // Diamond shape
                        Shape {
                            anchors.fill: parent
                            antialiasing: true
                            visible: !starItem.isFilled

                            ShapePath {
                                fillColor: root.starsColor
                                strokeColor: "transparent"

                                startX: 6.0
                                startY: 7.5
                                PathLine { x: 7.5; y: 6.0 }
                                PathLine { x: 9.0; y: 7.5 }
                                PathLine { x: 7.5; y: 9.0 }
                                PathLine { x: 6.0; y: 7.5 }
                            }
                        }
                    }
                }
            }
        }

        // Row 1: Slip, Quantize, Curpos (lock match)
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 4

            // Slip mode toggle
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckSlipButton
                stretchIcon: true
                contentOpacity: 0.72
            }

            // Quantize toggle
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckQuantizeButton
                stretchIcon: true
                contentOpacity: 0.72
            }

            // Curpos button
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckBeatCurposButton
                stretchIcon: true
                contentOpacity: 0.72
            }
        }

        // Row 2: Eject, Repeat, Keylock
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 4

            // Eject button: momentary push
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckEjectButton
                stretchIcon: true
                contentOpacity: 0.72
            }

            // Repeat toggle
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckRepeatButton
                stretchIcon: true
                contentOpacity: 0.72
            }

            // Keylock toggle
            LegacyIconButton {
                Layout.preferredWidth: 21
                Layout.preferredHeight: 18
                backgroundSource: LateNightTheme.legacyButton("btn__.svg")
                iconSource: LateNightTheme.assetDeckKeylockButton
                stretchIcon: true
                contentOpacity: 0.72
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
