import QtQuick 2.1
import QtQuick.Window 2.1
import QtGStreamer 1.0

Window {
    width: 200
    height: 200
    visible: true

    ListView {
        anchors.fill: parent
        model: 3

        delegate: VideoItem {
            width: 100
            height: 100
            surface: surface1
        }
    }
}
