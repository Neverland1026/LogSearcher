import QtQuick 2.15
import QtQuick.Window 2.1
import QtQuick.Controls 2.0
import QtQuick.Dialogs

Popup {
    id: customMenu
    width: 250
    height: listView.contentHeight

    property bool existToBeFindKeyword: false

    signal sigFindTargetKeyword();

    signal sigAddKeyword();

    signal sigOpenLatestLog();

    signal sigOpenNextLatestLog();

    background: Rectangle {
        color: "#00000000"  // 完全透明背景
        border.color: "#00FFFFFF"  // 半透明边框（可选）
    }

    contentItem: ListView {
        id: listView
        model: ListModel {
            ListElement {
                text: "查找选中字段"
                func: function() { customMenu.sigFindTargetKeyword(); }
            }
            ListElement {
                text: "将选中字段设置为关键字"
                func: function() { customMenu.sigAddKeyword(); }
            }
            ListElement {
                text: "打开最新服务端日志"
                func: function() { customMenu.sigOpenLatestLog(); }
            }
            ListElement {
                text: "打开次新服务端日志"
                func: function() { customMenu.sigOpenNextLatestLog(); }
            }
        }

        delegate: Rectangle {
            width: parent.width
            height: 40
            radius: 3
            color: mouseArea.containsMouse ? "#EEE9E9" : "#FFFAFA"
            enabled: (index === 0 || index === 1) ? existToBeFindKeyword : true
            Row {
                anchors.fill: parent

                Item { width: 20; height: 1; }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    color: enabled ? "black" : "#4F000000"
                    font { pointSize: 13; bold: true; family: "Consola"; }
                    text: model.text
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: parent.enabled
                onClicked: {
                    customMenu.close();
                    model.func();
                }
            }
        }
    }
}
