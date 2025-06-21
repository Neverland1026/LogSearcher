import QtQuick 2.15
import QtQuick.Controls 2.15

// SpecifiedLineNumberWindow
Window {
    id: root
    width: Screen.width / 8 * 3
    height: Screen.height / 7
    title: "Jump to the specified line"

    modality: Qt.WindowModal

    signal sigJumpToTheSpecifiedLine(var lineNumber);

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

        validator: IntValidator { bottom: 0; top: 99999999; }

        color: "black"
        text: ""
        font.family: "Consolas"
        font.pixelSize: 50
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        onAccepted: { root.sigJumpToTheSpecifiedLine(textField.text); close(); }
    }

    Button {
        id: button
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        height: 40
        text: "Jump"
        font.family: "Consolas"
        font.pixelSize: 20
        onClicked: { root.sigJumpToTheSpecifiedLine(textField.text); close(); }
    }

    Component.onCompleted: textField.forceActiveFocus()
}
