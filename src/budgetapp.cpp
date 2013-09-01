#include "budgetapp.hpp"

#include "BudgetDbHelper.hpp"

#include <bb/cascades/GroupDataModel>
#include <bb/cascades/ListView>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Page>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
#include <bb/cascades/Label>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/system/SystemDialog>
#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/DropDown>
#include <bb/cascades/Container>
#include <bb/cascades/Option>
#include <QDate>

using namespace bb::cascades;
using namespace bb::system;

BudgetApp::BudgetApp()
{

}

BudgetApp::BudgetApp(bb::cascades::Application *app) : QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // initial load
    onSystemLanguageChanged();
}

void BudgetApp::onStart()
{
	// Instantiate the database helper object
	mBudgetDbHelper = new BudgetDbHelper("budget.db");

	if (!loadQMLScene()) {
		qWarning("Failed to load QML scene.");
	}
}

bool BudgetApp::loadQMLScene()
{
	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	if (!qml->hasErrors()) {
		qml->setContextProperty("_budgetApp", this);

		// Register the file picker so we can use it in QML
		qmlRegisterType<pickers::FilePicker>("bb.cascades.pickers", 1, 0, "FilePicker");

		// Create root object for the UI
		AbstractPane *root = qml->createRootObject<AbstractPane>();

		if (root) {
			mExpensesListView = root->findChild<ListView*>("expensesList");
			mExpensesDataModel = root->findChild<GroupDataModel*>("expensesModel");

			mIncomesListView = root->findChild<ListView*>("incomesList");
			mIncomesDataModel = root->findChild<GroupDataModel*>("incomesModel");

			populateYearDropDowns(root);

			// Display the TabbedPane
			Application::instance()->setScene(root);

			return true;
		}
	}

	return false;
}

void BudgetApp::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Budget_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

bool BudgetApp::addExpense(const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year, const QString &toMonth, const QString &toYear)
{
	// Check they input a correct number
	if (!isANumber(amount)) {
		return false;
	}

	// Make sure they selected a valid "repeat until" date
	if (!comesBefore(month, year, toMonth, toYear)) {
		return false;
	}

	QDate fromDate = QDate(year.toInt(), month.toInt(), 1);
	QDate toDate = QDate(toYear.toInt(), toMonth.toInt(), 1);
	// We want to add an entry for every month <= to toDate so loop until fromDate == toDate.addMonths(1)
	while (fromDate != toDate.addMonths(1)) {
		QVariantMap map;
		map["name"] = QString(name);
		map["amount"] = QString(amount);
		map["imagepath"] = QString(imagePath);

		QString newMonth = QString::number(fromDate.month());
		QString newYear = QString::number(fromDate.year());
		// Get the associated month entry
		int monthId = mBudgetDbHelper->getMonthId(newMonth, newYear);

		// If the month doesn't exist in the DB yet, add it and then add the new expense
		if (monthId == 0) {
			monthId = addMonth(newMonth, newYear);
			map["month"] = QString::number(monthId);
		} else {
			// If it already exists in the DB, just add the new expense using the month id
			map["month"] = QString::number(monthId);
		}

		QVariant insertId = mBudgetDbHelper->insertExpense(map);
		if (insertId.isNull()) {
			return false;
		}

		map["id"] = insertId;
		// We only want to insert it into the data model the first time; it only needs to show up once in the ListView for the
		// currently selected month/year
		if (newMonth == month && newYear == year) {
			mExpensesDataModel->insert(map);
		}

		fromDate = fromDate.addMonths(1);
	}

	return true;
}

/*
 * Updates the currently selected expense item in the ListView. If they haven't selected anything
 * then this function does nothing. Since they are already viewing the item for the selected month/year
 * values, we can assume that this will never change so don't have to update it.
 */
bool BudgetApp::updateExpense(const QString &name, const QString &amount, const QString &imagePath)
{
	// Check they input a correct number
	if (!isANumber(amount)) {
		return false;
	}

    QVariantList indexPath = mExpensesListView->selected();

    // Only update if they selected an item in the ListView
    if (!indexPath.isEmpty()) {
        QVariantMap itemMapAtIndex = mExpensesDataModel->data(indexPath).toMap();

        itemMapAtIndex["name"] = QString(name);
        itemMapAtIndex["amount"] = QString(amount);
        itemMapAtIndex["imagepath"] = QString(imagePath);

        mBudgetDbHelper->updateExpense(itemMapAtIndex);
        mExpensesDataModel->updateItem(indexPath, itemMapAtIndex);
    }

    return true;
}

