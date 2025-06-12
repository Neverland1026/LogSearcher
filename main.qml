import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings 1.1
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.desktopAvailableWidth / 3 * 2
    height: width * 9 / 16
    title: "LogSearcher"

    // 背景
    Rectangle {
        anchors.fill: parent
        color: "#1E1E1E"
    }

    // 容器用于存放动态创建的标签
    KeywordTagContainer {
        id: keywordTagContainer
        anchors { top: parent.top; left: parent.left; right: parent.right; }
    }

    // 日志加载进度
    Rectangle {
        id: progressBar
        visible: value > 0
        anchors { left: keywordTagContainer.left; top: keywordTagContainer.bottom }
        width: value * keywordTagContainer.width
        height: 2
        color: "cyan"
        property real value: -1
    }

    // 日志内容 + 搜索结果
    SplitView {
        id: splitView
        anchors { left: parent.left; right: parent.right; top: keywordTagContainer.bottom; bottom: parent.bottom; margins: 5}
        orientation: Qt.Vertical

        handle: Rectangle {
            implicitHeight: 1
            color: "white"
            MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; }
        }

        LogPanel {
            id: logPanel
            implicitHeight: parent.height * 0.8
            logModel: $LogModel
        }

        LogPanel {
            id: resultPanel
            SplitView.fillHeight: true
            logModel: $ResultModel
        }
    }

    // 拖拽
    DropArea {
        anchors.fill: parent
        enabled: true
        onDropped: (drop)=> {
                       window.requestActivate();
                       if (drop.hasUrls) {
                           var fullPath = drop.urls[0];
                           $LogSearcher.search(fullPath);
                           window.title = fullPath.toString().substring(8, fullPath.toString().length);
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
            logPanel1.lineNumWidth = logPanel2.lineNumWidth = width;
        }

        function onProgressChanged(value) {
            progressBar.value = value;
        }
    }

    // 保存和恢复 SplitView 的状态
    Settings {
        id: settings
        property var splitViewState
    }
    Component.onCompleted: splitView.restoreState(settings.splitViewState)
    Component.onDestruction: settings.splitViewState = splitView.saveState()

}
