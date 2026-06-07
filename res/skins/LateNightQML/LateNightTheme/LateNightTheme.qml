pragma Singleton
import QtQuick
import "."

QtObject {
    readonly property color backgroundColor: "#1e1e1e"
    readonly property color buttonActiveColor: white
    readonly property color buttonNormalColor: "#696969"
    readonly property color buttonPressedColor: white
    readonly property color darkGray: "#0f0f0f"
    readonly property color accentColor: ColorScheme.accentColor
    readonly property color deckTimeTextColor: ColorScheme.deckTimeTextColor
    readonly property color libraryPanelSplitterBackground: "#1e1e1e"
    readonly property color libraryPanelSplitterHandle: "#5f5f5f"
    readonly property color libraryPanelSplitterHandleActive: "#7a7a7a"
    readonly property color textColor: white
    readonly property color textColorMuted: "#696969"
    readonly property color primaryDeckTextColor: ColorScheme.primaryDeckTextColor
    readonly property color primaryOverviewBackgroundColor: ColorScheme.primaryOverviewBackgroundColor
    readonly property color secondaryDeckTextColor: ColorScheme.secondaryDeckTextColor
    readonly property color secondaryOverviewBackgroundColor: ColorScheme.secondaryOverviewBackgroundColor
    readonly property color toolbarActiveColor: white
    readonly property color toolbarBackgroundColor: "#242424"
    readonly property int toolbarButtonHeight: 26
    readonly property int toolbarButtonWidth: 52
    readonly property color white: "#D9D9D9"

    readonly property url assetDeckArrowLeftUpButton: legacyAsset("buttons", "btn__arrow_left_up.svg")
    readonly property url assetDeckArrowRightDownButton: legacyAsset("buttons", "btn__arrow_right_down.svg")
    readonly property url assetDeckBeatjumpLeftButton: legacyAsset("buttons", "btn__beatjump_left.svg")
    readonly property url assetDeckBeatjumpRightButton: legacyAsset("buttons", "btn__beatjump_right.svg")
    readonly property url assetDeckCoverDefault: legacyAsset("style", "cover_default.svg")
    readonly property url assetDeckCueButton: legacyAsset("buttons", "btn__cue_deck.svg")
    readonly property url assetDeckEjectButton: legacyAsset("buttons", "btn__eject.svg")
    readonly property url assetDeckIntroEndButton: legacyAsset("buttons", "btn__intro_end.svg")
    readonly property url assetDeckIntroStartButton: legacyAsset("buttons", "btn__intro_start.svg")
    readonly property url assetDeckKeyDownButton: legacyAsset("buttons", "btn__key_down.svg")
    readonly property url assetDeckKeylockButton: legacyAsset("buttons", "btn__keylock.svg")
    readonly property url assetDeckKeyMatchButton: legacyAsset("buttons", "btn__key_match.svg")
    readonly property url assetDeckKeyUpButton: legacyAsset("buttons", "btn__key_up.svg")
    readonly property url assetDeckLeaderButton: legacyAsset("buttons", "btn__sync_leader.svg")
    readonly property url assetDeckLoopButton: legacyAsset("buttons", "btn__loop.svg")
    readonly property url assetDeckLoopAnchorEndButton: legacyAsset("buttons", "btn__loop_anchor_end.svg")
    readonly property url assetDeckLoopAnchorStartButton: legacyAsset("buttons", "btn__loop_anchor_start.svg")
    readonly property url assetDeckLoopInButton: legacyAsset("buttons", "btn__loop_in.svg")
    readonly property url assetDeckLoopOutButton: legacyAsset("buttons", "btn__loop_out.svg")
    readonly property url assetDeckMinusButton: legacyAsset("buttons", "btn__minus.svg")
    readonly property url assetDeckOutroEndButton: legacyAsset("buttons", "btn__outro_end.svg")
    readonly property url assetDeckOutroStartButton: legacyAsset("buttons", "btn__outro_start.svg")
    readonly property url assetDeckPlayButton: legacyAsset("buttons", "btn__play_deck.svg")
    readonly property url assetDeckPlusButton: legacyAsset("buttons", "btn__plus.svg")
    readonly property url assetDeckQuantizeButton: legacyAsset("buttons", "btn__quantize.svg")
    readonly property url assetDeckRateSliderBackground: legacyAsset("sliders", "slider_pitch_deck.svg")
    readonly property url assetDeckRateSliderHandle: legacyAsset("sliders", "knob_pitch_deck.svg")
    readonly property url assetDeckReloopButton: legacyAsset("buttons", "btn__reloop.svg")
    readonly property url assetDeckRepeatButton: legacyAsset("buttons", "btn__repeat.svg")
    readonly property url assetDeckReverseButton: legacyAsset("buttons", "btn__reverse.svg")
    readonly property url assetDeckSettingsOffButton: legacyAsset("buttons", "btn__settings_off.svg")
    readonly property url assetDeckSettingsOnButton: legacyAsset("buttons", "btn__settings_on.svg")
    readonly property url assetDeckSlipButton: legacyAsset("buttons", "btn__slip.svg")
    readonly property url assetDeckSpinnyBackground: legacyAsset("style", "spinny_bg.svg")
    readonly property url assetDeckSpinnyGhostIndicator: legacyAsset("style", "spinny_indicator_ghost.svg")
    readonly property url assetDeckSpinnyIndicator: legacyAsset("style", "spinny_indicator.svg")
    readonly property url assetDeckSpinnyMask12: legacyAsset("style", "spinny_mask_12.svg")
    readonly property url assetDeckSpinnyMask34: legacyAsset("style", "spinny_mask_34.svg")
    readonly property url assetDeckSyncButton: legacyAsset("buttons", "btn__sync_deck.svg")
    readonly property url assetDeckVolumeSliderBackground: legacyAsset("sliders", "slider_volume_deck.svg")
    readonly property url assetDeckVolumeSliderHandle: legacyAsset("sliders", "knob_volume_deck.svg")
    readonly property url assetFxKnobBackground: legacyAsset("knobs", "knob_bg_fx.svg")
    readonly property url assetMainKnobBackground: legacyAsset("knobs", "knob_bg_main.svg")
    readonly property url assetRegularKnobBackground: legacyAsset("knobs", "knob_bg_regular.svg")
    readonly property url assetSmallKnobBackground: legacyAsset("knobs", "knob_bg_small.svg")

    function legacyButton(fileName) {
        return legacyAsset("buttons", fileName);
    }

    function legacyRegionButton(regionButtonType, buttonSize) {
        return legacyButton("btn_" + regionButtonType + "_" + buttonSize + ".svg");
    }

    function legacySubRegionButton(buttonSize) {
        return legacyRegionButton(ColorScheme.subRegionButtonType, buttonSize);
    }

    function legacyTopRegionButton(buttonSize) {
        return legacyRegionButton(ColorScheme.topRegionButtonType, buttonSize);
    }

    function legacyAsset(directory, fileName) {
        return Qt.resolvedUrl("../../LateNight/" + ColorScheme.name + "/" + directory + "/" + fileName);
    }

    function sharedImage(fileName) {
        return Qt.resolvedUrl("../../../qml/images/" + fileName);
    }
}
