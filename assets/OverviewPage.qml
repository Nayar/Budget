import bb.cascades 1.0

Page {
    property alias currentMonthExpenses: curMonthExpenses.amount
    property alias currentMonthIncomes: curMonthIncomes.amount
    
    property alias currentYearExpenses: curYearExpenses.amount
    property alias currentYearIncomes: curYearIncomes.amount
    
    property int currentYear: new Date().getFullYear()
    
    property variant monthNames
    property int currentMonth: new Date().getMonth()
    
	Container {
        id: overviewContainer
        
        layout: StackLayout {}
    
        TextAndAmount {
            id: curMonthExpenses
            background: Color.create("#30FF0000")
            text: {
                text = monthNames[currentMonth] + " Expenses";
            }
            amount: currentMonthExpenses
        }
        
        TextAndAmount {
            id: curMonthIncomes
            background: Color.create("#3000FF00")
            text: {
                text = monthNames[currentMonth] + " Incomes";
            }
            amount: currentMonthIncomes
        }
        
        TextAndAmount {
            id: adjustedIncome
            text: monthNames[currentMonth] + " Adjusted Income"
            amount: currentMonthIncomes - currentMonthExpenses
        }
        
        Divider {
            bottomMargin: 0
            topMargin: 0
        }
        
        TextAndAmount {
            id: curYearExpenses
            background: Color.create("#30FF0000")
            text: currentYear + " Total Expenses"
            amount: currentYearExpenses
        }
        
        TextAndAmount {
            id: curYearIncomes
            background: Color.create("#3000FF00")
            text: currentYear + " Total Incomes"
            amount: currentYearIncomes
        }
        
        TextAndAmount {
            id: adjustedYearIncome
            text: currentYear + " Total Adjusted Income"
            amount: currentYearIncomes - currentYearExpenses
        }
        
        onCreationCompleted: {
            monthNames = [ "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December" ];
        }
	}
}
