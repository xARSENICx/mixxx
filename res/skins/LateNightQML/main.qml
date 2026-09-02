pragma ComponentBehavior: Bound

import "../../qml" as Skin
import "LateNightTheme"
import "Deck" as LateNightDeck
import "Mixer" as LateNightMixer
import "Toolbar" as LateNightToolbar
import "Waveforms" as LateNightWaveforms
import Mixxx 1.0 as Mixxx
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root

    readonly property int activeDeckState: maximizeLibrary ? LateNightDeck.Deck.Mini : normalDeckState
    readonly property bool maximizeLibrary: toolbar.maximizeLibrary
    readonly property int normalDeckState: showMixer ? LateNightDeck.Deck.Full : Math.max(LateNightDeck.Deck.Mini, Math.min(LateNightDeck.Deck.Full, toolbar.deckSizeWithoutMixer))
    readonly property int numDecks: 4
    readonly property int numSamplers: 64
    readonly property bool show4decks: toolbar.show4decks
    readonly property bool showCompactVuMeters: !maximizeLibrary && !showMixer && normalDeckState === LateNightDeck.Deck.Compact && showCompactVuMetersProxy.value > 0
    readonly property bool showDeckArea: !maximizeLibrary || showMaximizedDecks
    readonly property bool showMaximizedDecks: toolbar.showMaximizedDecks
    readonly property bool showMixer: toolbar.showMixer
    readonly property bool showWaveforms: toolbar.showWaveforms

    color: LateNightTheme.backgroundColor
    height: 1008
    minimumHeight: 720
    minimumWidth: 1280
    visible: true
    width: 1792

    Mixxx.ControlProxy {
        group: "[App]"
        key: "num_decks"

        onInitializedChanged: value = root.numDecks
    }
    Mixxx.ControlProxy {
        group: "[App]"
        key: "num_samplers"

        onInitializedChanged: value = root.numSamplers
    }
    Instantiator {
        model: [
            {
                key: "show_waveforms",
                defaultValue: 1.0
            },
            {
                key: "show_4decks",
                defaultValue: 0.0
            },
            {
                key: "show_hotcues",
                defaultValue: 1.0
            },
            {
                key: "show_8_hotcues",
                defaultValue: 1.0
            },
            {
                key: "show_intro_outro_cues",
                defaultValue: 1.0
            },
            {
                key: "show_loop_controls",
                defaultValue: 1.0
            },
            {
                key: "show_beatjump_controls",
                defaultValue: 1.0
            },
            {
                key: "show_rate_controls",
                defaultValue: 1.0
            },
            {
                key: "show_rate_control_buttons",
                defaultValue: 1.0
            },
            {
                key: "show_key_controls",
                defaultValue: 1.0
            },
            {
                key: "show_eq_knobs",
                defaultValue: 1.0
            },
            {
                key: "show_eq_kill_buttons",
                defaultValue: 1.0
            },
            {
                key: "show_xfader",
                defaultValue: 1.0
            },
            {
                key: "show_main_head_mixer",
                defaultValue: 1.0
            },
            {
                key: "equal_4deck_waveforms",
                defaultValue: 0.0
            },
            {
                key: "timing_shift_buttons",
                defaultValue: 0.0
            },
            {
                key: "show_superknobs",
                defaultValue: 0.0
            },
            {
                key: "show_sampler_fx",
                defaultValue: 0.0
            },
            {
                key: "show_rate_controls_compact",
                defaultValue: 1.0
            },
            {
                key: "show_loop_controls_compact",
                defaultValue: 1.0
            },
            {
                key: "show_beatjump_controls_compact",
                defaultValue: 1.0
            },
            {
                key: "show_key_controls_compact",
                defaultValue: 1.0
            },
            {
                key: "show_vumeters_compact",
                defaultValue: 1.0
            },
            {
                key: "latenight_show_sync_button_compact",
                defaultValue: 1.0
            },
            {
                key: "latenight_deck_size_without_mixer",
                defaultValue: 1.0
            },
            {
                key: "latenight_max_lib_show_decks",
                defaultValue: 1.0
            },
            {
                key: "latenight_sampler_rows",
                defaultValue: 1.0
            },
            {
                key: "latenight_expand_samplers_1_4",
                defaultValue: 0.0
            },
            {
                key: "latenight_expand_samplers_1_8",
                defaultValue: 0.0
            },
            {
                key: "latenight_expand_samplers_9_16",
                defaultValue: 0.0
            }
        ]

        delegate: Mixxx.SkinControlCreator {
            required property var modelData

            defaultValue: modelData.defaultValue
            group: "[Skin]"
            key: modelData.key
            persist: true
        }
    }
    Mixxx.ControlProxy {
        id: showCompactVuMetersProxy

        group: "[Skin]"
        key: "show_vumeters_compact"
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        LateNightToolbar.Toolbar {
            id: toolbar

            Layout.fillWidth: true
            Layout.preferredHeight: 26
            show4decksAvailable: root.height >= root.minimumHeight
        }
        SplitView {
            id: splitView

            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Vertical

            handle: Rectangle {
                id: handleDelegate

                readonly property color handleColor: SplitHandle.pressed || SplitHandle.hovered ? LateNightTheme.libraryPanelSplitterHandleActive : LateNightTheme.libraryPanelSplitterHandle
                readonly property int handleSize: SplitHandle.pressed || SplitHandle.hovered ? 6 : 3

                color: LateNightTheme.libraryPanelSplitterBackground
                implicitHeight: waveformLoader.active ? 4 : 0
                implicitWidth: 8
                visible: waveformLoader.active

                RowLayout {
                    anchors.centerIn: parent

                    Repeater {
                        model: 3

                        Rectangle {
                            required property int index

                            color: handleDelegate.handleColor
                            height: handleDelegate.handleSize
                            radius: handleDelegate.handleSize
                            width: handleDelegate.handleSize
                        }
                    }
                }
            }

            Loader {
                id: waveformLoader

                SplitView.maximumHeight: active ? Infinity : 0
                SplitView.minimumHeight: active ? 80 : 0
                SplitView.preferredHeight: active ? 180 : 0
                active: root.showWaveforms && !root.maximizeLibrary
                visible: active

                sourceComponent: Component {
                    LateNightWaveforms.WaveformStack {
                        show4decks: root.show4decks
                    }
                }
            }
            Item {
                id: lowerSection

                SplitView.fillHeight: true
                SplitView.minimumHeight: lowerSectionLayout.implicitHeight

                ColumnLayout {
                    id: lowerSectionLayout

                    anchors.fill: parent
                    spacing: 0

                    GridLayout {
                        id: deckGrid

                        Layout.fillWidth: true
                        Layout.preferredHeight: root.showDeckArea ? implicitHeight : 0
                        columnSpacing: 0
                        columns: 3
                        rowSpacing: 0
                        visible: root.showDeckArea

                        Loader {
                            id: deck1Loader

                            Layout.column: 0
                            Layout.fillWidth: true
                            Layout.preferredHeight: active ? implicitHeight : 0
                            Layout.row: 0
                            active: root.showDeckArea

                            sourceComponent: Component {
                                LateNightDeck.Deck {
                                    deckState: root.activeDeckState
                                    editMode: toolbar.editDeck
                                    group: "[Channel1]"
                                }
                            }
                        }
                        Item {
                            id: centerColumn

                            Layout.column: 1
                            Layout.preferredHeight: Math.max(mixerLoader.implicitHeight, compactVuLoader.implicitHeight)
                            Layout.preferredWidth: Math.max(mixerLoader.implicitWidth, compactVuLoader.implicitWidth)
                            Layout.row: 0
                            Layout.rowSpan: root.show4decks ? 2 : 1

                            Loader {
                                id: mixerLoader

                                active: root.showMixer && !root.maximizeLibrary
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.top: parent.top

                                sourceComponent: Component {
                                    LateNightMixer.Mixer {
                                        groups: ["[Channel1]", "[Channel2]", "[Channel3]", "[Channel4]"]
                                        show4decks: root.show4decks
                                    }
                                }
                            }
                            Loader {
                                id: compactVuLoader

                                active: root.showCompactVuMeters
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.top: parent.top

                                sourceComponent: Component {
                                    LateNightMixer.CompactCenterVuMeters {
                                        show4decks: root.show4decks
                                    }
                                }
                            }
                        }
                        Loader {
                            id: deck2Loader

                            Layout.column: 2
                            Layout.fillWidth: true
                            Layout.preferredHeight: active ? implicitHeight : 0
                            Layout.row: 0
                            active: root.showDeckArea

                            sourceComponent: Component {
                                LateNightDeck.Deck {
                                    deckState: root.activeDeckState
                                    editMode: toolbar.editDeck
                                    group: "[Channel2]"
                                }
                            }
                        }
                        Loader {
                            id: deck3Loader

                            Layout.column: 0
                            Layout.fillWidth: true
                            Layout.preferredHeight: active ? implicitHeight : 0
                            Layout.row: 1
                            active: root.showDeckArea && root.show4decks
                            visible: active

                            sourceComponent: Component {
                                LateNightDeck.Deck {
                                    deckState: root.activeDeckState
                                    editMode: toolbar.editDeck
                                    group: "[Channel3]"
                                }
                            }
                        }
                        Loader {
                            id: deck4Loader

                            Layout.column: 2
                            Layout.fillWidth: true
                            Layout.preferredHeight: active ? implicitHeight : 0
                            Layout.row: 1
                            active: root.showDeckArea && root.show4decks
                            visible: active

                            sourceComponent: Component {
                                LateNightDeck.Deck {
                                    deckState: root.activeDeckState
                                    editMode: toolbar.editDeck
                                    group: "[Channel4]"
                                }
                            }
                        }
                    }
                    Loader {
                        id: effectsRackLoader

                        Layout.fillWidth: true
                        Layout.preferredHeight: active ? implicitHeight : 0
                        active: toolbar.showEffects && !root.maximizeLibrary
                        visible: active

                        sourceComponent: Component {
                            Skin.EffectRow {
                                width: effectsRackLoader.width
                            }
                        }
                    }
                    Loader {
                        id: samplersRackLoader

                        Layout.fillWidth: true
                        Layout.preferredHeight: active ? implicitHeight : 0
                        active: toolbar.showSamplers && !root.maximizeLibrary
                        visible: active

                        sourceComponent: Component {
                            Skin.SamplerRow {
                                width: samplersRackLoader.width
                            }
                        }
                    }
                    Loader {
                        id: micAuxRackLoader

                        Layout.fillWidth: true
                        Layout.preferredHeight: active ? 58 : 0
                        active: toolbar.showMicAux && !root.maximizeLibrary
                        visible: active

                        sourceComponent: Component {
                            RowLayout {
                                spacing: 8

                                Skin.MicrophoneUnit {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    unitNumber: 1
                                }
                                Skin.MicrophoneUnit {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    unitNumber: 2
                                }
                                Skin.AuxiliaryUnit {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    unitNumber: 1
                                }
                                Skin.AuxiliaryUnit {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    unitNumber: 2
                                }
                            }
                        }
                    }
                    Loader {
                        id: libraryLoader

                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.minimumHeight: 180
                        active: true

                        sourceComponent: Component {
                            Library {
                            }
                        }
                    }
                }
            }
        }
    }
}
