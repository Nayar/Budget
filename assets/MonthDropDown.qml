import bb.cascades 1.0

Container {
    property alias title: monthDropDown.title
    property alias selectedMonth: monthDropDown.selectedValue
    property alias month: monthDropDown
    signal monthChanged(int selectedValue)
    
    DropDown {
        id: monthDropDown
        objectName: "monthDropDown"
        enabled: true
        
        Option {
            text: "January"
            value: "1"
        }
        Option {
            text: "February"
            value: "2"
        }
        Option {
            text: "March"
            value: "3"
        }
        Option {
            text: "April"
            value: "4"
        }
        Option {
            text: "May"
            value: "5"
        }
        Option {
            text: "June"
            value: "6"
        }
        Option {
            text: "July"
            value: "7"
        }
        Option {
            text: "August"
            value: "8"
        }
        Option {
            text: "September"
            value: "9"
        }
        Option {
            text: "October"
            value: "10"
        }
        Option {
            text: "November"
            value: "11"
        }
        Option {
            text: "December"
            value: "12"
        }
        
        onCreationCompleted: {
            monthDropDown.setSelectedIndex(new Date().getMonth());
        }
        
        onSelectedValueChanged: {
            monthChanged(selectedValue);
        }
    }
}
