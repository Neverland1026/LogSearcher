﻿import QtQuick 2.15
import QtQuick.Controls 2.15

// FindWindow
Window {
    id: root
    width: Screen.width / 8 * 3
    height: Screen.height / 7
    title: "Find"

    modality: Qt.WindowModal

    function setFindText(text) {
        textEdit.text = text;
        textEdit.cursorPosition = textEdit.text.length;
    }

    // 背景
    Rectangle { anchors.fill: parent; color: "white"; }

    TextEdit {
        id: textEdit
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: button.top
        }

        color: "black"
        text: ""
        font.family: "Consolas"
        font.pixelSize: 25
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        onAccepted: { $LogSearcher.find(textEdit.text); close(); }
    }

    Button {
        id: button
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        height: 40
        text: "Find"
        font.family: "Consolas"
        font.pixelSize: 20
        onClicked: { $LogSearcher.find(textEdit.text); close(); }
    }

    Component.onCompleted: textEdit.forceActiveFocus()
}
