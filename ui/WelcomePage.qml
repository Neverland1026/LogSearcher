import QtQuick 2.15
import QtQuick.Controls 2.15

// WelcomePage
Rectangle {
    id: root

    width: 350; height: 350
    color: "white"

    Image {
        id: image
        width: root.width / 5
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height / 5
        sourceSize.width: width * 2
        sourceSize.height: height * 2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/image/logo.svg"
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: (parent.height - image.height) / 2 / 2
        text: "Let's search it!"
        //font.family: "Consolas"
        font.bold: true
        font.pixelSize: root.width / 15
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#D81E06"
    }
}
