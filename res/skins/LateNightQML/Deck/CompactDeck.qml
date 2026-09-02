import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "../Controls" as Controls
import "../LateNightTheme"

Controls.Panel {
    id: root

    required property string group
    readonly property bool showBeatjumpControls: showBeatjumpControlsProxy.value > 0
    readonly property bool showBigSpinnyOrCover: selectBigSpinnyProxy.value > 0
    readonly property bool showKeyControls: showKeyControlsProxy.value > 0
    readonly property bool showLoopControls: showLoopControlsProxy.value > 0
    readonly property bool showRateControls: showRateControlsProxy.value > 0
    readonly property bool showSmallSpinnyOrCover: selectBigSpinnyProxy.value <= 0
    readonly property bool showVinylControls: showVinylControlsProxy.value > 0

    signal toggleFocus

    color: LateNightTheme.deckPanelColor
    implicitHeight: 168
    implicitWidth: 620

    Mixxx.ControlProxy {
        id: selectBigSpinnyProxy

        group: "[Skin]"
        key: "select_big_spinny_or_cover"
    }
    Mixxx.ControlProxy {
        id: showVinylControlsProxy

        group: "[Skin]"
        key: "show_vinylcontrol"
    }
    Mixxx.ControlProxy {
        id: showKeyControlsProxy

        group: "[Skin]"
        key: "show_key_controls_compact"
    }
    Mixxx.ControlProxy {
        id: showLoopControlsProxy

        group: "[Skin]"
        key: "show_loop_controls_compact"
    }
    Mixxx.ControlProxy {
        id: showBeatjumpControlsProxy

        group: "[Skin]"
        key: "show_beatjump_controls_compact"
    }
    Mixxx.ControlProxy {
        id: showRateControlsProxy

        group: "[Skin]"
        key: "show_rate_controls_compact"
    }
    RowLayout {
        anchors.bottomMargin: 1
        anchors.fill: parent
        anchors.leftMargin: 1
        anchors.rightMargin: 1
        anchors.topMargin: 1
        spacing: 2

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 1

            RowLayout {
                Layout.fillWidth: true
                Layout.preferredHeight: 110
                spacing: 2

                SpinnyCoverSlot {
                    Layout.preferredHeight: 109
                    Layout.preferredWidth: 109
                    group: root.group
                    visible: root.showBigSpinnyOrCover
                }
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    spacing: 1

                    TitleTimeRowsCompact {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 46
                        group: root.group

                        TapHandler {
                            onDoubleTapped: root.toggleFocus()
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 63
                        spacing: 1

                        SpinnyCoverSlot {
                            Layout.preferredHeight: 63
                            Layout.preferredWidth: 63
                            group: root.group
                            visible: root.showSmallSpinnyOrCover
                        }
                        OverviewRow {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            group: root.group
                        }
                    }
                }
            }
            TransportLoopBeatjumpCompact {
                Layout.fillWidth: true
                Layout.preferredHeight: 55
                group: root.group
                showBeatjumpControls: root.showBeatjumpControls
                showKeyControls: root.showKeyControls
                showLoopControls: root.showLoopControls
                showVinylControls: root.showVinylControls
            }
        }
        RateCompact {
            Layout.fillHeight: true
            Layout.preferredWidth: root.showRateControls ? 62 : 0
            group: root.group
            visible: root.showRateControls
        }
    }
}
