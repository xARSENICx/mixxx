import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx

Rectangle {
    id: root

    implicitHeight: root.minimized ? 80 : 206
    implicitWidth: 620

    required property string group
    property bool minimized: false
    property bool editMode: false
    readonly property var deckPlayer: Mixxx.PlayerManager.getPlayer(root.group)
    readonly property var currentTrack: deckPlayer?.currentTrack
    property bool honorLegacyVisibilityControls: false
    readonly property bool showBeatjumpControls: !honorLegacyVisibilityControls || showBeatjumpControlsProxy.value > 0
    readonly property bool showBigSpinnyOrCover: selectBigSpinnyProxy.value > 0
    readonly property bool showHotcues: !honorLegacyVisibilityControls || showHotcuesProxy.value > 0
    readonly property bool show8Hotcues: !honorLegacyVisibilityControls || show8HotcuesProxy.value > 0
    readonly property bool showIntroOutroCues: !honorLegacyVisibilityControls || showIntroOutroCuesProxy.value > 0
    readonly property bool showKeyControls: !honorLegacyVisibilityControls || showKeyControlsProxy.value > 0
    readonly property bool showLoopControls: !honorLegacyVisibilityControls || showLoopControlsProxy.value > 0
    readonly property bool showRateControlButtons: !honorLegacyVisibilityControls || showRateControlButtonsProxy.value > 0
    readonly property bool showRateControls: !honorLegacyVisibilityControls || showRateControlsProxy.value > 0
    readonly property bool showSmallSpinnyOrCover: selectBigSpinnyProxy.value <= 0 && !root.minimized
    readonly property bool showVinylControls: honorLegacyVisibilityControls && showVinylControlsProxy.value > 0
    readonly property string keyText: currentTrack?.keyText || "--"

    color: "#161616"
    border.color: "#2e2e2e"
    border.width: 1
    radius: 4

    Mixxx.ControlProxy {
        id: selectBigSpinnyProxy
        group: "[Skin]"
        key: "select_big_spinny_or_cover"
    }

    Mixxx.ControlProxy {
        id: showKeyControlsProxy
        group: "[Skin]"
        key: "show_key_controls"
    }

    Mixxx.ControlProxy {
        id: showVinylControlsProxy
        group: "[Skin]"
        key: "show_vinylcontrol"
    }

    Mixxx.ControlProxy {
        id: show4EffectUnitsProxy
        group: "[Skin]"
        key: "show_4effectunits"
    }

    Mixxx.ControlProxy {
        id: showHotcuesProxy
        group: "[Skin]"
        key: "show_hotcues"
    }

    Mixxx.ControlProxy {
        id: show8HotcuesProxy
        group: "[Skin]"
        key: "show_8_hotcues"
    }

    Mixxx.ControlProxy {
        id: showIntroOutroCuesProxy
        group: "[Skin]"
        key: "show_intro_outro_cues"
    }

    Mixxx.ControlProxy {
        id: showLoopControlsProxy
        group: "[Skin]"
        key: "show_loop_controls"
    }

    Mixxx.ControlProxy {
        id: showBeatjumpControlsProxy
        group: "[Skin]"
        key: "show_beatjump_controls"
    }

    Mixxx.ControlProxy {
        id: showRateControlsProxy
        group: "[Skin]"
        key: "show_rate_controls"
    }

    Mixxx.ControlProxy {
        id: showRateControlButtonsProxy
        group: "[Skin]"
        key: "show_rate_control_buttons"
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 2
        spacing: 4

        // Central main deck column
        ColumnLayout {
            id: mainDeckColumn
            Layout.fillWidth: true
            Layout.fillHeight: false
            Layout.alignment: Qt.AlignTop
            spacing: 4

            // Top placeholder row: FX assignment and key controls, visual-only for Week 4.
            RowLayout {
                id: topPlaceholderRow
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                Layout.minimumHeight: 20
                Layout.maximumHeight: 20
                Layout.fillHeight: false
                visible: !root.minimized
                spacing: 2

                // FX assignment buttons visual placeholders
                Row {
                    spacing: 0

                    Repeater {
                        model: show4EffectUnitsProxy.value > 0 ? ["FX1", "2", "3", "4"] : ["FX1", "FX2"]

                        delegate: Rectangle {
                            width: show4EffectUnitsProxy.value > 0 && index > 0 ? 20 : 26
                            height: 20
                            color: "#222222"
                            radius: 1
                            border.color: "#383838"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: modelData
                                font.family: "Open Sans"
                                font.pixelSize: 10
                                font.bold: true
                                color: "#888888"
                            }
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 1
                        color: "#303030"
                    }
                }

                VinylControlsPlaceholder {
                    Layout.preferredWidth: 158
                    Layout.preferredHeight: 20
                    Layout.maximumHeight: 20
                    visible: root.showVinylControls
                }

                Item {
                    Layout.preferredWidth: root.showVinylControls ? 2 : 0
                    Layout.fillHeight: true
                    visible: root.showVinylControls
                }

                KeyControlsPlaceholder {
                    Layout.preferredWidth: 111
                    Layout.maximumWidth: 111
                    Layout.preferredHeight: 20
                    Layout.maximumHeight: 20
                    group: root.group
                    keyText: root.keyText
                    visible: root.showKeyControls
                }
            }

            // Middle Row: Big Spinny on the left, Title/Overview on the right
            RowLayout {
                id: middleDeckRow
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.minimumHeight: root.minimized ? 68 : 122
                Layout.preferredHeight: root.minimized ? 68 : 122
                Layout.maximumHeight: root.minimized ? 68 : 122
                spacing: 8

                // Big Spinny/Cover Slot (Large mode)
                SpinnyCoverSlot {
                    id: leftSpinnyBig
                    Layout.preferredHeight: 114
                    Layout.preferredWidth: 114
                    group: root.group
                    visible: root.showBigSpinnyOrCover && !root.minimized
                }

                // Column containing Title rows and Overview row
                ColumnLayout {
                    id: titleOverviewColumn
                    Layout.fillWidth: true
                    Layout.fillHeight: false
                    Layout.preferredHeight: root.minimized ? 68 : 122
                    spacing: 4

                    // Title, Time, Artist, Duration Rows
                    TitleTimeRows {
                        id: titleTimeRows
                        Layout.fillWidth: true
                        Layout.minimumHeight: root.minimized ? 48 : 55
                        Layout.preferredHeight: root.minimized ? 48 : 55
                        Layout.maximumHeight: root.minimized ? 48 : 55
                        group: root.group
                    }

                    // Row containing Small Spinny (on the left of overview) and the Waveform Overview
                    RowLayout {
                        id: overviewAndSpinnyRow
                        Layout.fillWidth: true
                        Layout.fillHeight: false
                        Layout.minimumHeight: root.minimized ? 20 : 63
                        Layout.preferredHeight: root.minimized ? 20 : 63
                        Layout.maximumHeight: root.minimized ? 20 : 63
                        spacing: 2

                        // Small Spinny/Cover Slot (Small mode)
                        SpinnyCoverSlot {
                            id: leftSpinnySmall
                            Layout.preferredHeight: 63
                            Layout.preferredWidth: 63
                            group: root.group
                            visible: root.showSmallSpinnyOrCover
                        }

                        // Waveform Overview Row
                        OverviewRow {
                            id: overviewRow
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            group: root.group
                        }
                    }
                }
            }

            // Lower Transport, Loop, Beatjump Placeholders
            TransportLoopBeatjumpPlaceholders {
                id: transportRow
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.minimumHeight: 52
                Layout.preferredHeight: 52
                Layout.maximumHeight: 52
                group: root.group
                showHotcues: root.showHotcues
                show8Hotcues: root.show8Hotcues
                showIntroOutroCues: root.showIntroOutroCues
                showLoopControls: root.showLoopControls
                showBeatjumpControls: root.showBeatjumpControls
                visible: !root.minimized
            }
        }

        // Right Rate controls placeholder
        RatePlaceholder {
            id: rateControls
            Layout.preferredWidth: 90
            Layout.fillHeight: false
            Layout.minimumHeight: 202
            Layout.preferredHeight: 202
            Layout.maximumHeight: 202
            Layout.alignment: Qt.AlignTop
            group: root.group
            showRateControlButtons: root.showRateControlButtons
            visible: !root.minimized && root.showRateControls
        }
    }

    Mixxx.PlayerDropArea {
        anchors.fill: parent
        group: root.group
    }
}
