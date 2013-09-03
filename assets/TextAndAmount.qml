import bb.cascades 1.0

Container {
    property alias text: label.text
    property alias amount: amountLabel.text
    
    layout: StackLayout {}
    horizontalAlignment: HorizontalAlignment.Fill
    topPadding: 10
    bottomPadding: 10
    rightPadding: 20
    leftPadding: 20

    Label {
        id: label
    }

    Label {
        id: amountLabel
        textStyle { base: SystemDefaults.TextStyles.SubtitleText }
        horizontalAlignment: HorizontalAlignment.Left
    }
}
