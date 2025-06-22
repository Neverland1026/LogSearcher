import QtQuick 2.15
import QtQuick.Controls 2.15

// FindWindow
Window {
    id: root
    width: Screen.width / 8 * 3
    height: Screen.height / 7
    title: "Find"

    modality: Qt.WindowModal

    function setFindText(text) {
        textField.text = text;
        textField.cursorPosition = textField.text.length;
    }

    // 背景
    Rectangle { anchors.fill: parent; color: "white"; }

    TextField {
        id: textField
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: button.top
        }

        background: Rectangle { color: "transparent"; border.width: 0; }

        color: "black"
        text: ""
        font.family: "Consolas"
        font.pixelSize: 30
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        onAccepted: { $LogSearcher.find(textField.text); close(); }
    }

    Button {
        id: button
        height: 40
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: 6
        }
        text: "Find"
        font.family: "Consolas"
        font.pixelSize: 20
        onClicked: { $LogSearcher.find(textField.text); close(); }
    }

    Component.onCompleted: textField.forceActiveFocus()
}
