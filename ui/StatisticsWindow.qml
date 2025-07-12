import QtQuick 2.15
import QtCharts 2.15
import QtQuick.Controls 2.15
import CustomModels 1.0

// FindResultWindow
Window {
    id: root
    width: Screen.width / 7 * 5
    height: Screen.height / 7 * 5
    title: "Statistics"

    modality: Qt.NonModal

    ChartView {
        id: chartView
        anchors.fill: parent
        antialiasing: true
        animationOptions: ChartView.NoAnimation
        theme: ChartView.ChartThemeLight

        // X轴（类别轴）
        BarCategoryAxis {
            id: axisX
            labelsAngle: -90
            labelsFont {
                //family: "Arial"
                pixelSize: 14
                //bold: true
                italic: false
            }
            //labelsVisible: false
            titleText: "Time Point"
        }

        // Y轴（数值轴）
        ValueAxis {
            id: axisY
            min: 0
            labelFormat: "%d"
            titleText: "Memory"
        }

        // 折线系列
        LineSeries {
            id: lineSeries
            name: "Remain Memory"
            color: $LogSearcher.majorLogoColor
            capStyle: Qt.SquareCap
            width: 2
            axisX: axisX
            axisY: axisY
            pointsVisible: true

            //pointLabelsFormat: "@yPoint"
            //pointLabelsVisible: true

            onHovered: (point) => {
                           console.log(axisX.categories[Math.round(point.x)], point.y);

                           tooltip.show(chartView.mapToPosition(point).x,
                                        chartView.mapToPosition(point).y,
                                        axisX.categories[Math.round(point.x)],
                                        point.y
                                        );
                       }
        }
    }

    // 连接数据模型
    Connections {
        target: $ChartDataModel
        function onDataChanged() {
            updateChart();
        }
    }

    function updateChart() {
        // 清空现有数据
        lineSeries.clear();
        var categories = [];

        // 填充新数据
        console.log("$ChartDataModel =", $ChartDataModel.count)
        console.log("$ChartDataModel.count =", $ChartDataModel.count)
        for (var i = 0; i < $ChartDataModel.count; ++i) {
            var item = $ChartDataModel.get(i);
            lineSeries.append(i, item.value);
            categories.push(item.label);
        }

        // 更新X轴类别
        axisX.categories = categories;

        // 自动调整Y轴范围
        if ($ChartDataModel.count > 0) {
            var maxValue = 0;
            for (i = 0; i < $ChartDataModel.count; ++i) {
                maxValue = Math.max(maxValue, $ChartDataModel.get(i).value);
            }
            axisY.max = maxValue * 1.2; // 留20%余量
        }
    }

    Rectangle {
        id: tooltip
        visible: false
        width: 130
        height: 40
        color: "#f0f0e0"
        border.color: "gray"
        radius: 5
        z: 10

        property alias text: tooltipText.text

        function show(xPos, yPos, label, value) {
            x = xPos - width / 2;
            y = yPos - height - 10;
            text = `<b>Time Point: ${label}<br>Memory: ${value.toFixed(0)} MB</b>`;
            visible = true;
        }

        function hide() { visible = false; }

        Text {
            id: tooltipText
            anchors.centerIn: parent
            textFormat: Text.RichText
            font.pixelSize: 12
        }
    }

    Component.onCompleted: updateChart()
}
