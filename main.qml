import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.width / 5 * 2
    height: Screen.height / 5 * 2
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
        handle: Rectangle { implicitHeight: 5; color: "#44000000"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; } }
        LogPanel { id: resultPanel; SplitView.fillWidth: true; logModel: $ResultModel; onSigDoubleClicked: logPanel.positionViewAtIndex(lineNumber); }
    }

    // 拖拽
    DropArea { anchors.fill: parent; enabled: true;
        onDropped: (drop)=> {
                       window.requestActivate();
                       if (drop.hasUrls) {
                           //window.showMaximized();
                           var fullPath = drop.urls[0];
                           $LogSearcher.openLog(fullPath);
                           window.title = "LogSearcher" + "  -  " + fullPath.toString().substring(8, fullPath.toString().length);
                       }
                   }
    }

    // 欢迎页面
    WelcomePage { anchors.fill: parent; visible: logPanel.modelCount <= 0; z: 999999; }

    // 文件有修改提示
    LogContentModifiedRemind { id: logContentModifiedRemind; anchors.horizontalCenter: parent.horizontalCenter; anchors.top: keywordTagContainer.bottom; }

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

        function onLogContentModified() {
            logContentModifiedRemind.visible = true;
        }
    }

    Component.onCompleted: $LogSearcher.init()
}
