import bb.cascades 1.0

Page {
    property alias monthExpenses: monthExpensesLabels.amount
    property alias monthIncomes: monthIncomesLabels.amount
    property alias adjustedMonthIncome: adjustedMonthIncomeLabels.amount
    
    property alias yearExpenses: yearExpensesLabel.amount
    property alias yearIncomes: yearIncomesLabel.amount
    property alias adjustedYearIncome: adjustedYearIncomeLabels.amount
    
    property int currentYear: new Date().getFullYear()
    
    property variant monthNames
    property int currentMonth: new Date().getMonth()
    
    property alias selectedMonth: monthDropDown.selectedMonth
    property alias selectedMonthYear: monthYearDropDown.selectedValue

    property alias selectedYear: yearDropDown.selectedValue

	Container {
        id: overviewContainer
        objectName: "overviewContainer"
        
        layout: StackLayout {}
    
    	Container {
	        id: monthFilterContainer	
	        layout: StackLayout {
            	orientation: LayoutOrientation.LeftToRight
            }
	        
	    	MonthDropDown {
	    	    id: monthDropDown
	    	    objectName: "monthDropDown"
	    	    title: "Month"
	    	    
	            onMonthChanged: {
	                monthExpenses = "$" + _budgetApp.getMonthExpenseAmount(monthDropDown.selectedMonth, monthYearDropDown.selectedValue);
	                monthIncomes = "$" + _budgetApp.getMonthIncomeAmount(monthDropDown.selectedMonth, monthYearDropDown.selectedValue);
                    adjustedMonthIncome = ("$" + (overviewPage.monthIncomes.substring(1) - overviewPage.monthExpenses.substring(1))).replace("$-", "-$");
	            }
	    	}
	    	
	    	DropDown {
	    	    id: monthYearDropDown
	    	    objectName: "monthYearDropDown"
	    	    title: "Year"
	    	    
	            onSelectedValueChanged: {
	                monthExpenses = "$" + _budgetApp.getMonthExpenseAmount(monthDropDown.selectedMonth, monthYearDropDown.selectedValue);
	                monthIncomes = "$" + _budgetApp.getMonthIncomeAmount(monthDropDown.selectedMonth, monthYearDropDown.selectedValue);
                    adjustedMonthIncome = ("$" + (overviewPage.monthIncomes.substring(1) - overviewPage.monthExpenses.substring(1))).replace("$-", "-$");
	            }
	    	}
	    }
    	
        TextAndAmount {
            id: monthExpensesLabels
            background: Color.create("#30FF0000")
            text: monthNames[monthDropDown.selectedMonth - 1] + " " + monthYearDropDown.selectedValue + " Expenses"
        }
        
        TextAndAmount {
            id: monthIncomesLabels
            background: Color.create("#3000FF00")
            text: monthNames[monthDropDown.selectedMonth - 1] + " " + monthYearDropDown.selectedValue + " Incomes"
        }
        
        TextAndAmount {
            id: adjustedMonthIncomeLabels
            text: monthNames[monthDropDown.selectedMonth - 1] + " " + monthYearDropDown.selectedValue + " Adjusted Income"
        }
        
        DropDown {
            id: yearDropDown
            objectName: "yearDropDown"
            title: "Year"
            
            onSelectedValueChanged: {
                yearExpenses = "$" + _budgetApp.getYearExpenseAmount(yearDropDown.selectedValue);
                yearIncomes = "$" + _budgetApp.getYearIncomeAmount(yearDropDown.selectedValue);
                adjustedYearIncome = ("$" + (overviewPage.yearIncomes.substring(1) - overviewPage.yearExpenses.substring(1))).replace("$-", "-$");
            }
        }
        
        TextAndAmount {
            id: yearExpensesLabel
            background: Color.create("#30FF0000")
            text: yearDropDown.selectedValue + " Total Expenses"
        }
        
        TextAndAmount {
            id: yearIncomesLabel
            background: Color.create("#3000FF00")
            text: yearDropDown.selectedValue + " Total Incomes"
        }
        
        TextAndAmount {
            id: adjustedYearIncomeLabels
            text: yearDropDown.selectedValue + " Total Adjusted Income"
        }
        
        onCreationCompleted: {
            monthNames = [ "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December" ];
        }
	}
}
