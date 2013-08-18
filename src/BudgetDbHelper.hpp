/*
 * BudgetDbHelper.hpp
 *
 *  Created on: May 8, 2013
 *      Author: ryanmcdonald
 */

#ifndef BUDGETDBHELPER_HPP_
#define BUDGETDBHELPER_HPP_

#include <QtSql/QtSql>
#include <bb/data/SqlDataAccess>

using namespace bb::data;

class BudgetDbHelper
{
public:
    BudgetDbHelper(QString databaseName);
    ~BudgetDbHelper();

    QVariantList getExpenses();
    QVariantList getExpenses(const QString month, const QString year);
    bool deleteExpenseById(QVariant id);
    QVariant insertExpense(QVariantMap map);
    bool updateExpense(QVariantMap map);

    QVariantList getIncomes();
    QVariantList getIncomes(const QString month, const QString year);
	bool deleteIncomeById(QVariant id);
	QVariant insertIncome(QVariantMap map);
	bool updateIncome(QVariantMap map);

	QVariant insertMonth(QVariantMap map);
	void deleteUnusedMonth(QVariant id);
	int getMonthId(const QString month, const QString year);

	double getMonthExpenseAmount(const int &month, const int &year);
	double getYearExpenseAmount(const int &year);
	double getMonthIncomeAmount(const int &month, const int &year);
	double getYearIncomeAmount(const int &year);

    QSqlQuery queryDatabaseWithResult(const QString query);

private:
    bool copyDbToDataFolder(const QString databaseName);
    bool queryDatabase(const QString query);
    bool isDatabaseOutdated();
    void updateDatabaseSchema();

    QSqlDatabase mDb;
    QString mDbNameWithPath;
    QString mTable;
    int mInstalledDbVersion;
    int mLatestDbVersion;
};


#endif /* BUDGETDBHELPER_HPP_ */
