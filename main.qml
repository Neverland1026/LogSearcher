import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.settings
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

    // list 转 JSONArray
    function listModelToJSONArray(listModel) {
        var jsonArray = [];
        for (var i = 0; i < listModel.count; ++i) {
            var element = listModel.get(i);
            var jsonObject = {
                keyword: element.keyword/*,
                age: element.age*/
            };
            jsonArray.push(jsonObject);
        }

        console.log("jsonArray ===>", jsonArray);

        return jsonArray;
    }

    // 日志文件标题
    Text {
        id: logNameText
        width: parent.width
        height: 70
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font { pointSize: 25; bold: true }
        color: "white"
        text: "Let's search it !"
    }

    // 容器用于存放动态创建的标签
    Rectangle {
        id: tagContainer
        height: 60
        anchors {
            top: logNameText.bottom
            left: parent.left
            leftMargin: 10
            right: addTagBtn.left
            rightMargin: 10
        }
        color: "#2FFFFFFF"

        // 标签样式
        Component {
            id: tagModelComponent
            TagModel {
                anchors.verticalCenter: parent.verticalCenter
                keyword: tagList.get(index).keyword
                delBtnVisible: tagList.count > 1

                onSigInsert: {
                    keyword = keyword.trim();
                    $LogSearcher.insertKeyword(index, keyword, keywordColor);
                }

                onSigRemove: {
                    $LogSearcher.removeKeyword(index);
                }

                onSigAccepted: {
                    if(index === tagList.count - 1) {
                        tagContainer.addTag();
                    }
                }
            }
        }

        // 滚动条
        ScrollView {
            anchors.fill: parent

            contentWidth: listView.width
            contentHeight: listView.height

            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AlwaysOff
            ScrollBar.horizontal.interactive: true
            ScrollBar.vertical.interactive: true
            //flickableDirection: Flickable.VerticalAndHorizontal

            hoverEnabled: true
            wheelEnabled: true
            focus: true

            // 使用 ListView 管理标签
            ListView {
                id: listView
                anchors.fill: parent
                model: tagList
                delegate: tagModelComponent
                orientation: ListView.Horizontal
                spacing: 0
                clip: true
            }
        }

        // 存储标签数据
        ListModel {
            id: tagList
            ListElement { keyword: "Input your keyword" }
        }

        // 动态添加标签
        function addTag() {
            tagList.append({ keyword: "     " })
        }
    }

    // 标签添加按钮
    Rectangle {
        id: addTagBtn
        width: height
        height: tagContainer.height
        color: "#2FFFFFFF"
        radius: 5
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.verticalCenter: tagContainer.verticalCenter

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.color = "#7FFFFFFF"
            onExited: parent.color = "#2FFFFFFF"
            onPressed: tagContainer.addTag()

            Image {
                width: parent.width * 0.5
                height: width
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/add.svg"
            }
        }
    }

    // 搜索结果
    ScrollView {
        anchors {
            left: parent.left
            right: parent.right
            top: tagContainer.bottom
            bottom: parent.bottom
            margins: 10
        }

        contentWidth: textArea.width
        contentHeight: textArea.height

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        //ScrollBar.vertical.policy: ScrollBar.AlwaysOff
        ScrollBar.horizontal.interactive: true

        TextArea {
            id: textArea
            anchors.fill: parent
            wrapMode: TextArea.Wrap

            // 设置背景颜色
            background: Rectangle {
                color: "#2FFFFFFF"
                radius: 5
            }

            // 设置字体
            font.family: "Consolas"
            font.pointSize: 16
            font.bold: true
            font.italic: false

            textFormat: Text.RichText
        }
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

        function onRemoveKeywordFinish(index) {
            tagList.remove(index);
        }

        function onAppendContent(content) {
            textArea.append(content);
        }
    }

    // 配置文件
    Settings {
        id: settings
        property var keywords: JSON.stringify(listModelToJSONArray(tagList))
    }

    Component.onCompleted: {
        var storedData = settings.keywords;
        console.log("111", storedData)
        if (storedData !== undefined) {
            var jsonArray = JSON.parse(storedData);
            console.log("222", jsonArray.length, "---", jsonArray);

            tagList.clear();
            if(0 === jsonArray.length) {
                tagList.append({ keyword: "__Calibrate__" });
            } else {
                for (var i = 0; i < jsonArray.length; ++i) {
                    var element = jsonArray[i];
                    console.log("333", i, "---", element);
                    tagList.append({ keyword: element.toString() });
                }
            }
        }
    }

}
