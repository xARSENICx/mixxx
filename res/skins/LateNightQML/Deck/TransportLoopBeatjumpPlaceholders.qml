import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"

Item {
    id: root

    required property string group
    property bool show8Hotcues: true
    property bool showBeatjumpControls: true
    property bool showHotcues: true
    property bool showIntroOutroCues: true
    property bool showLoopControls: true

    height: 52

    Mixxx.ControlProxy {
        id: beatloopSizeProxy
        group: root.group
        key: "beatloop_size"
    }

    Mixxx.ControlProxy {
        id: beatjumpSizeProxy
        group: root.group
        key: "beatjump_size"
    }

    function beatSizeText(value) {
        if (value >= 1) {
            return value.toFixed(0);
        }
        return value.toString();
    }

    RowLayout {
        anchors.fill: parent
        spacing: 6

        GridLayout {
            columns: 2
            rows: 2
            rowSpacing: 0
            columnSpacing: 0
            Layout.preferredWidth: 68
            Layout.preferredHeight: 52

            LegacyIconButton {
                Layout.preferredWidth: 42
                Layout.preferredHeight: 26
                backgroundSource: LateNightTheme.legacySubRegionButton("medium")
                iconSource: LateNightTheme.assetDeckCueButton
                contentOpacity: 0.95
            }

            LegacyIconButton {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                iconSource: LateNightTheme.assetDeckReverseButton
                contentOpacity: 0.82
            }

            LegacyIconButton {
                Layout.columnSpan: 2
                Layout.preferredWidth: 68
                Layout.preferredHeight: 26
                backgroundSource: LateNightTheme.legacySubRegionButton("play")
                iconSource: LateNightTheme.assetDeckPlayButton
                contentOpacity: 0.82
            }
        }

        Item {
            Layout.preferredWidth: 4
        }

        GridLayout {
            columns: root.show8Hotcues ? 4 : 2
            rows: 2
            rowSpacing: 0
            columnSpacing: 0
            Layout.preferredWidth: root.show8Hotcues ? 104 : 52
            Layout.preferredHeight: 52
            visible: root.showHotcues

            Repeater {
                model: root.show8Hotcues ? 8 : 4

                delegate: LegacyIconButton {
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26
                    label: (index + 1).toString()
                    labelPixelSize: 11
                    labelColor: "#918273"
                    contentOpacity: 0.74
                }
            }
        }

        Item {
            Layout.preferredWidth: 4
        }

        GridLayout {
            columns: 2
            rows: 2
            rowSpacing: 0
            columnSpacing: 0
            Layout.preferredWidth: 52
            Layout.preferredHeight: 52
            visible: root.showIntroOutroCues

            Repeater {
                model: [
                    LateNightTheme.assetDeckIntroStartButton,
                    LateNightTheme.assetDeckIntroEndButton,
                    LateNightTheme.assetDeckOutroStartButton,
                    LateNightTheme.assetDeckOutroEndButton
                ]

                delegate: LegacyIconButton {
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26
                    iconSource: modelData
                    contentOpacity: 0.72
                }
            }
        }

        Item {
            Layout.preferredWidth: 8
        }

        GridLayout {
            columns: 4
            rows: 2
            rowSpacing: 0
            columnSpacing: 0
            Layout.preferredWidth: 104
            Layout.preferredHeight: 52
            visible: root.showLoopControls

            LegacyIconButton {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                iconSource: LateNightTheme.assetDeckLoopButton
                contentOpacity: 0.82
            }

            BeatSpinBoxPlaceholder {
                Layout.columnSpan: 3
                Layout.preferredWidth: 78
                Layout.preferredHeight: 26
                valueText: root.beatSizeText(beatloopSizeProxy.value)
            }

            Repeater {
                model: [
                    LateNightTheme.assetDeckReloopButton,
                    LateNightTheme.assetDeckLoopInButton,
                    LateNightTheme.assetDeckLoopOutButton,
                    LateNightTheme.assetDeckLoopAnchorStartButton
                ]

                delegate: LegacyIconButton {
                    Layout.preferredWidth: 26
                    Layout.preferredHeight: 26
                    iconSource: modelData
                    contentOpacity: 0.78
                }
            }
        }

        Item {
            Layout.preferredWidth: 8
        }

        GridLayout {
            columns: 2
            rows: 2
            rowSpacing: 0
            columnSpacing: 0
            Layout.preferredWidth: 60
            Layout.preferredHeight: 52
            visible: root.showBeatjumpControls

            BeatSpinBoxPlaceholder {
                Layout.columnSpan: 2
                Layout.preferredWidth: 60
                Layout.preferredHeight: 26
                preferredWidth: 60
                valueText: root.beatSizeText(beatjumpSizeProxy.value)
            }

            LegacyIconButton {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                iconSource: LateNightTheme.assetDeckBeatjumpLeftButton
                contentOpacity: 0.82
            }

            LegacyIconButton {
                Layout.preferredWidth: 26
                Layout.preferredHeight: 26
                iconSource: LateNightTheme.assetDeckBeatjumpRightButton
                contentOpacity: 0.82
            }
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
