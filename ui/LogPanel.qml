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

    property real dynamicFontSize: 18  // 全局动态字体尺寸

    function countDigits(number) {
        var count = 0;
        var temp = number;
        while (temp > 0) {
            count++;
            temp = Math.floor(temp / 10);
        }
        return count;
    }

    function positionViewAtEnd() {
        listView.positionViewAtEnd()
    }

    ListView {
        id: listView

        anchors.fill: parent
        model: logModel  // 绑定C++模型
        anchors.margins: 10
        cacheBuffer: 1000

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
                width: countDigits(listView.count) * 12
                text: index + 1
                font.family: "Consolas"
                font.pixelSize: dynamicFontSize
                font.bold: true
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
                font.bold: true

                onSelectedTextChanged: {
                    console.log("textEdit.selectedText = ", selectedText);
                }
            }
        }

        maximumFlickVelocity: 2000  // 限制最大滚动速度

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            //contentItem: Rectangle { color: "#4CAF50"; radius: 3 }
        }

        //        ScrollBar.vertical: CustomScrollBar {
        //            parent: listView
        //            //opacity: listView.moving ? 1 : 0.5
        //            //Behavior on opacity { NumberAnimation { duration: 300 } }
        //            anchors {
        //                top: listView.top
        //                right: listView.right
        //                bottom: listView.bottom
        //            }

        //            Rectangle {
        //                width: parent.width
        //                height: Math.max(20, listView.visibleArea.heightRatio * parent.height)
        //                y: listView.visibleArea.yPosition * parent.height
        //                color: "#FF0000"
        //            }

        //            // 隐藏上下箭头交互区域
        //            Component.onCompleted: {
        //                __scroller.verticalScrollBar.subControlRect = function() { return Qt.rect(0, 0, 0, 0); }
        //            }

        //        }

        //        ScrollBar.vertical: ScrollBar {
        //                id: vbar
        //                policy: ScrollBar.AlwaysOn  // 滚动条始终显示
        //                width: 10  // 自定义宽度
        //            }

        //        ScrollBar.vertical: ScrollBar {
        //            policy: ScrollBar.AsNeeded
        //            contentItem: Rectangle {
        //                color: "gray"
        //                radius: 3
        //            }
        //            background: Rectangle {
        //                color: "lightgray"
        //            }
        //        }

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
