pragma Singleton
import QtQuick
import Mixxx 1.0 as Mixxx

QtObject {
    id: root

    readonly property string configScheme: Mixxx.Config.configScheme
    readonly property var schemes: ({
        "PaleMoon": {
            name: "palemoon",
            displayName: "PaleMoon",
            accentColor: "#d9b28c",
            primaryDeckTextColor: "#c2b3a5",
            secondaryDeckTextColor: "#85bdbb",
            deckTimeTextColor: "#777777",
            primaryOverviewBackgroundColor: "#19191a",
            secondaryOverviewBackgroundColor: "#001b23",
            primaryWaveformSignalColor: "#d9b28c",
            secondaryWaveformSignalColor: "#7bc6c3",
            topRegionButtonType: "embedded",
            subRegionButtonType: "embedded"
        },
        "Classic": {
            name: "classic",
            displayName: "Classic",
            accentColor: "#e7c413",
            primaryDeckTextColor: "#f0bb2b",
            secondaryDeckTextColor: "#0bd9d1",
            deckTimeTextColor: "#f0bb2b",
            primaryOverviewBackgroundColor: "#0f0f0f",
            secondaryOverviewBackgroundColor: "#001b23",
            primaryWaveformSignalColor: "#e7c413",
            secondaryWaveformSignalColor: "#09b2ae",
            topRegionButtonType: "embedded",
            subRegionButtonType: "elevated"
        }
    })

    readonly property var activeScheme: schemes[configScheme] || schemes["PaleMoon"]

    readonly property color accentColor: activeScheme.accentColor
    readonly property color deckTimeTextColor: activeScheme.deckTimeTextColor
    readonly property string displayName: activeScheme.displayName
    readonly property string name: activeScheme.name
    readonly property color primaryOverviewBackgroundColor: activeScheme.primaryOverviewBackgroundColor
    readonly property color primaryWaveformSignalColor: activeScheme.primaryWaveformSignalColor
    readonly property color primaryDeckTextColor: activeScheme.primaryDeckTextColor
    readonly property color secondaryOverviewBackgroundColor: activeScheme.secondaryOverviewBackgroundColor
    readonly property color secondaryWaveformSignalColor: activeScheme.secondaryWaveformSignalColor
    readonly property color secondaryDeckTextColor: activeScheme.secondaryDeckTextColor
    readonly property string subRegionButtonType: activeScheme.subRegionButtonType
    readonly property string topRegionButtonType: activeScheme.topRegionButtonType
}
