/*
 * BudgetDbSql.cpp
 *
 *  Created on: May 17, 2013
 *      Author: ryanmcdonald
 */

#include "BudgetDbUpdater.hpp"

BudgetDbUpdater::BudgetDbUpdater(const QSqlDatabase &db) {
	mDb = db;
}

BudgetDbUpdater::~BudgetDbUpdater() {

}

bool BudgetDbUpdater::toV2()
{
	mDb.transaction();

	QString query =	"CREATE TABLE IF NOT EXISTS month (\n"
					"	id INTEGER PRIMARY KEY,\n"
					"	month INTEGER,\n"
					"	year INTEGER,\n"
					"	UNIQUE (month, year)\n"
					")";
	if (!queryDatabase(query)) {
		mDb.rollback();
		return false;
	}

	query = "ALTER TABLE expense\n"
			"ADD COLUMN month INTEGER REFERENCES month (id) ON DELETE CASCADE";
	if (!queryDatabase(query)) {
		mDb.rollback();
		return false;
	}

	query = "ALTER TABLE income\n"
			"ADD COLUMN month INTEGER REFERENCES month (id) ON DELETE CASCADE";
	if (!queryDatabase(query)) {
		mDb.rollback();
		return false;
	}

	mDb.commit();
	return true;
}

bool BudgetDbUpdater::queryDatabase(const QString query)
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