/**
 * Delete an expense from the database and the ListView. This function requires all parameters of the Data Model because
 * we have to be able to find the exact item in the list, even if someone hasn't clicked on one.
 */
void BudgetApp::deleteExpense(const QString &itemId, const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year)
{
	QVariantMap itemMap;

	// Construct the entire map manually since we don't want to use the one that is currently selected in the list
	itemMap["id"] = QString(itemId);
	itemMap["name"] = QString(name);
	itemMap["amount"] = QString(amount);
	itemMap["imagepath"] = QString(imagePath);
	itemMap["month"] = mBudgetDbHelper->getMonthId(month, year);

	if (mBudgetDbHelper->deleteExpenseById(itemMap["id"])) {
		mExpensesDataModel->remove(itemMap);
	}

	mBudgetDbHelper->deleteUnusedMonth(itemMap["month"]);
}

bool BudgetApp::addIncome(const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year, const QString &toMonth, const QString &toYear)
{
	// Check they input a correct number
	if (!isANumber(amount)) {
		return false;
	}

	// Make sure they selected a valid "repeat until" date
	if (!comesBefore(month, year, toMonth, toYear)) {
		return false;
	}

	QDate fromDate = QDate(year.toInt(), month.toInt(), 1);
	QDate toDate = QDate(toYear.toInt(), toMonth.toInt(), 1);
	// We want to add an entry for every month <= to toDate so loop until fromDate == toDate.addMonths(1)
	while (fromDate != toDate.addMonths(1)) {
		QVariantMap map;
		map["name"] = QString(name);
		map["amount"] = QString(amount);
		map["imagepath"] = QString(imagePath);

		QString newMonth = QString::number(fromDate.month());
		QString newYear = QString::number(fromDate.year());
		// Get the associated month entry
		int monthId = mBudgetDbHelper->getMonthId(newMonth, newYear);

		// If the month doesn't exist in the DB yet, add it and then add the new expense
		if (monthId == 0) {
			monthId = addMonth(newMonth, newYear);
			map["month"] = QString::number(monthId);
		} else {
			// If it already exists in the DB, just add the new expense using the month id
			map["month"] = QString::number(monthId);
		}

		QVariant insertId = mBudgetDbHelper->insertIncome(map);
		if (insertId.isNull()) {
			return false;
		}

		map["id"] = insertId;
		if (newMonth == month && newYear == year) {
			mIncomesDataModel->insert(map);
		}

		fromDate = fromDate.addMonths(1);
	}
	return true;
}

/*
 * Updates the currently selected income item in the ListView. If they haven't selected anything
 * then this function does nothing. Since they are already viewing the item for the selected month/year
 * values, we can assume that this will never change so don't have to update it.
 */
bool BudgetApp::updateIncome(const QString &name, const QString &amount, const QString &imagePath)
{
	// Check they input a correct number
	if (!isANumber(amount)) {
		return false;
	}

    QVariantList indexPath = mIncomesListView->selected();

    if (!indexPath.isEmpty()) {
        QVariantMap itemMapAtIndex = mIncomesDataModel->data(indexPath).toMap();

        itemMapAtIndex["name"] = QString(name);
        itemMapAtIndex["amount"] = QString(amount);
        itemMapAtIndex["imagepath"] = QString(imagePath);

        mBudgetDbHelper->updateIncome(itemMapAtIndex);
        mIncomesDataModel->updateItem(indexPath, itemMapAtIndex);
    }

    return true;
}

/**
 * Delete an income from the database and the ListView. This function requires all parameters of the Data Model because
 * we have to be able to find the exact item in the list, even if someone hasn't clicked on one.
 */
