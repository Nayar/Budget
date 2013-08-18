/*
 * BudgetDbUpdater.hpp
 *
 *  Created on: May 17, 2013
 *      Author: ryanmcdonald
 */

#ifndef BUDGETDBUPDATER_HPP_
#define BUDGETDBUPDATER_HPP_

#include <QObject>
#include <QtSql/QtSql>
#include <bb/data/SqlDataAccess>

class BudgetDbUpdater {
public:
	BudgetDbUpdater(const QSqlDatabase &db);
	virtual ~BudgetDbUpdater();

	bool toV2();

private:
	QSqlDatabase mDb;

	bool queryDatabase(const QString query);
};

#endif /* BUDGETDBUPDATER_HPP_ */
