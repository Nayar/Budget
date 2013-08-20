import bb.cascades 1.0
import bb.cascades.pickers 1.0

Container {
    id: root
    
    property int itemId;
    
    property alias name: nameLabel.text
    property alias amount: amountLabel.text
    property alias image: itemImage.imageSource
    property string itemType
    
    layout: StackLayout {}
    
    Divider {
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Center
        bottomMargin: 0
    }
    
    Container {
	    id: listItemContainer
		layout: StackLayout {
	        orientation: LayoutOrientation.LeftToRight
	    }
	    
        Container {
            id: pictureButtonContainer
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            layoutProperties: StackLayoutProperties {
                spaceQuota: 2 // This is so the other label will take up the rest of the space
            }
            
			ImageView {
		        id: itemImage
		        verticalAlignment: VerticalAlignment.Fill
		        horizontalAlignment: HorizontalAlignment.Fill
		        scalingMethod: root.image == "asset:///images/default.png" ? ScalingMethod.AspectFit : ScalingMethod.AspectFill
		        layoutProperties: StackLayoutProperties {
		            spaceQuota: 1 // This is so the other label will take up the rest of the space
		        }
			}
            
            gestureHandlers: [
                TapHandler {
                    onTapped: {
                        // Clear the selection first.
                        root.parent.clearSelection();

                        if (itemImage.imageSource == "asset:///images/default.png") {
                            // Because we cleared the selection already, the touch will propagate to the ListView's onTriggered function which will select the item for us.
                            filePicker.open()
                        } else {
                            // For some reason, touches aren't propagated to the ListView's onTriggered function when there's an image displaying. So we have to manually select it.
                            root.parent.select(root.ListItem.indexPath, true);
                            imageViewerSheet.open();
                        }
                    }
                }
            ]
            
            attachedObjects: [
                FilePicker {
                    id: filePicker
                    type : FileType.Picture
                    title : "Select Picture"
                    onFileSelected : {
                        // Beginning in 10.1, absolute paths must be prefixed with file://. Currently, file paths returned by
                        // FilePicker do not add this prefix, so it must be done manually.
                        if (root.itemType == "expense") {
                            Qt._budgetApp.updateExpense(name, amount.substring(1), "file://" + selectedFiles[0]);
                        } else if (root.itemType == "income") {
                            Qt._budgetApp.updateIncome(name, amount.substring(1), "file://" + selectedFiles[0]);
                        }
                        
                        // Make sure the list item is still selected (it gets cleared when updating an item in the list)
                        root.parent.select(root.ListItem.indexPath, true);
                    }
                }
            ]
		}
		
		Container {
		    id: labelsContainer
	        rightPadding: 20
	        leftPadding: rightPadding
	        topPadding: rightPadding
	        bottomPadding: rightPadding
	        layout: StackLayout {}
	        layoutProperties: StackLayoutProperties {
	            spaceQuota: 5 // This is so the other label will take up the rest of the space
	        }
	        
			Label {
			    id: nameLabel
			    textStyle {
			        base: SystemDefaults.TextStyles.PrimaryText
			    }
	          
			}
			Label {
			    id: amountLabel
			    textStyle {
			        base: SystemDefaults.TextStyles.SubtitleText
			    }
			}
		}
		
		Container {
		    id: deleteButtonContainer
	        horizontalAlignment: HorizontalAlignment.Center
	        verticalAlignment: VerticalAlignment.Center
	        layoutProperties: StackLayoutProperties {
	            spaceQuota: 1 // This is so the other label will take up the rest of the space
	        }
	        
		    ImageView {
		        id: deleteImage
		        imageSource: "asset:///images/delete.png"
		        horizontalAlignment: HorizontalAlignment.Fill
		        verticalAlignment: VerticalAlignment.Fill
		        scalingMethod: ScalingMethod.AspectFit
		    }
		    
		    gestureHandlers: [
		        TapHandler {
                    onTapped: {
	            	    if (itemType == "expense") {
                            Qt._budgetApp.deleteExpense(itemId, name, amount.substring(1), image, Qt.expensesMonthYearDropDowns.selectedMonth, Qt.expensesMonthYearDropDowns.selectedYear);
	                    } else if (itemType == "income") {
                            Qt._budgetApp.deleteIncome(itemId, name, amount.substring(1), image, Qt.incomesMonthYearDropDowns.selectedMonth, Qt.incomesMonthYearDropDowns.selectedYear);
	                    }
	            	}
	            }
		    ]
		}
	} // End of listItemContainer
    
    ListItem.onSelectionChanged: {
        // When they select an item in the list, we want to make the colour darker to show that it has
        // been selected. When they de-select it, it should return to the original colour.
        if (ListItem.selected) {
            if (itemType == "expense") {
                root.background = Color.create("#60FF0000")
            } else if (itemType == "income") {
                root.background = Color.create("#6000FF00")
            }
        } else {
            if (itemType == "expense") {
            	root.background = Color.create("#30FF0000")
            } else if (itemType == "income") {
                root.background = Color.create("#3000FF00")
            }
        }
    }
    
    attachedObjects: [
        Sheet {
            id: imageViewerSheet
            
            content: Page {
                id: imageViewerPage
                // We set it to visible once the page finishes loading. That way, the image can take up the entire space, and not be constrained.
                actionBarVisibility: ChromeVisibility.Hidden
                
                ImageView {
                    id: imageViewerImage
                    imageSource: itemImage.imageSource
                    scalingMethod: ScalingMethod.AspectFit
                    
                    gestureHandlers: [
                        TapHandler {
                            onTapped: {
                            	// Toggle visibility of the action bar
                            	if (imageViewerPage.actionBarVisibility == ChromeVisibility.Overlay) {
                            	    imageViewerPage.actionBarVisibility = ChromeVisibility.Hidden;
                            	} else {
                                    imageViewerPage.actionBarVisibility = ChromeVisibility.Overlay;
                            	}
                            }
                        }
                    ]
                } // End of ImageView
                
                actions: [
                    ActionItem {
                        title: "Done"
                        imageSource: "asset:///images/done.png"
                        ActionBar.placement: ActionBarPlacement.OnBar
                        onTriggered: {
                            imageViewerSheet.close();
                        }
                    },
                    ActionItem {
                        title: "Change"
                        imageSource: "asset:///images/default.png"
                        ActionBar.placement: ActionBarPlacement.OnBar
                        
                        onTriggered: {
                            filePicker.open();
                        }
                    },
                    ActionItem {
                        title: "Delete"
                        imageSource: "asset:///images/delete.png"
                        ActionBar.placement: ActionBarPlacement.OnBar
                        
                        onTriggered: {
                            imageViewerSheet.close();
                            if (root.itemType == "expense") {
                                Qt._budgetApp.updateExpense(name, amount.substring(1), "asset:///images/default.png");
                            } else if (root.itemType == "income") {
                                Qt._budgetApp.updateIncome(name, amount.substring(1), "asset:///images/default.png");
                            }
                        }
                    }
                ] // End of actions
                
                onCreationCompleted: {
                    actionBarVisibility = ChromeVisibility.Overlay
                }
            } // End of Page
        } // End of Sheet
    ] // End of attachedObjects
} // End of root container