void BudgetApp::deleteIncome(const QString &itemId, const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year)
{
	QVariantMap itemMap;

	// Construct the map manually since we don't want to use the one that is currently selected in the list
	itemMap["id"] = QString(itemId);
	itemMap["name"] = QString(name);
	itemMap["amount"] = QString(amount);
	itemMap["imagepath"] = QString(imagePath);
	itemMap["month"] = mBudgetDbHelper->getMonthId(month, year);

	if (mBudgetDbHelper->deleteIncomeById(itemMap["id"])) {
		mIncomesDataModel->remove(itemMap);
	}

	mBudgetDbHelper->deleteUnusedMonth(itemMap["month"]);
}

double BudgetApp::getMonthExpenseAmount(int month, int year)
{
	return mBudgetDbHelper->getMonthExpenseAmount(month, year);
}

double BudgetApp::getYearExpenseAmount(int year)
{
	return mBudgetDbHelper->getYearExpenseAmount(year);
}

double BudgetApp::getTotalMonthlyExpenseAmount()
{
	QString sqlQuery = "SELECT SUM(amount) AS sum FROM expense";
	QSqlQuery query = mBudgetDbHelper->queryDatabaseWithResult(sqlQuery);

	// Get the field's index
	const int sumField = query.record().indexOf("sum");

	double total = 0;
	while (query.next()) {
		// Access the data via the field's index
		total += query.value(sumField).toDouble();
	}

	return total;
}

double BudgetApp::getMonthIncomeAmount(const int &month, const int &year)
{
	return mBudgetDbHelper->getMonthIncomeAmount(month, year);
}

double BudgetApp::getYearIncomeAmount(const int &year)
{
	return mBudgetDbHelper->getYearIncomeAmount(year);
}

double BudgetApp::getTotalMonthlyIncomeAmount()
{
	QString sqlQuery = "SELECT SUM(amount) AS sum FROM income";
	QSqlQuery query = mBudgetDbHelper->queryDatabaseWithResult(sqlQuery);

	// Get the field's index
	const int sumField = query.record().indexOf("sum");

	double total = 0;
	while (query.next()) {
		// Access the data via the field's index
		total += query.value(sumField).toDouble();
	}

	return total;
}

QVariantList BudgetApp::getExpenses(const QString &month, const QString &year)
{
	return mBudgetDbHelper->getExpenses(month, year);
}

QVariantList BudgetApp::getIncomes(const QString &month, const QString &year)
{
	return mBudgetDbHelper->getIncomes(month, year);
}

int BudgetApp::addMonth(const QString &month, const QString &year)
{
	QVariantMap monthMap;
	monthMap["month"] = QString(month);
	monthMap["year"] = QString(year);
	return mBudgetDbHelper->insertMonth(monthMap).toInt();
}

/*
 * Queries the expense table to find out the earliest year for which an expense has been entered.
 * This value is used in determining the years to display in the DropDown menu. If there are no
 * entries, then return the current year.
 */
int BudgetApp::getEarliestExpenseYear()
{
	QString query = 	"SELECT MIN(month.year) AS minyear\n"
						"FROM expense JOIN month\n"
						"WHERE expense.month = month.id";

	QSqlQuery sqlQuery = mBudgetDbHelper->queryDatabaseWithResult(query);

	// Default to the current year.
	int year = QDate::currentDate().year();

	while (sqlQuery.next()) {
		year = sqlQuery.value(0).toInt();
	}

	return year;
}

/*
 * Queries the income table to find out the earliest year for which an income has been entered.
 * This value is used in determining the years to display in the DropDown menu. If there are no
 * entries, then return the current year.
 */
int BudgetApp::getEarliestIncomeYear()
{
	QString query = 	"SELECT MIN(month.year) AS minyear\n"
						"FROM income JOIN month\n"
						"WHERE income.month = month.id";

	QSqlQuery sqlQuery = mBudgetDbHelper->queryDatabaseWithResult(query);

	// Default to the current year.
	int year = QDate::currentDate().year();

	while (sqlQuery.next()) {
		year = sqlQuery.value(0).toInt();
	}

	return year;
}

bool BudgetApp::isANumber(const QString &input)
{
	// Check they input a correct number
	bool isANumber;
	input.toDouble(&isANumber);
	if (!isANumber) {
		alert("Invalid Dollar Amount", "Please enter a correct dollar amount.");
		return false;
	}

	return true;
}

