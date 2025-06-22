import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects

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
        source: "qrc:/image/log.svg"

        MouseArea {
            anchors.fill: parent
            onDoubleClicked: fileDialog.open()
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: (parent.height - image.height) / 2 / 2
        text: ""//"Let's search it!"
        //font.family: "Consolas"
        font.bold: true
        font.pixelSize: root.width / 20
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: "#D81E06"
    }

    Item {
        id: item

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: (parent.height - image.height) / 2 / 2

        property string fullText: "Let's search it!"
        property string displayText: ""
        property bool showCursor: true

        Row {
            anchors.centerIn: parent
            spacing: 2

            Text {
                text: item.displayText
                //font.family: "Consolas"
                color: "#D81E06"
                font.bold: true
                font.pixelSize: root.width / 20
            }

            Rectangle {
                width: 2
                height: 30
                color: "black"
                visible: item.showCursor && item.displayText.length < item.fullText.length
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Timer {
            id: typeTimer
            interval: 10
            repeat: true
            running: true
            onTriggered: {
                if (item.displayText.length < item.fullText.length) {
                    item.displayText = item.fullText.substring(0, item.displayText.length + 1);
                } else {
                    stop();
                }
            }
        }

        Timer {
            id: cursorTimer
            interval: 50
            repeat: true
            running: true
            onTriggered: item.showCursor = !item.showCursor
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Select Log File")
        fileMode: FileDialog.OpenFile
        nameFilters: [ "Log File (*.LOG *.log *.TXT *.txt)" ]

        onAccepted: {
            $LogSearcher.openLog(fileDialog.selectedFile.toString());
        }
    }

}
