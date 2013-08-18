import bb.cascades 1.0

Container {
    // The BudgetListPage and BudgetListItem components use these values
    property alias selectedMonth: monthDropDown.selectedMonth
    property int selectedMonthIndex: monthDropDown.month.selectedIndex
    property alias selectedYear: yearDropDown.selectedValue
    property alias selectedYearIndex: yearDropDown.selectedIndex
    property variant data;
    
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
    	MonthDropDown {
            id: monthDropDown
            title: "Month"
            
            onMonthChanged: {
                // Make sure the edit input fields are cleared when they switch months/years.
                budgetList.clearSelection();
                
                // Default to current year if the year dropdown hasn't loaded yet
                var year = new Date().getFullYear();
                if (yearDropDown.selectedValue) {
                    year = yearDropDown.selectedValue;
                }
                
                // We want the "repeat until" DropDown to change with the month DropDown
                monthRange.month.setSelectedIndex(monthDropDown.month.selectedIndex);
                
	            // Clear the ListView and load the new entries based on their selected month/year
	            data.clear();
            
	            if (data.objectName == "expensesModel") {
	                data.insertList(_budgetApp.getExpenses(selectedValue, year));
	            } else if (data.objectName == "incomesModel") {
	                data.insertList(_budgetApp.getIncomes(selectedValue, year));
	            }
            }
        }
        
        // The year DropDown is populated in the C++ code
        DropDown {
            id: yearDropDown
            objectName: "yearDropDown"
            title: "Year"
            enabled: true
            
            onSelectedValueChanged: {
                // Make sure the edit input fields are cleared when they switch months/years.
                budgetList.clearSelection();
                
                // Default to current month if the month dropdown hasn't loaded yet
                var month = new Date().getMonth();
                if (monthDropDown.selectedMonth) {
                    month = monthDropDown.selectedMonth;
                }
                
                // We want the "repeat until" DropDown to change with the year DropDown
                yearRange.setSelectedIndex(yearDropDown.selectedIndex);
                
                // Clear the ListView and load the new entries based on their selected month/year
                data.clear();
                
                if (data.objectName == "expensesModel") {
                    data.insertList(_budgetApp.getExpenses(month, selectedValue));
                } else if (data.objectName == "incomesModel") {
                    data.insertList(_budgetApp.getIncomes(month, selectedValue));
                }
            }
        }
    }
}
