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
        border.color: /*$LogSearcher.topMOST*/0 ? $LogSearcher.majorLogoColor : "transparent"
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
        handle: Rectangle { implicitHeight: 2; color: $LogSearcher.majorLogoColor; MouseArea { anchors.fill: parent; cursorShape: Qt.SizeVerCursor; } }
        Item {
            id: findResultItem
            SplitView.fillWidth: true
            property string findTargetKeyword: ""
            property int findCount: 0
            property int findTimeCost: 0
            Column {
                // 分隔符
                Rectangle { width: parent.width; height: 2; color: "transparent" }
                // 查找关键字、数量及耗时
                anchors.fill: parent
                Text {
                    visible: findResultItem.findTargetKeyword !== ""
                    width: parent.width
                    height: 15
                    color: "black"
                    text: "<font color='#000000'>&nbsp;匹配到 "
                          + ("<font color='#FF0000'><b>%1</b></font>").arg(findResultItem.findTargetKeyword)
                          + ("<font color='#000000'> 共 </font>")
                          + ("<font color='#FF0000'><b>%1</b></font>").arg(findResultItem.findCount)
                          + ("<font color='#000000'> 项，耗时 </font>")
                          + ("<font color='#FF0000'><b>%1</b>ms</font>").arg(findResultItem.findTimeCost)
                          + ("</font>")
                    font.family: "Consolas"
                    font.pixelSize: 14
                    textFormat: TextEdit.RichText
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                // 分隔符
                Rectangle { visible: findResultItem.findTargetKeyword !== ""; width: parent.width; height: 1; color: "black" }
                // 查找结果
                LogPanel {
                    id: findLogPanel
                    width: parent.width
                    logModel: $FindModel
                    summaryMode: true
                    onSigCustomSummaryHeight: logPanel.SplitView.preferredHeight = parent.height * (1 - number * 0.1);
                    onSigDoubleClicked: logPanel.positionViewAtIndex(lineNumber)
                }
            }
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

        function onLineNumWidth(width) {
            var minWidth = Math.max(width, 5);
            logPanel.lineNumWidth = findLogPanel.lineNumWidth = minWidth;
        }

        function onLoadStart() {
            logContentModifiedRemind.visible = false;
        }

        function onLoadFinish(logPath, exist) {
            window.title = "LogSearcher" + (exist ? ("  -  " + logPath) : "");
            logPanel.reset();
            findLogPanel.reset();
            if(findResultWindowAlreadyCreated) {
                findResultWindow.close();
            }

            console.log("DataModel available?", !!$ChartDataModel) // 应输出true
            if ($ChartDataModel) {
                console.log("Initial data count:", $ChartDataModel.count)
            }
        }

        function onFindFinish(targetKeyword, findCount, findTimeCost) {
            findResultItem.findTargetKeyword = targetKeyword;
            findResultItem.findCount = findCount;
            findResultItem.findTimeCost = findTimeCost;
//            if(!findResultWindowAlreadyCreated) {
//                var component = Qt.createComponent("qrc:/ui/FindResultWindow.qml");
//                if (component.status === Component.Ready) {
//                    findResultWindow = component.createObject(window);
//                    findResultWindow.show();

//                    findResultWindowAlreadyCreated = true;

//                    // 连接新窗口的信号到主窗口的槽
//                    findResultWindow.sigPositionViewAtIndex.connect(function cb(lineNumber) {
//                        logPanel.positionViewAtIndex(lineNumber);
//                    });
//                    findResultWindow.sigClose.connect(function cb() {
//                        findResultWindowAlreadyCreated = false;
//                    });
//                }
//            }

//            findResultWindow.setLineNumWidth(logPanel.lineNumWidth);
//            findResultWindow.findCount = findCount;
//            findResultWindow.findTimeCost = findTimeCost;
//            findResultWindow.title = "Find Result" + "  -  " + targetKeyword;
        }

        function onLogContentModified() {
            logContentModifiedRemind.visible = true;
        }
    }

    Component.onCompleted: $LogSearcher.init()
}
