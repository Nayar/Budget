/*
 * BudgetDbHelper.cpp
 *
 *  Created on: May 8, 2013
 *      Author: ryanmcdonald
 */

#include "BudgetDbHelper.hpp"
#include "BudgetDbUpdater.hpp"

using namespace bb::data;

/*
 * Creates a new BudgetDbHelper object using the specified database.
 *
 * If this is the first time running the app, then the empty db will be copied from the assets folder into
 * the app's data folder so we can perform writes to it.
 *
 * If the app has been run before, then they already have a copy of the database, so we have to check that
 * their version is not out of date compared to the db in the assets folder.
 */
BudgetDbHelper::BudgetDbHelper(const QString databaseName)
{
	if (copyDbToDataFolder(databaseName)) {
		mDbNameWithPath = "./data/" + databaseName;

		mDb = QSqlDatabase::addDatabase("QSQLITE");
		mDb.setDatabaseName(mDbNameWithPath);

		if (!mDb.isValid()) {
			qWarning() << "Could not set database name, probably due to an invalid driver.";
		}

		bool success = mDb.open();

		if (!success) {
			qWarning() << "Could not open database.";
		}

	}

	// Make sure their database is up to date
	if (isDatabaseOutdated()) {
		updateDatabaseSchema();
	}
}

BudgetDbHelper::~BudgetDbHelper()
{
    if (mDb.isOpen()) {
        QSqlDatabase::removeDatabase(mDbNameWithPath);
        mDb.removeDatabase("QSQLITE");
    }
}

bool BudgetDbHelper::copyDbToDataFolder(const QString databaseName)
{
    // First, we check to see if the file already exists in the
    // data folder (that is, the file was copied already).
    QString dataFolder = QDir::homePath();
    QString newFileName = dataFolder + "/" + databaseName;
    QFile newFile(newFileName);

    if (!newFile.exists()) {
        // If the file is not already in the data folder, we copy
        // it from the assets folder (read-only) to the data folder
        // (read-write).
        QString appFolder(QDir::homePath());
        appFolder.chop(4);
        QString originalFileName = appFolder + "app/native/assets/sql/" + databaseName;
        QFile originalFile(originalFileName);

        if (originalFile.exists()) {
            return originalFile.copy(newFileName);
        } else {
            qDebug() << "Failed to copy file, database file does not exist.";
            return false;
        }
    }

    return true;
}

QVariantList BudgetDbHelper::getExpenses()
{
	QVariantList sqlData;

	SqlDataAccess sqlDataAccess(mDbNameWithPath);

	sqlData = sqlDataAccess.execute("SELECT * FROM expense").value<QVariantList>();

	if (sqlDataAccess.hasError()) {
		DataAccessError err = sqlDataAccess.error();
		qWarning() << "SQL error: type=" << err.errorType() << ": " << err.errorMessage();
	}

	return sqlData;
}

QVariantList BudgetDbHelper::getExpenses(const QString month, const QString year)
{
	QVariantList sqlData;

	SqlDataAccess sqlDataAccess(mDbNameWithPath);

	int monthId = getMonthId(month, year);
	QString query = QString("SELECT * FROM expense WHERE month = %1").arg(monthId);

	sqlData = sqlDataAccess.execute(query).value<QVariantList>();

	if (sqlDataAccess.hasError()) {
		DataAccessError err = sqlDataAccess.error();
		qWarning() << "SQL error: type=" << err.errorType() << ": " << err.errorMessage();
	}

	return sqlData;
}

bool BudgetDbHelper::deleteExpenseById(QVariant id)
{
    // Query for deleting an entry in the table.
    if (id.canConvert(QVariant::String)) {
        QString query = "DELETE FROM expense WHERE id=" + id.toString();
        return queryDatabase(query);
    }

    qWarning() << "Failed to delete item with id: " << id;

    return false;
}

QVariant BudgetDbHelper::insertExpense(QVariantMap map)
{
    QSqlQuery sqlQuery(mDb);

    sqlQuery.prepare("INSERT INTO expense (id, name, amount, imagepath, month) VALUES (NULL, :name, :amount, :imagepath, :month)");

    sqlQuery.bindValue(":name", map["name"]);
    sqlQuery.bindValue(":amount", map["amount"]);
    sqlQuery.bindValue(":imagepath", map["imagepath"]);
    sqlQuery.bindValue(":month", map["month"]);

    sqlQuery.exec();

    QSqlError err = sqlQuery.lastError();
    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    return sqlQuery.lastInsertId();
}

bool BudgetDbHelper::updateExpense(QVariantMap map)
{
    QSqlQuery sqlQuery(mDb);

    sqlQuery.prepare("UPDATE expense SET name=:name, amount=:amount, imagepath=:imagepath WHERE id=:id");

    sqlQuery.bindValue(":name", map["name"]);
    sqlQuery.bindValue(":amount", map["amount"]);
    sqlQuery.bindValue(":imagepath", map["imagepath"]);
    sqlQuery.bindValue(":id", map["id"]);

    sqlQuery.exec();

    QSqlError err = sqlQuery.lastError();
    if (!err.isValid()) {
        return true;
    }

    qWarning() << "SQL reported an error : " << err.text();

    return false;
}

