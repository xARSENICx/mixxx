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
    readonly property bool showBigSpinnyOrCover: showBigSpinnyProxy.value > 0
    readonly property bool showSmallSpinnyOrCover: !showBigSpinnyOrCover && !root.minimized
    readonly property string keyText: currentTrack?.keyText || "--"

    color: "#161616"
    border.color: "#2e2e2e"
    border.width: 1
    radius: 4

    Mixxx.ControlProxy {
        id: showBigSpinnyProxy
        group: "[Skin]"
        key: "show_big_spinny_or_cover"
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
                        model: ["FX1", "FX2"]

                        delegate: Rectangle {
                            width: 26
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

                KeyControlsPlaceholder {
                    Layout.preferredWidth: 111
                    Layout.maximumWidth: 111
                    Layout.preferredHeight: 20
                    Layout.maximumHeight: 20
                    group: root.group
                    keyText: root.keyText
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
            visible: !root.minimized
        }
    }

    Mixxx.PlayerDropArea {
        anchors.fill: parent
        group: root.group
    }
}
