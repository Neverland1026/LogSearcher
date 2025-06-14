import QtQuick 2.15
import QtQuick.Controls 2.15

// LogPanel
Rectangle {
    id: root

    width: 350; height: 350
    color: "transparent"
    //border.color: "cyan"; border.width: 1
    clip: true

    property var logModel

    property real dynamicFontSize: 16  // 全局动态字体尺寸

    property int lineNumWidth: 7

    function positionViewAtEnd() {
        listView.positionViewAtEnd()
    }

    ListView {
        id: listView

        anchors.fill: parent
        model: logModel  // 绑定C++模型
        anchors.margins: 10
        cacheBuffer: 2000

        //asynchronous: true

        boundsBehavior: Flickable.StopAtBounds
        //flickDeceleration: 0
        maximumFlickVelocity: 1200
        //highlightMoveDuration: 0

        delegate:  Item {
            width: listView.width
            height: dynamicFontSize * 1.1

            Text {
                id: lineNumText
                anchors {
                    left: parent.left
                    //right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: lineNumWidth * 12
                text: index + 1
                font.family: "Consolas"
                font.pixelSize: dynamicFontSize
                //font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "white"
            }

            TextEdit {
                anchors {
                    left: lineNumText.right
                    leftMargin: 5
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                text: display
                textFormat: TextEdit.RichText
                readOnly: true
                selectByMouse: true
                selectionColor: "lightblue"
                selectedTextColor: "navy"
                font.family: "Consolas"
                font.pixelSize: dynamicFontSize
                //font.bold: true
                //wrapMode: TextEdit.Wrap

                onSelectedTextChanged: {
                    //console.log("textEdit.selectedText = ", selectedText);
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }
        //ScrollBar.horizontal: ScrollBar {
        //    policy: ScrollBar.AsNeeded
        //}

        // 拦截滚轮事件
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            onWheel: {
                if (wheel.modifiers & Qt.ControlModifier) {
                    console.log("-----", dynamicFontSize)
                    dynamicFontSize += (wheel.angleDelta.y > 0) ? 1 : -1;
                    dynamicFontSize = Math.min(24, Math.max(8, dynamicFontSize));
                    wheel.accepted = true;
                } else {
                    wheel.accepted = false;
                }
            }
        }
    }
}
