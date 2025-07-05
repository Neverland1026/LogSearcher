import QtQuick 2.15
import QtQuick.Controls 2.15

// LogPanel
Rectangle {
    id: root

    width: 350; height: 350
    color: "transparent"
    //border.color: "cyan"; border.width: 1
    clip: true

    // 数据模型
    property var logModel

    // 模型项数
    readonly property alias modelCount: listView.count

    // 全局动态字体尺寸
    property real dynamicFontSize: 14

    // 文件行号宽度
    property int lineNumWidth: 7

    // 实时选中的内容
    property string selectedText: ""

    // 当前行获取焦点颜色
    readonly property string focusBackgroundColor: "#1FFF0000"

    // 当前行高亮颜色
    readonly property string highlightBackgroundColor: summaryMode ? "transparent" : "#3FFFFF00"

    // 行索引记录
    property var positionRecorder: QtObject {
        property int first: -1   // 当前行索引
        property int second: -1  // 上一行索引
    }

    // 是否是搜索内容汇总模式
    property bool summaryMode: false

    // 重置索引到第一行
    function resetCurrentIndex() {
        if(modelCount >= 0) {
            listView.currentIndex = 0;
        }
    }

    // 跳转到指定行
    function positionViewAtIndex(lineNumber) {
        if(lineNumber < 0 || lineNumber >= modelCount)
            return;

        // 处理既有行
        if(positionRecorder.first >= 0) {
            var lastItem1 = listView.itemAtIndex(positionRecorder.first);
            if (lastItem1) {
                var rect1 = lastItem1.backgroundRect;
                rect1.color = ($LogSearcher.isHighlight(positionRecorder.first) ? root.highlightBackgroundColor : "transparent");
            }
        }
        if(positionRecorder.second >= 0) {
            var lastItem2 = listView.itemAtIndex(positionRecorder.second);
            if (lastItem2) {
                var rect2 = lastItem2.backgroundRect;
                rect2.color = ($LogSearcher.isHighlight(positionRecorder.second) ? root.highlightBackgroundColor : "transparent");
            }
        }

        // 更新记录
        positionRecorder.second = positionRecorder.first = lineNumber;

        // 设置当前行
        listView.currentIndex = lineNumber;

        // 跳转到当前行
        listView.positionViewAtIndex(lineNumber, ListView.Center);

        // 处理当前行
        var curItem = listView.itemAtIndex(positionRecorder.first);
        if (curItem) {
            var rect = curItem.backgroundRect;
            rect.color = ($LogSearcher.isHighlight(positionRecorder.first) ? root.highlightBackgroundColor : root.focusBackgroundColor);
        }
    }

    // 打开搜索框
    function openFindWindow(findText) {
        var component1 = Qt.createComponent("qrc:/ui/FindWindow.qml");
        if (component1.status === Component.Ready) {
            var findResultWindow = component1.createObject(window);
            findResultWindow.setFindText(findText);
            findResultWindow.show();
        }
    }

    // 打开行数跳转框
    function openSpecifiedLineNumberWindow() {
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
    }

    signal sigDoubleClicked(var lineNumber);

    signal sigCustomSummaryHeight(var number);

    // 主布局
    ScrollView {
        anchors.fill: parent

        clip: true

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        ListView {
            id: listView

            width: parent.width
            height: parent.height

            anchors.centerIn: parent
            anchors.margins: 10

            model: logModel

            cacheBuffer: 500
            boundsBehavior: Flickable.DragOverBounds
            maximumFlickVelocity: 1200
            flickDeceleration: 10000
            clip: true

            // 不转发给任何子元素
            Keys.forwardTo: []

            delegate: Item {
                id: item

                width: listView.width * 2
                height: visible ? (dynamicFontSize * 1.2) : 0

                visible: root.summaryMode ? isVisible : true

                property alias backgroundRect: backgroundRect

                Rectangle { id: backgroundRect; anchors.fill: parent }

                Component.onCompleted: {
                    backgroundRect.color = ($LogSearcher.isHighlight(index) ? root.highlightBackgroundColor : "transparent");
                }

                // 行号
                Text {
                    id: lineNumText
                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: lineNumWidth * 10
                    text: index + 1
                    font.family: "Consolas"
                    font.pixelSize: dynamicFontSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "black"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            $LogSearcher.toggleHighlight(index);
                            backgroundRect.color = ($LogSearcher.isHighlight(index) ? root.highlightBackgroundColor : "transparent");
                        }
                    }
                }

                // 搜索汇总面板专用 - 对应真实文件中的行号
                Text {
                    id: realLineNumText
                    visible: root.summaryMode
                    anchors {
                        left: lineNumText.right
                        leftMargin: 5
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: visible ? lineNumText.width : 0
                    text: lineNumber + 1
                    font.family: "Consolas"
                    font.pixelSize: dynamicFontSize
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "black"
                }

                // 内容
                TextEdit {
                    id: textEdit
                    anchors {
                        left: realLineNumText.right
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

                    function find_and_select() {
                        var retVal = $LogSearcher.getKeywordPos(lineNumber, root.selectedText);
                        if(retVal[0] >= 0 && lineNumber !== positionRecorder.first) {
                            textEdit.select(retVal[0], retVal[1]);
                        } else {
                            textEdit.deselect();
                        }
                    }

                    property string rootSelectedText: root.selectedText
                    //onRootSelectedTextChanged: find_and_select()

                    TapHandler {
                        cursorShape: Qt.BusyCursor
                        onDoubleTapped: {
                            sigDoubleClicked(lineNumber);
                        }
                    }

                    onActiveFocusChanged: {
                        // 只考虑获取焦点的情况，此时一并处理失焦的行
                        if(activeFocus) {
                            // 更新记录
                            positionRecorder.second = positionRecorder.first;
                            positionRecorder.first = index;

                            // 处理上一次选中的行
                            if(positionRecorder.second >= 0) {
                                var lastTargetItem = listView.itemAtIndex(positionRecorder.second);
                                if (lastTargetItem) {
                                    var rect = lastTargetItem.backgroundRect;
                                    rect.color = ($LogSearcher.isHighlight(positionRecorder.second) ? root.highlightBackgroundColor : "transparent");
                                }
                            }

                            // 处理当前行
                            backgroundRect.color = root.focusBackgroundColor;

                            // 重定位
                            listView.currentIndex = index;
                        }
                    }
                }
            }

            // 拦截滚轮事件
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.NoButton | Qt.RightButton
                hoverEnabled: true

                onWheel: (wheel) => {
                             if (wheel.modifiers & Qt.ControlModifier) {
                                 dynamicFontSize += (wheel.angleDelta.y > 0) ? 1 : -1;
                                 dynamicFontSize = Math.min(24, Math.max(8, dynamicFontSize));
                                 wheel.accepted = true;
                             } else {
                                 var delta = wheel.angleDelta.y > 0 || wheel.pixelDelta.y > 0;
                                 var step = root.modelCount > 300 ? 5 : 3;
                                 var newIndex = listView.currentIndex + (delta > 0 ? -1 : 1) * step;
                                 newIndex = Math.max(0, Math.min(newIndex, modelCount - 1));
                                 listView.positionViewAtIndex(newIndex, ListView.Center);
                                 listView.currentIndex = newIndex;
                                 wheel.accepted = true;
                             }
                         }

                onPressed: (mouse) => {
                               if(mouse.button === Qt.LeftButton) {

                               } else if(mouse.button === Qt.MiddleButton) {

                               } else if(mouse.button === Qt.RightButton) {
                                   if(modelCount > 0) {
                                       var pos = parent.mapToItem(root, mouseX, mouseY);

                                       // 计算菜单可能的位置
                                       var menuWidth = rightMenu.width;
                                       var menuHeight = rightMenu.height;
                                       var windowWidth = root.width;
                                       var windowHeight = root.height;

                                       // 调整X坐标防止右侧超出
                                       rightMenu.x = Math.min(pos.x, windowWidth - menuWidth);

                                       // 调整Y坐标防止底部超出
                                       rightMenu.y = Math.min(pos.y, windowHeight - menuHeight);

                                       rightMenu.visible = true
                                   }
                               }
                           }
            }
        }
    }

    // 右键菜单
    RightMenu {
        id: rightMenu

        majorPanel: !root.summaryMode

        onSigFindTargetKeyword: {
            /*root.openFindWindow(root.selectedText);*/
            $LogSearcher.find(root.selectedText, true, false);
        }

        onSigAddKeyword: {
            $LogSearcher.insertKeyword(-1, root.selectedText, "");
        }

        onSigToggleHighlight: {
            if(positionRecorder.first >= 0) {
                var curItem = listView.itemAtIndex(positionRecorder.first);
                if (curItem) {
                    var rect = curItem.backgroundRect;
                    $LogSearcher.toggleHighlight(positionRecorder.first);
                    rect.color = ($LogSearcher.isHighlight(positionRecorder.first) ? root.highlightBackgroundColor : "transparent");
                }
            }
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
                                listView.currentIndex = root.modelCount - 1;
                                event.accepted = true;
                            } else if (event.key === Qt.Key_Home) {
                                listView.positionViewAtBeginning();
                                listView.currentIndex = 0;
                                event.accepted = true;
                            } else if (event.key === Qt.Key_F) {
                                root.openFindWindow(root.selectedText);
                                event.accepted = true;
                            } else if (event.key === Qt.Key_G) {
                                root.openSpecifiedLineNumberWindow();
                                event.accepted = true;
                            }
                        } else {
                            //console.log("event.key =", event.key);
                            // 判断是否数字键（主键盘区）
                            if (event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
                                root.sigCustomSummaryHeight(event.key - Qt.Key_0);
                            }
                            // 判断是否小键盘数字键
                            if (event.key >= Qt.Key_Keypad0 && event.key <= Qt.Key_Keypad9) {
                                root.sigCustomSummaryHeight(event.key - Qt.Key_Keypad0);
                            }
                            // Up 和 Down
                            if (event.key === Qt.Key_Up || event.key === Qt.Key_Down) {
                                // 更新记录
                                //console.log("event.key === Qt.Key_Up || event.key === Qt.Key_Down")
                                if (event.key === Qt.Key_Up) {
                                    positionRecorder.second--;
                                    positionRecorder.first--;
                                } else if (event.key === Qt.Key_Down) {
                                    positionRecorder.second++;
                                    positionRecorder.first++;
                                }

                                positionViewAtIndex(positionRecorder.first);

                                event.accepted = true;
                            }
                        }
                    }

}
