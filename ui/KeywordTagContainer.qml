import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root

    function append(keyword, color) {
        tagList.append({ keyword: keyword, color: color });
    }

    function remove(index) {
        tagList.remove(index);
    }

    height: {
        var baseHeigght = 38;

        var extraHeight = 60;
        var itemBegin = repeater.itemAt(0);
        var itemEnd = repeater.itemAt(repeater.count - 1);
        if(itemBegin && itemEnd) {
            extraHeight = Math.floor((itemEnd.y - itemBegin.y) / itemBegin.height) * extraHeight;
        }

        return (baseHeigght + extraHeight);
    }
    width: 400
    color: "#50FFFFFF"

    // 使用 Flow 管理标签
    Flow {
        anchors.fill: root
        anchors.verticalCenter: parent.verticalCenter
        spacing: 0
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
