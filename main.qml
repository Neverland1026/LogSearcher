import QtQuick 2.15
import QtQuick.Controls 2.15
import "./ui"

ApplicationWindow {
    id: window
    visible: true
    width: 1000
    height: 600
    title: "LogSearcher"

    // 背景
    Rectangle {
        anchors.fill: parent
        color: "#1E1E1E"
    }

    // 日志文件标题
    Text {
        id: logNameText
        width: parent.width
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font { pointSize: 20; bold: true }
        color: "#FC9000"
        text: "Let's search it !"
    }

    // 容器用于存放动态创建的标签
    Rectangle {
        id: tagContainer
        height: {
            var baseHeigght = 50;

            var extraHeight = 60;
            var itemBegin = repeater.itemAt(0);
            var itemEnd = repeater.itemAt(repeater.count - 1);
            if(itemBegin && itemEnd) {
                extraHeight = Math.floor((itemEnd.y - itemBegin.y) / itemBegin.height) * extraHeight;
            }

            return (baseHeigght + extraHeight);
        }
        anchors {
            top: logNameText.bottom
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        color: "#2FFFFFFF"

        // 使用 Flow 管理标签
        Flow {
            anchors.fill: tagContainer
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5
            flow: Flow.LeftToRight

            Repeater {
                id: repeater
                model: tagList
                delegate: TagModel {
                    keyword: tagList.get(index).keyword
                    keywordColor: tagList.get(index).color
                    delBtnVisible: tagList.count > 1

                    onSigUpdate: {
                        tagList.get(index).keyword = keyword.trim();
                        tagList.get(index).color = keywordColor;
                        $LogSearcher.insertKeyword(index, tagList.get(index).keyword, tagList.get(index).color);
                        //$LogSearcher.refresh();
                    }

                    onSigRemove: {
                        $LogSearcher.removeKeyword(index);
                    }

                    onSigAccepted: {
                        if(index === tagList.count - 1) {
                            $LogSearcher.insertKeyword(-1, "", "");
                        }
                    }
                }
            }
        }

        // 存储标签数据
        ListModel { id: tagList }
    }

    // 搜索结果
    Row {
        anchors {
            left: parent.left
            right: parent.right
            top: tagContainer.bottom
            bottom: parent.bottom
            margins: 5
        }

        spacing: 5

        LogPanel {
            id: logPanel1
            width: (parent.width - parent.spacing) / 2
            height: parent.height
            logModel: $LogModel1
        }

        LogPanel {
            id: logPanel2
            width: (parent.width - parent.spacing) / 2
            height: parent.height
            logModel: $LogModel2
        }
    }

    // 悬浮显示/隐藏前缀按钮
    Rectangle {
        width: 50
        height: width
        color: "#2FFFFFFF"
        radius: width
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#7FFFFFFF"
            onExited: parent.color = "#2FFFFFFF"
            onPressed: $LogSearcher.togglePrefix();

            Image {
                width: parent.width * 0.7
                height: width
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/refresh.svg"
            }
        }
        z: 99999999
    }

    // 悬浮刷新按钮
    Rectangle {
        width: 50
        height: width
        color: "#2FFFFFFF"
        radius: width
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 80

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#7FFFFFFF"
            onExited: parent.color = "#2FFFFFFF"
            onPressed: $LogSearcher.refresh();

            Image {
                width: parent.width * 0.7
                height: width
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/refresh.svg"
            }
        }
        z: 99999999
    }

    // 拖拽
    DropArea {
        anchors.fill: parent
        enabled: true
        onDropped: (drop)=> {
                       window.requestActivate();
                       if (drop.hasUrls) {
                           logNameText.text = drop.urls[0];
                           $LogSearcher.search(logNameText.text);
                           logNameText.text = logNameText.text.split("/").pop();
                       }
                   }
    }

    // C++ 消息响应
    Connections {
        target: $LogSearcher

        function onAddKeywordFinish(keyword, color) {
            tagList.append({ keyword: keyword, color: color });
        }

        function onRemoveKeywordFinish(index) {
            tagList.remove(index);
        }
    }

    // C++ 消息响应
    Connections {
        target: $LogModel1

        function onRowsInserted() {
            logPanel1.positionViewAtEnd();
        }
    }

    Connections {
        target: $LogModel2

        function onRowsInserted() {
            logPanel2.positionViewAtEnd();
        }
    }

    Component.onCompleted: {

    }

}
