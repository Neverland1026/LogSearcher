import QtQuick 2.15
import QtQuick.Controls 2.15

// WelcomePage
Rectangle {
    id: root

    width: 350; height: 350
    color: "white"

    Text {
        anchors.centerIn: parent
        text: "Let's search it!"
        //font.family: "Consolas"
        font.bold: true
        font.pixelSize: window.width / 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#E8E8E8"
    }
}