QVariantList BudgetDbHelper::getIncomes()
{
	QVariantList sqlData;

	SqlDataAccess sqlDataAccess(mDbNameWithPath);

	sqlData = sqlDataAccess.execute("SELECT * FROM income").value<QVariantList>();

	if (sqlDataAccess.hasError()) {
		DataAccessError err = sqlDataAccess.error();
		qWarning() << "SQL error: type=" << err.errorType() << ": " << err.errorMessage();
	}

	return sqlData;
}

QVariantList BudgetDbHelper::getIncomes(const QString month, const QString year)
{
	QVariantList sqlData;

	SqlDataAccess sqlDataAccess(mDbNameWithPath);

	int monthId = getMonthId(month, year);
	QString query = QString("SELECT * FROM income WHERE month = %1").arg(monthId);

	sqlData = sqlDataAccess.execute(query).value<QVariantList>();

	if (sqlDataAccess.hasError()) {
		DataAccessError err = sqlDataAccess.error();
		qWarning() << "SQL error: type=" << err.errorType() << ": " << err.errorMessage();
	}

	return sqlData;
}

bool BudgetDbHelper::deleteIncomeById(QVariant id)
{
    if (id.canConvert(QVariant::String)) {
        QString query = "DELETE FROM income WHERE id=" + id.toString();
        return queryDatabase(query);
    }

    qWarning() << "Failed to delete income entry with id: " << id;

    return false;
}

QVariant BudgetDbHelper::insertIncome(QVariantMap map)
{
    QSqlQuery sqlQuery(mDb);

    sqlQuery.prepare("INSERT INTO income (id, name, amount, imagepath, month) VALUES (NULL, :name, :amount, :imagepath, :month)");

    sqlQuery.bindValue(":name", map["name"]);
    sqlQuery.bindValue(":amount", map["amount"]);
    sqlQuery.bindValue(":imagepath", map["imagepath"]);
    sqlQuery.bindValue(":month", map["month"]);

    sqlQuery.exec();

    QSqlError err = sqlQuery.lastError();
    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    return sqlQuery.lastInsertId();
}

bool BudgetDbHelper::updateIncome(QVariantMap map)
{
    QSqlQuery sqlQuery(mDb);

    sqlQuery.prepare("UPDATE income SET name=:name, amount=:amount, imagepath=:imagepath WHERE id=:id");

    sqlQuery.bindValue(":name", map["name"]);
    sqlQuery.bindValue(":amount", map["amount"]);
    sqlQuery.bindValue(":imagepath", map["imagepath"]);
    sqlQuery.bindValue(":id", map["id"]);

    sqlQuery.exec();

    QSqlError err = sqlQuery.lastError();
    if (!err.isValid()) {
        return true;
    }

    qWarning() << "SQL reported an error : " << err.text();

    return false;
}

QVariant BudgetDbHelper::insertMonth(QVariantMap map)
{
    QSqlQuery sqlQuery(mDb);

    sqlQuery.prepare("INSERT INTO month (id, month, year) VALUES (NULL, :month, :year)");

    sqlQuery.bindValue(":month", map["month"]);
    sqlQuery.bindValue(":year", map["year"]);

    sqlQuery.exec();

    QSqlError err = sqlQuery.lastError();
    if (err.isValid()) {
        qWarning() << "SQL reported an error : " << err.text();
    }

    return sqlQuery.lastInsertId();
}

/*
 * Checks if a month entry has any existing expense or income entries associated with it.
 * If not, it deletes that month entry. This is useful for populating the DropDown lists so that
 * we aren't continuously populating them with unused year values in the past.
 */
void BudgetDbHelper::deleteUnusedMonth(QVariant id)
{
	// Check if there are any expense or income entries for that month
	QString query = QString("DELETE FROM month WHERE id = %1 AND id NOT IN ("
							"SELECT month FROM expense WHERE month = %1\n"
							"UNION ALL\n"
							"SELECT month FROM income WHERE month = %1)\n"
							"\n").arg(id.toInt());

	queryDatabase(query);
}

int BudgetDbHelper::getMonthId(const QString month, const QString year)
{
	QString query = QString("SELECT id FROM month WHERE month = %1 AND year = %2").arg(month, year);

	QSqlQuery sqlQuery = queryDatabaseWithResult(query);

	int id = 0;
	while (sqlQuery.next()) {
		id = sqlQuery.value(0).toInt();
	}

	// If we get no results, id will be 0
	return id;
}

