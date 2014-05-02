/*
    Copyright (C) 2012-2013 Collabora Ltd. <info@collabora.com>
      @author George Kiagiadakis <george.kiagiadakis@collabora.com>
    Copyright (C) 2013 basysKom GmbH <info@basyskom.com>
      @author Benjamin Federau <benjamin.federau@basyskom.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
import QtQuick 2.0
import QtGStreamer 1.0

Rectangle {
    id: window
    width: 600
    height: 300

    Column {

        VideoItem {
            id: video

            width: window.width
            height: 260
            surface: videoSurface1 //bound on the context from main()
        }

        Row {
            id: buttons

            width: window.width
            height: 35
            spacing: 5

            Rectangle {
                id: playButton
                color: "black"

                width: 60
                height: 30

                Text { text: "Play"; color: "white"; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: player.play() }
            }

            Rectangle {
                id: pauseButton
                color: "black"

                width: 60
                height: 30

                Text { text: "Pause"; color: "white"; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: { player.pause(); } }
            }

            Rectangle {
                id: stopButton
                color: "black"

                width: 60
                height: 30

                Text { text: "Stop"; color: "white"; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent; onClicked: player.stop() }
            }
        }
    }
}
