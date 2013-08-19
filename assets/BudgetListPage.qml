import bb.cascades 1.0

Page {
    // This determines whether we need to render an expense page or an income page
    property string type
    
    Container {
        layout: StackLayout {}
        
        onCreationCompleted: {
            // This is so the BudgetListItem components can access the currently selected month/year for deleting from the ListView
            if (type == "expense") {
                Qt.expensesMonthYearDropDowns = monthYearDropDowns;
            } else if (type == "income") {
                Qt.incomesMonthYearDropDowns = monthYearDropDowns;
            }
        }
        
        MonthYearFilter {
            id: monthYearDropDowns
            objectName: type == "expense" ? "expensesMonthYearDropDowns" : "incomesMonthYearDropDowns"
            data: budgetList.dataModel
        }
        
        ListView {
            id: budgetList
            objectName: type == "expense" ? "expensesList" : "incomesList" // Used to reference the object in C++
            dataModel: budgetDataModel
            
            listItemComponents: [
                ListItemComponent {
                    type: "item"
                    BudgetListItem {
                        id: itemRoot
                        // To access the type property outside the ListItemComponent, we need to get a handle
                        // to the list item and then back to the view.
                        itemType: itemRoot.ListItem.view.parent.parent.type
                        itemId: ListItemData.id
	                    name: ListItemData.name
	                    amount: "$" + ListItemData.amount
                        image: ListItemData.imagepath
                        background: itemType == "expense" ? Color.create("#30FF0000") : Color.create("#3000FF00")
                    }
                }
            ]
            
            onTriggered: {
                toggleSelection(indexPath);

                // If they selected another item, clear all selections and re-select the last one they selected
                if (selectionList().length > 1) {
                    clearSelection();
                    toggleSelection(indexPath);
                }

            }
            
            onSelectionChanged: {
                if (selected) {
                    var chosenItem = dataModel.data(indexPath);
                    nameAdd.text = chosenItem.name;
                    amountAdd.text = chosenItem.amount;
                    addContainer.image = chosenItem.imagepath;
                    editButton.opacity = 1;
                    monthRange.month.enabled = false;
                    yearRange.enabled = false;
                } else {
                    nameAdd.text = "";
                    amountAdd.text = "";
                    addContainer.image = "";
                    editButton.opacity = 0;
                    monthRange.month.enabled = true;
                    yearRange.enabled = true;
                }
            }
            
            attachedObjects: [
                GroupDataModel {
                    id: budgetDataModel
                    objectName: type == "expense" ? "expensesModel" : "incomesModel" // Used to reference the object in C++
                    grouping: ItemGrouping.ByFirstChar
                    sortingKeys: ["name", "amount"]
                    
                    onItemAdded: {
                        // This is handled in the addButton's onClick method instead, so that it takes into account adding to multiple months.
                        //updateOverviewPage();
                    }
                    
                    onItemRemoved: {
                        updateOverviewPage();
                    }
                    
                    onItemUpdated: {
                        updateOverviewPage();
                    }
                }
            ]
        } // End of ListView
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
	        Container {
                layout: StackLayout {}
                leftPadding: 20
                rightPadding: 20
                topPadding: 25
                bottomPadding: 25
	            
	            Container {
	                id: addContainer
	                property string image
	                layout: StackLayout {
	                    orientation: LayoutOrientation.LeftToRight
	                }
	                TextField {
	                    id: nameAdd
	                    hintText: "Name"
	                }
	                TextField {
	                    id: amountAdd
	                    hintText: "Amount"
	                    inputMode: TextFieldInputMode.NumbersAndPunctuation
	                }
	                
	            } // End of text field container
	            
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    topMargin: 10
                    
                    Label {
                        text: "Repeat Until:"
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1
                        }
                        multiline: true
                        horizontalAlignment: HorizontalAlignment.Center
                        verticalAlignment: VerticalAlignment.Center
                        textStyle {
                            base: SystemDefaults.TextStyles.BodyText
                            textAlign: TextAlign.Center
                        }
                    
                    }
                    
                    MonthDropDown {
                        id: monthRange
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 2
                        }
                    }
                    
                    DropDown {
                        id: yearRange
                        objectName: "yearRange"
                        layoutProperties: StackLayoutProperties {
                            spaceQuota: 1
                        }
                    }
                } // End of Month/Year DropDown container
	        } // End of input fields container
            
            Container {
                id: buttonContainer
                layout: AbsoluteLayout {}
                verticalAlignment: VerticalAlignment.Center
                
                Button {
                    id: addButton
                    imageSource: "asset:///images/add.png"
                    preferredWidth: 0;
                    onClicked: {
                        if (type == "expense") {
                            _budgetApp.addExpense(nameAdd.text, amountAdd.text, "asset:///images/default.png", Qt.expensesMonthYearDropDowns.selectedMonth, Qt.expensesMonthYearDropDowns.selectedYear, monthRange.selectedMonth, yearRange.selectedValue) 
                        } else if (type == "income") {
                            _budgetApp.addIncome(nameAdd.text, amountAdd.text, "asset:///images/default.png", Qt.incomesMonthYearDropDowns.selectedMonth, Qt.incomesMonthYearDropDowns.selectedYear, monthRange.selectedMonth, yearRange.selectedValue)
                        }

                        updateOverviewPage();
                    }
                }
                
                Button {
                    id: editButton
                    opacity: 0
                    imageSource: "asset:///images/done.png"
                    preferredWidth: 0;
                    onClicked: {
                        // Update the expense in the database and ListView
                        if (type == "expense") {
                            _budgetApp.updateExpense(nameAdd.text, amountAdd.text, addContainer.image)
                        } else if (type == "income") {
                            _budgetApp.updateIncome(nameAdd.text, amountAdd.text, addContainer.image)
                        }
                    }
                }
            } // End of buttonContainer
        } // End of add container
    } // End of outer container
    
    // This function runs whenever an item in the list is added/updated/deleted.
    function updateOverviewPage() {
        var currentMonth = new Date().getMonth();
        var currentYear = new Date().getFullYear();
        
        if (type == "expense") {
            overviewPage.currentMonthExpenses = _budgetApp.getMonthExpenseAmount(currentMonth, currentYear);
            overviewPage.currentYearExpenses = _budgetApp.getYearExpenseAmount(currentYear);
        } else if (type == "income") {
            overviewPage.currentMonthIncomes = _budgetApp.getMonthIncomeAmount(currentMonth, currentYear);
            overviewPage.currentYearIncomes = _budgetApp.getYearIncomeAmount(currentYear);
        }
        
        nameAdd.text = "";
        amountAdd.text = "";
        addContainer.image = "";
        
        budgetList.clearSelection();
        editButton.opacity = 0;
        
        monthRange.month.setSelectedIndex(monthYearDropDowns.selectedMonthIndex);
        yearRange.setSelectedIndex(monthYearDropDowns.selectedYearIndex);
    }
} // End of Page
