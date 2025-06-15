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

    property int lineNumWidth: 7

    property string selectedText: ""

    function positionViewAtIndex(lineNumber) {
        listView.positionViewAtIndex(lineNumber, ListView.Center);
        listView.itemAtIndex(lineNumber).forceActiveFocus();
    }

    signal sigDoubleClicked(var lineNumber);

    ListView {
        id: listView

        anchors.fill: parent
        model: logModel  // 绑定C++模型
        anchors.margins: 10
        cacheBuffer: 2000

        boundsBehavior: Flickable.StopAtBounds
        maximumFlickVelocity: 1200
        //flickDeceleration: 5000

        clip: true

        delegate:  Item {
            width: listView.width
            height: dynamicFontSize * 1.1

            Rectangle { id: backgroundRect; anchors.fill: parent }

            Text {
                id: lineNumText
                anchors {
                    left: parent.left
                    //right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                width: lineNumWidth * 12
                text: lineNumber + 1
                font.family: "Consolas"
                font.bold: parent.activeFocus
                font.pixelSize: dynamicFontSize
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "black"
            }

            TextEdit {
                id: textEdit
                anchors {
                    left: lineNumText.right
                    leftMargin: 5
                    right: parent.right
                    top: parent.top
                    bottom: parent.bottom
                }
                text: lineContent
                textFormat: TextEdit.RichText
                verticalAlignment: Text.AlignVCenter
                readOnly: true
                selectByMouse: true
                selectionColor: "lightblue"
                selectedTextColor: "navy"
                font.family: "Consolas"
                font.pixelSize: dynamicFontSize

                onSelectedTextChanged: {
                    root.selectedText = selectedText;
                    rightMenu.existToBeFindKeyword = (selectedText !== "");
                }

                TapHandler {
                    onDoubleTapped: {
                        sigDoubleClicked(lineNumber);
                    }
                }

                onActiveFocusChanged: {
                    backgroundRect.color = activeFocus ? "#1FFF0000" : "transparent";
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            background: Rectangle {
                color: "transparent"
            }
        }
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AsNeeded
            background: Rectangle {
                color: "transparent"
            }
        }

        // 拦截滚轮事件
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton | Qt.RightButton
            onWheel: (wheel) => {
                         if (wheel.modifiers & Qt.ControlModifier) {
                             dynamicFontSize += (wheel.angleDelta.y > 0) ? 1 : -1;
                             dynamicFontSize = Math.min(24, Math.max(8, dynamicFontSize));
                             wheel.accepted = true;
                         } else {
                             wheel.accepted = false;
                         }
                     }

            onPressed: (mouse) => {
                           if(mouse.button === Qt.LeftButton) {

                           } else if(mouse.button === Qt.MiddleButton) {

                           } else if(mouse.button === Qt.RightButton) {
                               var pos = parent.mapToItem(root, mouseX, mouseY);
                               rightMenu.x =  pos.x;
                               rightMenu.y = pos.y;
                               rightMenu.visible = true
                           }
                       }
        }
    }

    // 右键菜单
    RightMenu {
        id: rightMenu

        onSigFindTargetKeyword: {
            $LogSearcher.find(root.selectedText);
        }

        onSigAddKeyword: {
            $LogSearcher.insertKeyword(-1, root.selectedText, "");
        }

        onSigOpenLatestLog: {
            $LogSearcher.openLatestIndexLog(0);
        }

        onSigOpenNextLatestLog: {
            $LogSearcher.openLatestIndexLog(1);
        }
    }

    Keys.onPressed: (event) => {
                        if ((event.key === Qt.Key_End) && (event.modifiers & Qt.ControlModifier)) {
                            listView.positionViewAtEnd();
                            event.accepted = true;
                        } else if ((event.key === Qt.Key_Home) && (event.modifiers & Qt.ControlModifier)) {
                            listView.positionViewAtBeginning();
                            event.accepted = true;
                        }
                    }
}
