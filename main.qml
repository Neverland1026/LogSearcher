import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: Screen.width / 5 * 3
    height: Screen.height / 5 * 3
    title: "LogSearcher"

    // 搜索窗口
    property FindResultWindow findResultWindow: null
    property bool findResultWindowAlreadyCreated: false

    // 背景
    Rectangle {
        anchors.fill: parent
        color: $LogSearcher.eyeProtectionMode ? $LogSearcher.eyeProtectionColor : "white"
        border.width: 4
        border.color: /*$LogSearcher.topMOST*/0 ? "#D81E06" : "transparent"
    }

    // 容器用于存放动态创建的标签
    KeywordTagContainer { id: keywordTagContainer; anchors { top: parent.top; left: parent.left; right: parent.right; }}

    // 日志内容 + 搜索结果
    SplitView {
        anchors { left: parent.left; right: parent.right; top: keywordTagContainer.bottom; bottom: parent.bottom; margins: 5}
        orientation: Qt.Vertical
        LogPanel {
            id: logPanel
            SplitView.preferredHeight: parent.height * 0.5
            logModel: $LogModel
            onSigCustomSummaryHeight: SplitView.preferredHeight = parent.height * (1 - number * 0.1);
        }
        handle: Rectangle { implicitHeight: 2; color: "#D81E06"; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; } }
        LogPanel {
            id: summaryLogPanel
            SplitView.fillWidth: true
            summaryMode: true
            logModel: $SummaryModel
            onSigCustomSummaryHeight: logPanel.SplitView.preferredHeight = parent.height * (1 - number * 0.1);
            onSigDoubleClicked: logPanel.positionViewAtIndex(lineNumber)
        }
    }

    // 拖拽
    DropArea { anchors.fill: parent; enabled: true;
        onDropped: (drop)=> {
                       window.requestActivate();
                       if (drop.hasUrls) {
                           //window.showMaximized();
                           var fullPath = drop.urls[0];
                           $LogSearcher.openLog(fullPath);
                       }
                   }
    }

    // 欢迎页面
    WelcomePage { anchors.fill: parent; visible: logPanel.modelCount <= 0; z: 999999; }

    // 文件有修改提示
    LogContentModifiedRemind { id: logContentModifiedRemind; anchors.horizontalCenter: parent.horizontalCenter; anchors.top: keywordTagContainer.bottom; }

    // 置顶图标
    Image {
        visible: $LogSearcher.topMOST
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 5
        width: 50
        height: width
        sourceSize.width: width * 2
        sourceSize.height: height * 2
        fillMode: Image.PreserveAspectFit
        source: "qrc:/image/topMOST.svg"
        MouseArea { anchors.fill: parent; onPressed: $LogSearcher.toggleTOPMOST(); }
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

        function onLineMaximumWidth(width) {
            logPanel.lineMaximumWidth = width;
            summaryLogPanel.lineMaximumWidth = width;
        }

        function onLineNumWidth(width) {
            var minWidth = Math.max(width, 5);
            logPanel.lineNumWidth = summaryLogPanel.lineNumWidth = minWidth;
        }

        function onLoadStart() {
            logContentModifiedRemind.visible = false;
        }

        function onLoadFinish(logPath, exist) {
            window.title = "LogSearcher" + (exist ? ("  -  " + logPath) : "");
            logPanel.resetCurrentIndex();
            summaryLogPanel.resetCurrentIndex();
            if(findResultWindowAlreadyCreated) {
                findResultWindow.close();
            }
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

            findResultWindow.setLineMaximumWidth(logPanel.lineMaximumWidth);
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