double BudgetDbHelper::getMonthExpenseAmount(const int &month, const int &year)
{
	QString query = QString("SELECT SUM(amount)\n"
							"FROM expense, month\n"
							"WHERE expense.month = month.id\n"
							"AND month.month = %1\n"
							"AND month.year = %2").arg(month).arg(year);

	QSqlQuery sqlQuery = queryDatabaseWithResult(query);

	double sum = 0.0;
	while (sqlQuery.next()) {
		sum = sqlQuery.value(0).toDouble();
	}

	// If we get no results, then there are $0 worth of expenses for the year
	return sum;
}

double BudgetDbHelper::getYearExpenseAmount(const int &year)
{
	QString query = QString("SELECT SUM(amount)\n"
							"FROM expense, month\n"
							"WHERE expense.month = month.id\n"
							"AND month.year = %1").arg(year);

	QSqlQuery sqlQuery = queryDatabaseWithResult(query);

	double sum = 0.0;
	while (sqlQuery.next()) {
		sum = sqlQuery.value(0).toDouble();
	}

	// If we get no results, then there are $0 worth of expenses for the year
	return sum;
}

double BudgetDbHelper::getMonthIncomeAmount(const int &month, const int &year)
{
	QString query = QString("SELECT SUM(amount)\n"
							"FROM income, month\n"
							"WHERE income.month = month.id\n"
							"AND month.month = %1\n"
							"AND month.year= %2").arg(month).arg(year);

	QSqlQuery sqlQuery = queryDatabaseWithResult(query);

	double sum = 0.0;
	while (sqlQuery.next()) {
		sum = sqlQuery.value(0).toDouble();
	}

	// If we get no results, then there are $0 worth of expenses for the year
	return sum;
}

double BudgetDbHelper::getYearIncomeAmount(const int &year)
{
	QString query = QString("SELECT SUM(amount)\n"
							"FROM income, month\n"
							"WHERE income.month = month.id\n"
							"AND month.year = %1").arg(year);

	QSqlQuery sqlQuery = queryDatabaseWithResult(query);

	double sum = 0.0;
	while (sqlQuery.next()) {
		sum = sqlQuery.value(0).toDouble();
	}

	// If we get no results, then there are $0 worth of expenses for the year
	return sum;
}

QSqlQuery BudgetDbHelper::queryDatabaseWithResult(const QString query)
{
	// Execute the query.
    QSqlQuery sqlQuery(query, mDb);

    QSqlError err = sqlQuery.lastError();
    if (err.isValid()) {
        qWarning() << "SQL reported an error for query: " << query << " error: " << mDb.lastError().text();
    }

    return sqlQuery;
}

bool BudgetDbHelper::queryDatabase(const QString query)
{
    // Execute the query.
    QSqlQuery sqlQuery(query, mDb);

    QSqlError err = sqlQuery.lastError();
    if (err.isValid()) {
        qWarning() << "SQL reported an error for query: " << query << " error: " << mDb.lastError().text();
        return false;
    }

    return true;
}

bool BudgetDbHelper::isDatabaseOutdated()
{
	// Grab version of database installed in the data directory
	QString query = "SELECT value FROM metadata WHERE name = 'database_version'";
	QSqlQuery userSqlQuery = queryDatabaseWithResult(query);

	int valueField = userSqlQuery.record().indexOf("value");
	while (userSqlQuery.next()) {
		mInstalledDbVersion = userSqlQuery.value(valueField).toInt();
	}

	// Check the version of the database in the assets directory (the latest schema)
	QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE", "assets_db_connection");
	QString appFolder(QDir::homePath());
	appFolder.chop(4);
	QString databasePath = appFolder + "app/native/assets/sql/budget.db";
	database.setDatabaseName(databasePath);

	if (!database.isValid()) {
		qWarning() << "Could not set database name, probably due to an invalid driver.";
	}

	bool success = database.open();

	if (!success) {
		qWarning() << "Could not open database.";
	}

	// Execute the query.
	QSqlQuery defaultSqlQuery(query, database);

	QSqlError err = defaultSqlQuery.lastError();
	if (err.isValid()) {
		qWarning() << "SQL reported an error for query: " << query << " error: " << database.lastError().text();
	}

	valueField = defaultSqlQuery.record().indexOf("value");
	while (defaultSqlQuery.next()) {
		mLatestDbVersion = defaultSqlQuery.value(valueField).toInt();
	}

	database.close();

	if (mInstalledDbVersion < mLatestDbVersion) {
		return true;
	}

	return false;
}

void BudgetDbHelper::updateDatabaseSchema()
{
//	BudgetDbUpdater dbUpdater = BudgetDbUpdater(mDb);

	// TODO: This is sample code for when there is a version 2 of the database
//	if (mInstalledDbVersion < 2) {
//		if (dbUpdater.toV2()) mInstalledDbVersion = 2;
//	}

	// TODO: If their app isn't fully updated at this point (installedDbVersion != latestDbVersion),
	// maybe alert them or close/restart the app?

//	QString query = QString("UPDATE metadata SET value = '%1' WHERE name = 'database_version'").arg(mInstalledDbVersion);
//	queryDatabase(query);
}
