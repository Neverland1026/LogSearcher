import QtQuick 2.15
import QtQuick.Controls 2.15

// KeywordTagContainer
Rectangle {
    id: root

    // 存储标签数据
    ListModel { id: tagList }

    function append(keyword, color) {
        tagList.append({ keyword: keyword, color: color });
    }

    function remove(index) {
        tagList.remove(index);
    }

    height: flow.rowCount > 2 ? ((flow.rowCount - 1) * 40) : 38
    width: 400
    color: "#1A000000"

    // 使用 Flow 管理标签
    Flow {
        id: flow
        anchors.fill: root
        anchors.verticalCenter: parent.verticalCenter
        spacing: 1
        flow: Flow.LeftToRight

        readonly property int rowCount: {
            if(children.length > 0) {
                return Math.ceil(childrenRect.height / (children[0].height));
            }
            return 0;
        }

        Repeater {
            id: repeater
            model: tagList
            delegate: TagModel {
                keyword: tagList.get(index).keyword
                keywordColor: tagList.get(index).color
                delBtnVisible: tagList.count > 1

                onSigUpdate: {
                    tagList.set(index, { keyword: keyword__.trim(), color: color__ });
                    $LogSearcher.insertKeyword(index, keyword__.trim(), color__);
                }

                onSigRemove: {
                    $LogSearcher.removeKeyword(index);
                }

                onSigAccepted: {
                    $LogSearcher.openLog("", true);
                }
            }
        }

        Rectangle {
            width: 40
            height: width
            color: "transparent"

            Image {
                width: 25
                height: width
                anchors.centerIn: parent
                sourceSize.width: width * 2
                sourceSize.height: height * 2
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/add.svg"
                MouseArea {
                    anchors.fill: parent
                    onPressed: $LogSearcher.insertKeyword(-1, "", "");
                }
            }
        }
    }
}
