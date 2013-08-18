import bb.cascades 1.0

TabbedPane {
    id: tabbedPane
    showTabsOnActionBar: true
    property int currentMonth: new Date().getMonth()
    property int currentYear: new Date().getFullYear()
    
    onCreationCompleted: {
        Qt._budgetApp = _budgetApp;
    }
    
    Tab {
        title: "Overview"
        id: overviewTab
        OverviewPage {
            id: overviewPage
            currentMonthExpenses: _budgetApp.getMonthExpenseAmount(currentMonth, currentYear)
            currentMonthIncomes: _budgetApp.getMonthIncomeAmount(currentMonth, currentYear)
            currentYearExpenses: _budgetApp.getYearExpenseAmount(currentYear)
            currentYearIncomes: _budgetApp.getYearIncomeAmount(currentYear)
        }
    }
    
    Tab {
        title: "Monthly Expenses"
		BudgetListPage {
            id: expensesPage
            objectName: "expensesPage"
            type: "expense"
        }
    }
    
    Tab {
        title: "Monthly Income"
		BudgetListPage {
		    id: incomesPage
		    objectName: "incomesPage"
		    type: "income"
      	}
    }
}