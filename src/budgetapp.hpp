#ifndef BudgetApp_HPP_
#define BudgetApp_HPP_

#include <bb/cascades/Application>
#include <bb/cascades/DataModel>
#include <bb/data/SqlDataAccess>
#include <QObject>

using namespace bb::cascades;
using namespace bb::data;

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
        class GroupDataModel;
        class ListView;
    }
}

class QTranslator;
class BudgetDbHelper;

/*!
 * @brief Application object
 *
 *
 */

class BudgetApp : public QObject
{
    Q_OBJECT
public:
    BudgetApp();
    BudgetApp(bb::cascades::Application *app);
    virtual ~BudgetApp() { }

    void onStart();

    Q_INVOKABLE
    bool addExpense(const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year, const QString &toMonth, const QString &toYear);
    Q_INVOKABLE
    bool updateExpense(const QString &name, const QString &amount, const QString &imagePath);
    Q_INVOKABLE
    void deleteExpense(const QString &itemId, const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year);

    Q_INVOKABLE
	bool addIncome(const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year, const QString &toMonth, const QString &toYear);
	Q_INVOKABLE
	bool updateIncome(const QString &name, const QString &amount, const QString &imagePath);
	Q_INVOKABLE
	void deleteIncome(const QString &itemId, const QString &name, const QString &amount, const QString &imagePath, const QString &month, const QString &year);

	Q_INVOKABLE
	double getMonthExpenseAmount(int month, int year);
	Q_INVOKABLE
	double getYearExpenseAmount(int year);
	Q_INVOKABLE
	double getTotalMonthlyExpenseAmount();

	Q_INVOKABLE
	double getMonthIncomeAmount(const int &month, const int &year);
	Q_INVOKABLE
	double getYearIncomeAmount(const int &year);
	Q_INVOKABLE
	double getTotalMonthlyIncomeAmount();

	Q_INVOKABLE
	QVariantList getExpenses(const QString &month, const QString &year);
	Q_INVOKABLE
	QVariantList getIncomes(const QString &month, const QString &year);

private slots:
    void onSystemLanguageChanged();
private:
    bool loadQMLScene();
    void populateYearDropDowns(AbstractPane *root);

    int addMonth(const QString &month, const QString &year);

    int getEarliestExpenseYear();
    int getEarliestIncomeYear();

    bool isANumber(const QString &input);
    bool comesBefore(const QString &month, const QString &year, const QString &toMonth, const QString &toYear);

    // To alert the user if something has gone wrong
    void alert(const QString &title, const QString &message);

    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    BudgetDbHelper *mBudgetDbHelper;

    GroupDataModel *mExpensesDataModel;
    ListView *mExpensesListView;

    GroupDataModel *mIncomesDataModel;
	ListView *mIncomesListView;
};

#endif /* BudgetApp_HPP_ */
