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

    property real dynamicFontSize: 14  // 全局动态字体尺寸

    property int lineNumWidth: 7

    property string selectedText: ""

    property bool selectedTextIsKeyword: false

    property int lastPosition: -1

    property alias modelCount: listView.count

    function positionViewAtIndex(lineNumber) {
        if(lineNumber < 0 || lineNumber >= modelCount)
            return;

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

        cacheBuffer: 5000
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
                    root.selectedTextIsKeyword = $LogSearcher.isKeyword(root.selectedText);
                    rightMenu.existToBeFindKeyword = (selectedText !== "");
                }

                function find_and_select() {
                    var retVal = $LogSearcher.getKeywordPos(lineNumber, root.selectedText);
                    if(retVal[0] >= 0) {
                        textEdit.select(retVal[0], retVal[1]);
                    } else {
                        textEdit.deselect();
                    }
                }

                property string rootSelectedText: root.selectedText
                onRootSelectedTextChanged: find_and_select()

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

                    lastPosition = index;
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
                               if(modelCount > 0) {
                                   var pos = parent.mapToItem(root, mouseX, mouseY);
                                   rightMenu.x =  pos.x;
                                   rightMenu.y = pos.y;
                                   rightMenu.visible = true
                               }
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

    // 延时高亮跳转到的行，不延时会有问题
    Timer {
        id: timer
        running: false;
        interval: 200;
        property int lineNumber: -1
        onTriggered: positionViewAtIndex(lineNumber);
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
                                var component1 = Qt.createComponent("qrc:/ui/FindWindow.qml");
                                if (component1.status === Component.Ready) {
                                    var findResultWindow = component1.createObject(window);
                                    findResultWindow.setFindText(root.selectedText);
                                    findResultWindow.show();
                                }
                                event.accepted = true;
                            } else if (event.key === Qt.Key_G) {
                                var component2 = Qt.createComponent("qrc:/ui/SpecifiedLineNumberWindow.qml");
                                if (component2.status === Component.Ready) {
                                    var specifiedLineNumberWindow = component2.createObject(window);
                                    specifiedLineNumberWindow.show();

                                    specifiedLineNumberWindow.sigJumpToTheSpecifiedLine.connect(function cb(lineNumber) {
                                        if(lineNumber > 0 && lineNumber < modelCount) {
                                            timer.lineNumber = lineNumber - 1;
                                            timer.restart();
                                        }
                                    });
                                }
                                event.accepted = true;
                            }
                        } else {
                            if (event.key === Qt.Key_Up || event.key === Qt.Key_Down) {
//                                console.log("__UP_DUWN__", lastPosition)
//                                if (event.key === Qt.Key_Up) lastPosition--;
//                                if (event.key === Qt.Key_Down) lastPosition++;
//                                console.log("__UP_DUWN__", lastPosition)

//                                var targetItem = listView.itemAtIndex(lastPosition - 1);
//                                if (targetItem) {
//                                    var newBackgroundRect = targetItem.backgroundRect;
//                                    newBackgroundRect.color = "#1FFF0000";
//                                }

                                listView.currentIndex = lastPosition + 1;
                                listView.positionViewAtIndex(lastPosition, ListView.SnapPosition);

                                event.accepted = true;
                            }
                        }
                    }
}
