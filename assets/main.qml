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
        imageSource: "asset:///images/home.png";

        OverviewPage {
            id: overviewPage
        }
    }
    
    Tab {
        title: "Monthly Expenses"
        imageSource: "asset:///images/notes.png"

		BudgetListPage {
            id: expensesPage
            objectName: "expensesPage"
            type: "expense"
        }
    }
    
    Tab {
        title: "Monthly Incomes"
        imageSource: "asset:///images/notes.png"

		BudgetListPage {
		    id: incomesPage
		    objectName: "incomesPage"
		    type: "income"
      	}
    }
}