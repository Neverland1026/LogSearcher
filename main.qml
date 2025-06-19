import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.width / 3 * 2
    height: Screen.height / 3 * 2
    title: "LogSearcher"

    // 搜索窗口
    property FindResultWindow findResultWindow: null
    property bool findResultWindowAlreadyCreated: false

    // 背景
    Rectangle { anchors.fill: parent; color: "white"; }

    // 容器用于存放动态创建的标签
    KeywordTagContainer { id: keywordTagContainer; anchors { top: parent.top; left: parent.left; right: parent.right; }}

    // 日志内容 + 搜索结果
    SplitView {
        anchors { left: parent.left; right: parent.right; top: keywordTagContainer.bottom; bottom: parent.bottom; margins: 5}
        orientation: Qt.Vertical
        LogPanel { id: logPanel; implicitHeight: parent.height * 0.7; logModel: $LogModel; }
        handle: Rectangle { implicitHeight: 3; color: "#22000000"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; } }
        SplitView {
            SplitView.fillHeight: true
            orientation: Qt.Horizontal
            LogPanel { id: resultPanel; SplitView.fillWidth: true; logModel: $ResultModel;
                onSigDoubleClicked: {
                    logPanel.positionViewAtIndex(lineNumber);
                }
            }
            handle: Rectangle { implicitWidth: 3; color: "#22000000"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeHorCursor; } }
            LogPanel { id: findPanel; implicitWidth: parent.width * 0.5;/* logModel: $FindModel;*/ }
        }
    }

    // 拖拽
    DropArea { anchors.fill: parent; enabled: true;
        onDropped: (drop)=> {
                       window.requestActivate();
                       if (drop.hasUrls) {
                           var fullPath = drop.urls[0];
                           $LogSearcher.openLog(fullPath);
                           window.title = "LogSearcher" + "  -  " + fullPath.toString().substring(8, fullPath.toString().length);
                       }
                   }
    }

    // C++ 消息响应
    Connections {
        target: $LogSearcher

        function onAddKeywordFinish(keyword, color) {
            keywordTagContainer.append(keyword, color);
        }

        function onRemoveKeywordFinish(index) {
            keywordTagContainer.remove(index);
        }

        function onLineNumWidth(width) {
            var minWidth = Math.max(width, 5);
            logPanel.lineNumWidth = resultPanel.lineNumWidth = minWidth;
        }

        function onLoadStart() {

        }

        function onLoadFinish() {

        }

        function onFindFinish(targetKeyword, findCount, findTimeCost) {
            if(!findResultWindowAlreadyCreated) {
                var component = Qt.createComponent("qrc:/ui/FindResultWindow.qml");
                if (component.status === Component.Ready) {
                    findResultWindow = component.createObject(window);
                    findResultWindow.show();

                    findResultWindowAlreadyCreated = true;

                    // 连接新窗口的信号到主窗口的槽
                    findResultWindow.sigPositionViewAtIndex.connect(function cb(lineNumber) {
                        logPanel.positionViewAtIndex(lineNumber);
                    });
                    findResultWindow.sigClose.connect(function cb() {
                        findResultWindowAlreadyCreated = false;
                    });
                }
            }

            findResultWindow.setLineNumWidth(logPanel.lineNumWidth);
            findResultWindow.findCount = findCount;
            findResultWindow.findTimeCost = findTimeCost;
            findResultWindow.title = "Find Result" + "  -  " + targetKeyword;
        }
    }

    Component.onCompleted: $LogSearcher.init()
}
