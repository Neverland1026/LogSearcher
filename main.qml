import QtQuick 2.15
import QtQuick.Controls 2.15
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.desktopAvailableWidth / 4 * 3
    height: Screen.desktopAvailableHeight / 4 * 3
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
        handle: Rectangle { implicitHeight: 1; color: "black"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; } }
        SplitView {
            SplitView.fillHeight: true
            orientation: Qt.Horizontal
            LogPanel { id: resultPanel; SplitView.fillWidth: true; logModel: $ResultModel; }
            handle: Rectangle { implicitWidth: 1.5; color: "black"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeHorCursor; } }
            LogPanel { id: findPanel; implicitWidth: parent.width * 0.5; logModel: $FindModel; }
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

        function onFindFinish(targetKeyword) {
            if(!findResultWindowAlreadyCreated) {
                var component = Qt.createComponent("qrc:/ui/FindResultWindow.qml");
                if (component.status === Component.Ready) {
                    findResultWindow = component.createObject(window);
                    findResultWindow.show();

                    findResultWindowAlreadyCreated = true;

                    // 连接新窗口的信号到主窗口的槽
                    findResultWindow.sigPositionViewAtIndex.connect(function cb(index) {
                        console.log("__FIND__", index);
                    });
                    findResultWindow.sigClose.connect(function cb() {
                        findResultWindowAlreadyCreated = false;
                    });
                }
            }

            findResultWindow.title = "Find Result" + "  -  " + targetKeyword;
        }
    }

    Shortcut {
        sequence: "Ctrl+End"
        onActivated: console.log("Ctrl+End triggered")
        onActivatedAmbiguously: console.log("Ctrl+End triggered")
    }

    Component.onCompleted: $LogSearcher.init()
}
