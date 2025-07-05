import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs

// TagModel
Rectangle {
    id: root
    width: contentRect.width + 10
    height: 40
    color: "transparent"
    //border.color: "yellow"; border.width: 1;

    property alias keyword: textInput.text
    property alias keywordColor: colorRect.color
    property alias delBtnVisible: delBtnRect.visible

    property bool ignoreKeyword: false

    signal sigUpdate(var keyword__, var color__);
    signal sigRecolorful();
    signal sigRemove();
    signal sigAccepted();

    // 内容区域
    Rectangle {
        id: contentRect
        width: colorMangerRect.width + 20 + textInput.width + 20
        height: parent.height - 10
        anchors.centerIn: parent
        radius: 5
        color: root.ignoreKeyword ? "#CFCFCF" : ($LogSearcher.eyeProtectionMode ? $LogSearcher.eyeProtectionColor : "white")
        //border.color: "black"; border.width: 0.5;

        // 控制窗口
        Rectangle {
            id: colorMangerRect
            width: parent.height * 0.35
            height: parent.height
            radius: width
            color: "transparent"
            anchors.left: parent.left
            anchors.leftMargin: 7
            //border.color: "red"; border.width: 1;

            Column {
                id: column
                width: parent.width
                height: parent.height
                anchors.centerIn: parent
                topPadding: (parent.height - (colorRect.height + enabledKeywordRect.height + spacing)) / 2
                spacing: 2

                // 关键字颜色选择
                Rectangle {
                    id: colorRect
                    width: parent.width
                    height: width
                    radius: width
                    color: "yellow"
                    enabled: !root.ignoreKeyword
                    opacity: root.ignoreKeyword ? 0.3 : 1.0
                    //border.color: "#000000"; border.width: 1;

                    MouseArea {
                        anchors.fill: parent
                        onPressed: colorDialog.open()
                    }
                }

                // 是否搜索当前关键字
                Rectangle {
                    id: enabledKeywordRect
                    width: colorRect.width
                    height: colorRect.height
                    radius: width
                    color: "#4F4F4F"
                    border.color: "#4F4F4F"; border.width: 1;

                    MouseArea {
                        anchors.fill: parent
                        onPressed: { root.ignoreKeyword = !root.ignoreKeyword; root.sigRecolorful(); }
                    }
                }
            }
        }

        // 查询关键字
        TextInput {
            id: textInput
            enabled: !root.ignoreKeyword
            height: contentRect.height
            anchors.left: colorMangerRect.right
            anchors.leftMargin: 10
            anchors.verticalCenter: colorMangerRect.verticalCenter
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            font.family: "Consolas"
            font.pointSize: 14
            color: root.ignoreKeyword ? "#4F000000" : "#000000"
            selectByMouse: true
            selectionColor: "lightblue"
            selectedTextColor: "black"
            onAccepted: root.sigAccepted()
            onTextEdited: root.sigUpdate(textInput.text, colorRect.color)
        }
    }

    // 删除标签
    Rectangle {
        id: delBtnRect
        width: 10
        height: width
        visible: false
        color: /*contentRect.color*/"transparent"
        radius: contentRect.radius
        anchors.left: contentRect.right
        anchors.leftMargin: -width / 3 * 2
        anchors.bottom: contentRect.top
        anchors.bottomMargin: -width / 3 * 2
        border.color: contentRect.border.color; border.width: /*contentRect.border.width*/0;

        MouseArea {
            anchors.fill: parent
            onPressed: root.sigRemove()

            Image {
                id: image
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: "qrc:/image/cancel.svg"
            }
        }
    }

    // 颜色选择对话框
    ColorDialog {
        id: colorDialog
        title: "请选择颜色"
        //showAlphaChannel: false  // 显示透明度调节滑块
        modality: Qt.WindowModal // 设置为模态对话框
        onAccepted: {
            //console.log("你选择了颜色: " + colorDialog.selectedColor);
            colorRect.color = colorDialog.selectedColor;
            root.sigUpdate(textInput.text, colorRect.color);
            root.sigRecolorful();
        }
        onRejected: {}
    }
}
