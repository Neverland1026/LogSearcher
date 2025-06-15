import QtQuick 2.15
import QtQuick.Controls 2.15

Window {
    id: root
    width: Screen.desktopAvailableWidth / 5 * 3
    height: Screen.desktopAvailableHeight / 4
    title: "Find Result"

    function setLineNumWidth(lineNumWidth) {
        logPanel.lineNumWidth = lineNumWidth;
    }

    signal sigPositionViewAtIndex(var lineNumber);

    signal sigClose();

    // 背景
    Rectangle { anchors.fill: parent; color: "white"; }

    // 搜索结果
    LogPanel { id: logPanel; anchors.fill: parent; logModel: $FindModel;
        onSigDoubleClicked: {
            sigPositionViewAtIndex(lineNumber);
        }
    }

    onClosing: { sigClose(); }
}
