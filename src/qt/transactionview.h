// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2017-2018 The Puppycoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_TRANSACTIONVIEW_H
#define BITCOIN_QT_TRANSACTIONVIEW_H

#include "guiutil.h"

#include <QKeyEvent>
#include <QWidget>
#include <QAction>

class TransactionFilterProxy;
class WalletModel;

QT_BEGIN_NAMESPACE
class QComboBox;
class QDateTimeEdit;
class QFrame;
class QItemSelectionModel;
class QLineEdit;
class QMenu;
class QModelIndex;
class QSignalMapper;
class QTableView;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class TransactionView : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionView(QWidget* parent = 0);

    void setModel(WalletModel* model);

    // Date ranges for filter
    enum DateEnum {
        All,
        Today,
        ThisWeek,
        ThisMonth,
        LastMonth,
        ThisYear,
        Range
    };

    enum ColumnWidths {
        STATUS_COLUMN_WIDTH = 23,
        WATCHONLY_COLUMN_WIDTH = 23,
        DATE_COLUMN_WIDTH = 120,
        TYPE_COLUMN_WIDTH = 240,
        AMOUNT_MINIMUM_COLUMN_WIDTH = 120,
        MINIMUM_COLUMN_WIDTH = 23
    };

private:
    WalletModel* model;
    TransactionFilterProxy* transactionProxyModel;
    QTableView* transactionView;
    QComboBox* dateWidget;
    QComboBox* typeWidget;
    QComboBox* watchOnlyWidget;
    QLineEdit* addressWidget;
    QLineEdit* amountWidget;
    QAction* hideOrphansAction;

    QMenu* contextMenu;
    QSignalMapper* mapperThirdPartyTxUrls;

    QFrame* dateRangeWidget;
    QDateTimeEdit* dateFrom;
    QDateTimeEdit* dateTo;

    QWidget* createDateRangeWidget();

    GUIUtil::TableViewLastColumnResizingFixer* columnResizingFixer;

    virtual void resizeEvent(QResizeEvent* event);

    bool eventFilter(QObject* obj, QEvent* event);

private slots:
    void contextualMenu(const QPoint&);
    void dateRangeChanged();
    void showDetails();
    void copyAddress();
    void editLabel();
    void copyLabel();
    void copyAmount();
    void copyTxID();
    void openThirdPartyTxUrl(QString url);
    void updateWatchOnlyColumn(bool fHaveWatchOnly);

signals:
    void doubleClicked(const QModelIndex&);

    /**  Fired when a message should be reported to the user */
    void message(const QString& title, const QString& message, unsigned int style);

    /** Send computed sum back to wallet-view */
    void trxAmount(QString amount);

public slots:
    void chooseDate(int idx);
    void chooseType(int idx);
    void hideOrphans(bool fHide);
    void updateHideOrphans(bool fHide);
    void chooseWatchonly(int idx);
    void changedPrefix(const QString& prefix);
    void changedAmount(const QString& amount);
    void exportClicked();
    void focusTransaction(const QModelIndex&);
    void computeSum();
};

#endif // BITCOIN_QT_TRANSACTIONVIEW_H
