import QtQuick 2.15
import QtQuick.Controls 2.15

// FindResultWindow
Window {
    id: root
    width: Screen.width / 6 * 5
    height: Screen.height / 5 * 4
    title: "Find Result"

    modality: Qt.WindowModal

    function setLineNumWidth(lineNumWidth) {
        logPanel.lineNumWidth = lineNumWidth;
    }

    // 匹配到的项数
    property int findCount: -1

    // 匹配耗时
    property int findTimeCost: -1

    signal sigPositionViewAtIndex(var lineNumber);

    signal sigClose();

    // 背景
    Rectangle { anchors.fill: parent; color: "white"; }

    // 搜索结果
    LogPanel {
        id: logPanel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: separateRect.top
        logModel: $FindModel
        onSigDoubleClicked: sigPositionViewAtIndex(lineNumber)
    }

    // 分隔符
    Rectangle {
        id: separateRect
        anchors.left: parent.left
        anchors.bottom: text.top
        width: parent.width
        height: 1
        color: "black"
    }

    // 查找到的数量及耗时
    Text {
        id: text
        width: parent.width
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "black"
        text: ("<font color='#000000'>匹配到 <b>%1</b> 项，耗时 <b>%2ms</b></font>").arg(findCount).arg(findTimeCost)
        font.family: "Consolas"
        font.pixelSize: 18
        textFormat: TextEdit.RichText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    onClosing: { sigClose(); }
}
