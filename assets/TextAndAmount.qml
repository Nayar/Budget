import bb.cascades 1.0

Container {
    property alias text: label.text
    property alias amount: amountLabel.text
    
    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
    horizontalAlignment: HorizontalAlignment.Fill
    topPadding: 20
    bottomPadding: topPadding
    rightPadding: topPadding
    leftPadding: topPadding
    Label {
        id: label
        textStyle { base: SystemDefaults.TextStyles.PrimaryText }
        layoutProperties: StackLayoutProperties {
            spaceQuota: 1 // This is so the other label will take up the rest of the space
        }
    }
    Label {
        id: amountLabel
        text: overviewContainer.totalExpenses
        textStyle { base: SystemDefaults.TextStyles.BodyText }
        horizontalAlignment: HorizontalAlignment.Right
    }
}
