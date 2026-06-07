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
            topRegionButtonType: "embedded",
            subRegionButtonType: "elevated"
        }
    })

    readonly property var activeScheme: schemes[configScheme] || schemes["PaleMoon"]

    readonly property color accentColor: activeScheme.accentColor
    readonly property color deckTimeTextColor: activeScheme.deckTimeTextColor
    readonly property string displayName: activeScheme.displayName
    readonly property string name: activeScheme.name
    readonly property color primaryDeckTextColor: activeScheme.primaryDeckTextColor
    readonly property color secondaryDeckTextColor: activeScheme.secondaryDeckTextColor
    readonly property string subRegionButtonType: activeScheme.subRegionButtonType
    readonly property string topRegionButtonType: activeScheme.topRegionButtonType
}