/*
 * Checks that the given month and year comes before the given toMonth and toYear
 */
bool BudgetApp::comesBefore(const QString &month, const QString &year, const QString &toMonth, const QString &toYear)
{
	QDate fromDate = QDate(year.toInt(), month.toInt(), 1);
	QDate toDate = QDate(toYear.toInt(), toMonth.toInt(), 1);

	if (fromDate > toDate) {
		alert("Invalid Date Range", "Please enter a \"Repeat Until\" date that is before the selected date.");
		return false;
	}

	return true;
}

// Opens up an alert dialog box with the specified title and message.
void BudgetApp::alert(const QString &title, const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'OK' button, no 'Cancel' button
    dialog->setTitle(title); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    connect(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), dialog, SLOT(deleteLater()));
    dialog->show();
}

/*
 * Populate the DropDown menus for the year filters at the top of the expense/income page and the
 * year DropDown for the "Repeat Until" input
 */
void BudgetApp::populateYearDropDowns(AbstractPane *root)
{
	// Get the current year
	int currentYear = QDate::currentDate().year();

	// Get the earliest entered year in the database
	int earliestExpenseYear = getEarliestExpenseYear();

	DropDown *expensesYearDropDown = root->findChild<Container*>("expensesMonthYearDropDowns")->findChild<DropDown*>("yearDropDown");
	DropDown *expensesYearRangeDropDown = root->findChild<Page*>("expensesPage")->findChild<DropDown*>("yearRange");
	// Display years starting from earliest year up to five years past the current year
	for (int i = earliestExpenseYear; i <= currentYear + 5; i ++) {
		expensesYearDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
		expensesYearRangeDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
	}

	// Select the current year by default. We can derive the index from currentYear - earliestYear since we know index 0 will be earliestYear.
	expensesYearDropDown->setSelectedIndex(currentYear - earliestExpenseYear);
	expensesYearRangeDropDown->setSelectedIndex(currentYear - earliestExpenseYear);

	// Repeat for incomes DropDowns; get earliest year in the database
	int earliestIncomeYear = getEarliestIncomeYear();

	DropDown *incomesYearDropDown = root->findChild<Container*>("incomesMonthYearDropDowns")->findChild<DropDown*>("yearDropDown");
	DropDown *incomesYearRangeDropDown = root->findChild<Page*>("incomesPage")->findChild<DropDown*>("yearRange");
	// Display years starting from earliest year up to five years past the current year
	for (int i = earliestIncomeYear; i <= currentYear + 5; i ++) {
		incomesYearDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
		incomesYearRangeDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
	}

	// Select the current year by default. We can derive the index from currentYear - earliestYear since we know index 0 will be earliestYear.
	incomesYearDropDown->setSelectedIndex(currentYear - earliestIncomeYear);
	incomesYearRangeDropDown->setSelectedIndex(currentYear - earliestIncomeYear);

	// Populate the Overview Page's year drop downs
	int earliestYear = earliestExpenseYear;
	if (earliestIncomeYear < earliestExpenseYear && earliestIncomeYear != 0) {
		earliestYear = earliestIncomeYear;
	}

	DropDown *overviewMonthYearDropDown = root->findChild<Container*>("overviewContainer")->findChild<DropDown*>("monthYearDropDown");
	// Display years starting from earliest year up to five years past the current year
	for (int i = earliestYear; i <= currentYear + 5; i ++) {
		overviewMonthYearDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
	}

	// Select the current year by default. We can derive the index from currentYear - earliestYear since we know index 0 will be earliestYear.
	overviewMonthYearDropDown->setSelectedIndex(currentYear - earliestYear);

	DropDown *overviewYearDropDown = root->findChild<Container*>("overviewContainer")->findChild<DropDown*>("yearDropDown");
	// Display years starting from earliest year up to five years past the current year
	for (int i = earliestYear; i <= currentYear + 5; i ++) {
		overviewYearDropDown->add(Option::create().text(QString::number(i)).value(QString::number(i)));
	}

	// Select the current year by default. We can derive the index from currentYear - earliestYear since we know index 0 will be earliestYear.
	overviewYearDropDown->setSelectedIndex(currentYear - earliestYear);
}
