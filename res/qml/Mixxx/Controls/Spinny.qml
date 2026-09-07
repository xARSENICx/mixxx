import Mixxx 1.0 as Mixxx
import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root

    required property string group
    property bool indicatorVisible: true
    property alias indicator: indicatorContainer.contentItem
    property bool rotationAnimationReady: false
    property real animatedAngle: 0
    property real observedPlayPositionIntervalMs: 1000 / 15
    property real lastPlayPositionUpdateMs: 0
    readonly property real targetAngle: indicatorRotation.targetAngle
    readonly property bool rotationAnimationEnabled: rotationAnimationReady &&
        root.visible &&
        root.indicatorVisible &&
        trackLoadedControl.value > 0 &&
        !scratchArea.pressed &&
        scratchEnableControl.value <= 0

    // Avoid animation short blinking of spinny during startup
    Component.onCompleted: {
        indicatorTransition.enabled = true;
        rotationAnimationReady = true;
        updateAnimatedAngle();
    }

    function recordPlayPositionUpdate() {
        if (trackLoadedControl.value <= 0) {
            lastPlayPositionUpdateMs = 0;
            return;
        }

        var now = Date.now();
        if (lastPlayPositionUpdateMs > 0) {
            var interval = now - lastPlayPositionUpdateMs;
            if (interval >= 40 && interval <= 200) {
                observedPlayPositionIntervalMs =
                    observedPlayPositionIntervalMs * 0.75 + interval * 0.25;
            } else if (interval > 250) {
                observedPlayPositionIntervalMs = 1000 / 15;
            }
        }
        lastPlayPositionUpdateMs = now;
    }

    function resetPlayPositionTiming() {
        observedPlayPositionIntervalMs = 1000 / 15;
        lastPlayPositionUpdateMs = 0;
    }

    function stopAndSnapAnimation() {
        if (rotationAnimationReady) {
            rotationAnimation.stop();
        }
        if (isFinite(targetAngle)) {
            animatedAngle = targetAngle;
        } else {
            animatedAngle = 0;
        }
    }

    function updateAnimatedAngle() {
        if (!isFinite(targetAngle)) {
            stopAndSnapAnimation();
            return;
        }

        if (!rotationAnimationEnabled || !isFinite(animatedAngle)) {
            stopAndSnapAnimation();
            return;
        }

        if (Math.abs(animatedAngle - targetAngle) < 0.0001) {
            rotationAnimation.stop();
            animatedAngle = targetAngle;
            return;
        }

        rotationAnimation.stop();
        rotationAnimation.from = animatedAngle;
        rotationAnimation.to = targetAngle;
        rotationAnimation.start();
    }

    onTargetAngleChanged: updateAnimatedAngle()
    onIndicatorVisibleChanged: updateAnimatedAngle()
    onVisibleChanged: updateAnimatedAngle()
    onRotationAnimationReadyChanged: updateAnimatedAngle()
    onRotationAnimationEnabledChanged: updateAnimatedAngle()

    Mixxx.ControlProxy {
        id: samplesControl

        group: root.group
        key: "track_samples"
    }

    Mixxx.ControlProxy {
        id: sampleRateControl

        group: root.group
        key: "track_samplerate"
    }

    Mixxx.ControlProxy {
        id: playPositionControl

        group: root.group
        key: "playposition"

        onValueChanged: root.recordPlayPositionUpdate()
    }

    Mixxx.ControlProxy {
        id: vinylSpeedTypeControl

        group: root.group
        key: "vinylcontrol_speed_type"
    }

    Mixxx.ControlProxy {
        id: trackLoadedControl

        group: root.group
        key: "track_loaded"

        onValueChanged: {
            root.resetPlayPositionTiming();
            root.stopAndSnapAnimation();
        }
    }

    Mixxx.ControlProxy {
        id: scratchEnableControl

        group: root.group
        key: "scratch_position_enable"
        value: scratchArea.pressed
    }

    Mixxx.ControlProxy {
        id: scratchPositionControl

        group: root.group
        key: "scratch_position"
    }

    // Use the deck's configured vinyl speed, falling back to 33.33 RPM
    readonly property real rpm: vinylSpeedTypeControl.value > 0
        ? vinylSpeedTypeControl.value : 33.33
    readonly property real rps: Math.PI * rpm / 60.0
    readonly property real frameRate: sampleRateControl.value
    readonly property int playPositionUpdateIntervalMs: Math.max(
        50, Math.min(100, Math.round(root.observedPlayPositionIntervalMs)))

    Control {
        id: indicatorContainer

        anchors.fill: parent
        visible: opacity > 0

        contentItem: Rectangle {
            height: root.height / 2
            width: height / 12
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
        }

        transform: Rotation {
            id: indicatorRotation

            property real roundsPerSecond: root.rps / Math.PI
            property real totalFrames: samplesControl.value / 2
            property real positionSeconds: (!isNaN(sampleRateControl.value) && sampleRateControl.value > 0) ? playPositionControl.value * totalFrames / sampleRateControl.value : 0
            property real rotationFactor: indicatorRotation.roundsPerSecond * indicatorRotation.positionSeconds % 1

            origin.x: root.width / 2
            origin.y: root.height / 2
            readonly property real targetAngle: 360 * rotationFactor
            angle: root.animatedAngle
        }

        RotationAnimation {
            id: rotationAnimation

            duration: root.playPositionUpdateIntervalMs
            direction: RotationAnimation.Shortest
            easing.type: Easing.Linear
            target: root
            property: "animatedAngle"
        }

        states: State {
            name: "hidden"
            when: !root.indicatorVisible

            PropertyChanges {
                target: indicatorContainer
                opacity: 0
            }
        }

        transitions: Transition {
            id: indicatorTransition

            enabled: false
            to: "hidden"
            reversible: true

            PropertyAnimation {
                property: "opacity"
                duration: 150
            }
        }
    }

    MouseArea {
        id: scratchArea

        anchors.fill: parent
        enabled: trackLoadedControl.value > 0
        cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor

        property real lastAngle: 0

        function getAngle(x, y) {
            return Math.atan2(y - height / 2, x - width / 2);
        }

        onPressed: {
            root.stopAndSnapAnimation();
            scratchPositionControl.value = 0.0;
            lastAngle = getAngle(mouse.x, mouse.y);
        }

        onPressedChanged: root.updateAnimatedAngle()

        onPositionChanged: {
            if (isNaN(sampleRateControl.value) || sampleRateControl.value <= 0) {
                console.error(`Could not find a valid sample rate on group ${root.group}, got ${sampleRateControl.value}`);
                return;
            }
            var currentAngle = getAngle(mouse.x, mouse.y);
            var delta = currentAngle - lastAngle;

            // Normalize to [-π, π] to handle atan2 boundary crossing
            while (delta > Math.PI) delta -= 2 * Math.PI;
            while (delta < -Math.PI) delta += 2 * Math.PI;

            lastAngle = currentAngle;

            // Convert angular delta (radians) to samples:
            // samples = radians * (frameRate * 2) / (2π * rps)
            //         = radians * frameRate / (π * rps)
            scratchPositionControl.value += delta * root.frameRate / root.rps;
        }
    }
}
