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

    property int globalX: -1
    property int globalY: -1

    property int realtimeRemainMemory: -1

    ChartView {
        id: chartView
        anchors.fill: parent
        antialiasing: true
        animationOptions: ChartView.NoAnimation
        theme: ChartView.ChartThemeLight

        BarCategoryAxis {
            id: axisX
            labelsAngle: -90
            labelsFont {
                //family: "Arial"
                pixelSize: 14
                //bold: true
                italic: false
            }
            gridVisible: false  // 关键设置
            minorGridVisible: false
            lineVisible: true   // 保持轴线可见
            titleText: "Time Point"
        }

        ValueAxis {
            id: axisY
            min: 0
            labelFormat: "%d"
            titleText: "Memory"
        }

        LineSeries {
            id: lineSeries
            name: "Remain Memory"
            color: $LogSearcher.majorLogoColor
            capStyle: Qt.SquareCap
            width: 2
            axisX: axisX
            axisY: axisY
            pointsVisible: true
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            onPositionChanged: (mouse) => {
                                   var point1 = chartView.mapToValue(Qt.point(mouse.x, mouse.y), lineSeries)
                                   root.realtimeRemainMemory = parseInt(point1.y);

                                   root.globalX = mouse.x;
                                   root.globalY = mouse.y;
                                   canvas.requestPaint();
                               }
        }
    }

    Text {
        id: text
        visible: false
        anchors.centerIn: parent
        font.family: "Consolas"
        font.pixelSize: 30
        text: root.realtimeRemainMemory
    }

    // 动态十字线
    Canvas {
        id: canvas
        onVisibleChanged: if(visible) requestPaint()
        anchors.fill: parent

        onPaint: {
            if(visible) {
                var ctx = getContext("2d");
                draw(ctx);
            }
        }

        function draw(ctx) {
            ctx.clearRect(0, 0, root.width, root.height);

            ctx.strokeStyle = $LogSearcher.majorLogoColor;
            ctx.lineWidth = 1;

            // 绘制竖向线
            ctx.beginPath();
            ctx.moveTo(root.globalX, 0);
            ctx.lineTo(root.globalX, root.height);
            ctx.stroke();

            // 绘制横向线
            ctx.beginPath();
            ctx.moveTo(0, root.globalY);
            ctx.lineTo(root.width, root.globalY);
            ctx.stroke();

            // 图像坐标
            ctx.font = 'bold 25px sans-serif';
            ctx.fillStyle = $LogSearcher.majorLogoColor;
            var bias = root.globalX > (parent.width - 350) ? -350 : 0;
            context.fillText("Remain memory: " + Math.max(0, root.realtimeRemainMemory) + "Mb",
                             root.globalX + 10 + bias,
                             root.globalY + 30);
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

    Component.onCompleted: updateChart()
}
