import QtQuick
import Mixxx 1.0 as Mixxx
import Mixxx.Controls 1.0 as MixxxControls
import "../LateNightTheme"

Item {
    id: root

    readonly property var currentTrack: deckPlayer?.currentTrack
    readonly property var deckPlayer: Mixxx.PlayerManager.getPlayer(root.group)
    required property string group
    readonly property bool isLoaded: deckPlayer?.isLoaded ?? false
    readonly property bool showCover: showCoverArtProxy.value > 0
    readonly property bool showSpinny: showSpinniesProxy.value > 0 || showCoverArtProxy.value <= 0

    // Maintain a 1:1 aspect ratio (square)
    width: height

    Mixxx.ControlProxy {
        id: showSpinniesProxy

        group: "[Skin]"
        key: "show_spinnies"
    }
    Mixxx.ControlProxy {
        id: showCoverArtProxy

        group: "[Skin]"
        key: "show_coverart"
    }

    // Spinny Platter Mode
    Item {
        id: spinnyContainer

        anchors.fill: parent
        visible: root.showSpinny

        // Platter Background
        Image {
            id: spinnyBg

            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: LateNightTheme.assetDeckSpinnyBackground
        }
        Image {
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop
            source: (root.isLoaded && root.currentTrack?.coverArtUrl) ? root.currentTrack.coverArtUrl : LateNightTheme.assetDeckCoverDefault
            visible: root.showCover
        }

        // Rotating Platter Indicator (Active when track is loaded)
        MixxxControls.Spinny {
            id: spinnyIndicator

            anchors.fill: parent
            group: root.group
            indicatorVisible: root.isLoaded

            indicator: Image {
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: LateNightTheme.assetDeckSpinnyIndicator
            }
        }

        // Vinyl Grooves Overlay (Mask)
        Image {
            id: spinnyMask

            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: {
                const isDeck12 = root.group === "[Channel1]" || root.group === "[Channel2]";
                return isDeck12 ? LateNightTheme.assetDeckSpinnyMask12 : LateNightTheme.assetDeckSpinnyMask34;
            }
        }
    }

    // Cover Art Mode
    Item {
        id: coverArtContainer

        anchors.fill: parent
        visible: !root.showSpinny && root.showCover

        Image {
            id: coverArtImage

            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: (root.isLoaded && currentTrack?.coverArtUrl) ? currentTrack.coverArtUrl : LateNightTheme.assetDeckCoverDefault
        }
    }
}
