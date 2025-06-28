import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs

// WelcomePage
Rectangle {
    id: root

    width: 350; height: 350
    color: "white"

    Column {
        width: parent.width
        anchors.centerIn: parent

        spacing: window.height / 7

        Image {
            id: image
            width: root.width / 4
            height: width
            anchors.horizontalCenter: parent.horizontalCenter
            sourceSize.width: width * 2
            sourceSize.height: height * 2
            fillMode: Image.PreserveAspectFit
            source: "qrc:/image/logo.svg"
            opacity: 1.0//0.0

            MouseArea {
                anchors.fill: parent
                onDoubleClicked: fileDialog.open()
            }

            Behavior on opacity {
                NumberAnimation {
                    duration: 700  // 动画持续1秒
                    easing.type: Easing.InOutQuad  // 缓动效果
                }
            }
        }

        Item {
            width: parent.width
            height: 30
            anchors.horizontalCenter: parent.horizontalCenter

            Text {
                id: animatedText
                y: 0//-500
                text: "Let's search it!"
                //font.family: "Consolas"
                font.bold: true
                font.pixelSize: root.width / 20
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#D81E06"
                anchors.horizontalCenter: parent.horizontalCenter

                SequentialAnimation {
                    id: textAnimation
                    running: false

                    PauseAnimation { duration: 100 }

                    NumberAnimation {
                        target: animatedText
                        property: "y"
                        to: 0
                        duration: 600
                        easing.type: Easing.OutElastic
                    }

                    onStopped: image.opacity = 1.0
                }

                //Component.onCompleted: textAnimation.start()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr("Select Log File")
        fileMode: FileDialog.OpenFile
        nameFilters: [ "Log File (*.LOG *.log *.TXT *.txt *.ZST *.zst)" ]

        onAccepted: {
            $LogSearcher.openLog(fileDialog.selectedFile.toString());
        }
    }

}
