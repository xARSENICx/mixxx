import QtQuick
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "../LateNightTheme"
import "../../../qml/Deck" as SharedDeck

Item {
    id: root

    required property string group

    readonly property var deckPlayer: Mixxx.PlayerManager.getPlayer(root.group)
    readonly property var currentTrack: deckPlayer?.currentTrack
    readonly property bool isLoaded: deckPlayer?.isLoaded ?? false
    readonly property bool useSecondaryDeckText: root.group === "[Channel3]" || root.group === "[Channel4]"
    readonly property color loadedDeckTextColor: useSecondaryDeckText ? LateNightTheme.secondaryDeckTextColor : LateNightTheme.primaryDeckTextColor

    implicitHeight: 55

    function formatDuration(value) {
        if (!Number.isFinite(value) || value <= 0) {
            return "";
        }
        const totalSeconds = Math.floor(value);
        const hours = Math.floor(totalSeconds / 3600);
        const minutes = Math.floor((totalSeconds % 3600) / 60);
        const seconds = totalSeconds % 60;
        if (hours > 0) {
            return hours.toString() + ":" +
                    minutes.toString().padStart(2, "0") + ":" +
                    seconds.toString().padStart(2, "0");
        }
        return minutes.toString() + ":" + seconds.toString().padStart(2, "0");
    }

    Mixxx.ControlProxy {
        id: durationProxy
        group: root.group
        key: "duration"
    }

    Mixxx.ControlProxy {
        id: playpositionProxy
        group: root.group
        key: "playposition"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Row 1: Title and Elapsed/Remaining Time
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 25
            spacing: 10

            Text {
                id: titleText
                Layout.fillWidth: true
                text: root.isLoaded ? (currentTrack?.title || "Unknown Title") : "No track loaded"
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Normal
                color: root.isLoaded ? root.loadedDeckTextColor : LateNightTheme.textColorMuted
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }

            SharedDeck.TrackTime {
                id: trackTimeDisplay
                Layout.preferredWidth: 180
                group: root.group
                display: Mixxx.Config.controlPositionDisplay
                mode: Mixxx.Config.controlTimeFormat
                elapsed: durationProxy.value * playpositionProxy.value
                remaining: durationProxy.value * (1 - playpositionProxy.value)
                font.family: "Open Sans"
                font.pixelSize: 16
                font.weight: Font.Normal
                color: root.isLoaded ? LateNightTheme.deckTimeTextColor : LateNightTheme.textColorMuted
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                visible: root.isLoaded
            }
        }

        // Row 2: 2px Track Color Strip
        Rectangle {
            id: trackColorStrip
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            color: {
                if (!root.isLoaded || !currentTrack) {
                    return "transparent";
                }
                const trackColor = currentTrack.color;
                if (!trackColor.valid) {
                    return "transparent";
                }
                return trackColor;
            }
            visible: root.isLoaded
        }

        // Spacer when color strip is invisible to keep height stable
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 2
            visible: !trackColorStrip.visible
        }

        // Row 3: Artist and Duration
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            spacing: 10

            Text {
                id: artistText
                Layout.fillWidth: true
                text: root.isLoaded ? (currentTrack?.artist || "Unknown Artist") : ""
                font.family: "Open Sans"
                font.pixelSize: 18
                font.weight: Font.Normal
                color: root.isLoaded ? root.loadedDeckTextColor : LateNightTheme.textColorMuted
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: durationText
                Layout.preferredWidth: 180
                text: root.isLoaded ? root.formatDuration(durationProxy.value) : ""
                font.family: "Open Sans"
                font.pixelSize: 14
                font.weight: Font.Normal
                color: root.isLoaded ? LateNightTheme.deckTimeTextColor : LateNightTheme.textColorMuted
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
