import bb.cascades 1.0

Container {
    property alias text: label.text
    property alias amount: amountLabel.text
    
    layout: StackLayout {}
    horizontalAlignment: HorizontalAlignment.Fill
    topPadding: 20
    bottomPadding: 20
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
