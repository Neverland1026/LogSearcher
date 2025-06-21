import QtQuick 2.15
import QtQuick.Controls 2.15

// LogContentModifiedRemind
Rectangle {
    id: root
    width: 230
    height: 30
    color: "red"
    radius: 3
    visible: false

    Text {
        anchors.centerIn: parent
        text: "本地文件有修改，点击刷新"
        color: "white"
        //font.family: "Consolas"
        font.pixelSize: 15
        font.bold: true
    }

    MouseArea {
        anchors.fill: parent
        onPressed: { parent.visible = false; $LogSearcher.openLog("", true); }
    }
}
