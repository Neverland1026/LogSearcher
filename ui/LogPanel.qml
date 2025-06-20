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

    property int lastPosition: -1

    function positionViewAtIndex(lineNumber) {
        listView.positionViewAtIndex(lineNumber, ListView.Center);

        if(lastPosition >= 0) {
            var lastTargetItem = listView.itemAtIndex(lastPosition);
            if (lastTargetItem) {
                var firstBackgroundRect = lastTargetItem.backgroundRect;
                firstBackgroundRect.color = "transparent";
            }
        }

        lastPosition = lineNumber;
        var targetItem = listView.itemAtIndex(lineNumber);
        if (targetItem) {
            var newBackgroundRect = targetItem.backgroundRect;
            newBackgroundRect.color = "#1FFF0000";
        }
    }

    signal sigDoubleClicked(var lineNumber);

    ListView {
        id: listView

        anchors.fill: parent
        anchors.margins: 10

        model: logModel

        cacheBuffer: 2000
        boundsBehavior: Flickable.DragOverBounds
        maximumFlickVelocity: 1200
        flickDeceleration: 10000
        clip: true

        delegate:  Item {
            width: listView.width
            height: dynamicFontSize * 1.2

            property alias backgroundRect: backgroundRect

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
                    cursorShape: Qt.BusyCursor
                    onDoubleTapped: {
                        sigDoubleClicked(lineNumber);
                    }
                }

                onActiveFocusChanged: {
                    backgroundRect.color = (activeFocus) ? "#1FFF0000" : "transparent";
                    lineNumText.font.bold = activeFocus;

                    if(lastPosition >= 0) {
                        var lastTargetItem = listView.itemAtIndex(lastPosition, listView.contentY);
                        if (lastTargetItem) {
                            var firstBackgroundRect = lastTargetItem.backgroundRect;
                            firstBackgroundRect.color = "transparent";
                        }
                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            background: Rectangle {
                color: "transparent"
            }
        }
        //ScrollBar.horizontal: ScrollBar {
        //    policy: ScrollBar.AsNeeded
        //    background: Rectangle {
        //        color: "transparent"
        //    }
        //}

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
                        if((event.modifiers & Qt.ControlModifier)) {
                            if (event.key === Qt.Key_End) {
                                listView.positionViewAtEnd();
                                event.accepted = true;
                            } else if (event.key === Qt.Key_Home) {
                                listView.positionViewAtBeginning();
                                event.accepted = true;
                            } else if (event.key === Qt.Key_F) {
                                var component = Qt.createComponent("qrc:/ui/FindWindow.qml");
                                if (component.status === Component.Ready) {
                                    var findResultWindow = component.createObject(window);
                                    findResultWindow.setFindText(root.selectedText);
                                    findResultWindow.show();
                                }
                                event.accepted = true;
                            }
                        }
                    }
}
