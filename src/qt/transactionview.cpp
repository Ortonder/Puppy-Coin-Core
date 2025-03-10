// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2017-2019 The Puppycoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transactionview.h"

#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include "guiinterface.h"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QDesktopServices>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPoint>
#include <QScrollBar>
#include <QSettings>
#include <QSignalMapper>
#include <QTableView>
#include <QUrl>
#include <QVBoxLayout>

TransactionView::TransactionView(QWidget* parent) : QWidget(parent), model(0), transactionProxyModel(0),
                                                    transactionView(0)
{
    QSettings settings;
    // Build filter row
    setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
#ifdef Q_OS_MAC
    hlayout->setSpacing(5);
    hlayout->addSpacing(26);
#else
    hlayout->setSpacing(0);
    hlayout->addSpacing(23);
#endif

    watchOnlyWidget = new QComboBox(this);
    watchOnlyWidget->setFixedWidth(24);
    watchOnlyWidget->addItem("", TransactionFilterProxy::WatchOnlyFilter_All);
    watchOnlyWidget->addItem(QIcon(":/icons/eye_plus"), "", TransactionFilterProxy::WatchOnlyFilter_Yes);
    watchOnlyWidget->addItem(QIcon(":/icons/eye_minus"), "", TransactionFilterProxy::WatchOnlyFilter_No);
    hlayout->addWidget(watchOnlyWidget);

    dateWidget = new QComboBox(this);
#ifdef Q_OS_MAC
    dateWidget->setFixedWidth(121);
#else
    dateWidget->setFixedWidth(120);
#endif
    dateWidget->addItem(tr("All"), All);
    dateWidget->addItem(tr("Today"), Today);
    dateWidget->addItem(tr("This week"), ThisWeek);
    dateWidget->addItem(tr("This month"), ThisMonth);
    dateWidget->addItem(tr("Last month"), LastMonth);
    dateWidget->addItem(tr("This year"), ThisYear);
    dateWidget->addItem(tr("Range..."), Range);
    dateWidget->setCurrentIndex(settings.value("transactionDate").toInt());
    hlayout->addWidget(dateWidget);

    typeWidget = new QComboBox(this);
#ifdef Q_OS_MAC
    typeWidget->setFixedWidth(TYPE_COLUMN_WIDTH + 1);
#else
    typeWidget->setFixedWidth(TYPE_COLUMN_WIDTH);
#endif

    typeWidget->addItem(tr("All"), TransactionFilterProxy::ALL_TYPES);
    typeWidget->addItem(tr("Most Common"), TransactionFilterProxy::COMMON_TYPES);
    typeWidget->addItem(tr("Received with"), TransactionFilterProxy::TYPE(TransactionRecord::RecvWithAddress) | TransactionFilterProxy::TYPE(TransactionRecord::RecvFromOther));
    typeWidget->addItem(tr("Sent to"), TransactionFilterProxy::TYPE(TransactionRecord::SendToAddress) | TransactionFilterProxy::TYPE(TransactionRecord::SendToOther));

/* Obsolete Obfuscation entries. Remove once the corresponding TYPES are removed:
 *
    typeWidget->addItem(tr("Obfuscated"), TransactionFilterProxy::TYPE(TransactionRecord::Obfuscated));
    typeWidget->addItem(tr("Obfuscation Make Collateral Inputs"), TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationMakeCollaterals));
    typeWidget->addItem(tr("Obfuscation Create Denominations"), TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationCreateDenominations));
    typeWidget->addItem(tr("Obfuscation Denominate"), TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationDenominate));
    typeWidget->addItem(tr("Obfuscation Collateral Payment"), TransactionFilterProxy::TYPE(TransactionRecord::ObfuscationCollateralPayment));
 */

    typeWidget->addItem(tr("To yourself"), TransactionFilterProxy::TYPE(TransactionRecord::SendToSelf));
    typeWidget->addItem(tr("Mined"), TransactionFilterProxy::TYPE(TransactionRecord::Generated));
    typeWidget->addItem(tr("Minted"), TransactionFilterProxy::TYPE(TransactionRecord::StakeMint) | TransactionFilterProxy::TYPE(TransactionRecord::StakeZPUP));
    typeWidget->addItem(tr("Masternode Reward"), TransactionFilterProxy::TYPE(TransactionRecord::MNReward));
    typeWidget->addItem(tr("Received PUP from zPUP"), TransactionFilterProxy::TYPE(TransactionRecord::RecvFromZerocoinSpend));
    typeWidget->addItem(tr("Zerocoin Mint"), TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinMint));
    typeWidget->addItem(tr("Zerocoin Spend"), TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend));
    typeWidget->addItem(tr("Zerocoin Spend, Change in zPUP"), TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend_Change_zPiv));
    typeWidget->addItem(tr("Zerocoin Spend to Self"), TransactionFilterProxy::TYPE(TransactionRecord::ZerocoinSpend_FromMe));
    typeWidget->addItem(tr("Other"), TransactionFilterProxy::TYPE(TransactionRecord::Other));
    typeWidget->setCurrentIndex(settings.value("transactionType").toInt());

    hlayout->addWidget(typeWidget);

    addressWidget = new QLineEdit(this);
    addressWidget->setPlaceholderText(tr("Enter address or label to search"));
    hlayout->addWidget(addressWidget);

    amountWidget = new QLineEdit(this);
    amountWidget->setPlaceholderText(tr("Min amount"));
#ifdef Q_OS_MAC
    amountWidget->setFixedWidth(97);
#else
    amountWidget->setFixedWidth(100);
#endif
    amountWidget->setValidator(new QDoubleValidator(0, 1e20, 8, this));
    hlayout->addWidget(amountWidget);

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->setSpacing(0);

    QTableView* view = new QTableView(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(createDateRangeWidget());
    vlayout->addWidget(view);
    vlayout->setSpacing(0);
    int width = view->verticalScrollBar()->sizeHint().width();
// Cover scroll bar width with spacing
#ifdef Q_OS_MAC
    hlayout->addSpacing(width + 2);
#else
    hlayout->addSpacing(width);
#endif
    // Always show scroll bar
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    view->installEventFilter(this);

    transactionView = view;

    // Actions
    QAction* copyAddressAction = new QAction(tr("Copy address"), this);
    QAction* copyLabelAction = new QAction(tr("Copy label"), this);
    QAction* copyAmountAction = new QAction(tr("Copy amount"), this);
    QAction* copyTxIDAction = new QAction(tr("Copy transaction ID"), this);
    QAction* editLabelAction = new QAction(tr("Edit label"), this);
    QAction* showDetailsAction = new QAction(tr("Show transaction details"), this);
    hideOrphansAction = new QAction(tr("Hide orphan stakes"), this);

    hideOrphansAction->setCheckable(true);
    hideOrphansAction->setChecked(settings.value("fHideOrphans", false).toBool());

    contextMenu = new QMenu();
    contextMenu->addAction(copyAddressAction);
    contextMenu->addAction(copyLabelAction);
    contextMenu->addAction(copyAmountAction);
    contextMenu->addAction(copyTxIDAction);
    contextMenu->addAction(editLabelAction);
    contextMenu->addAction(showDetailsAction);
    contextMenu->addAction(hideOrphansAction);

    mapperThirdPartyTxUrls = new QSignalMapper(this);

    // Connect actions
    connect(mapperThirdPartyTxUrls, SIGNAL(mapped(QString)), this, SLOT(openThirdPartyTxUrl(QString)));

    connect(dateWidget, SIGNAL(activated(int)), this, SLOT(chooseDate(int)));
    connect(typeWidget, SIGNAL(activated(int)), this, SLOT(chooseType(int)));
    connect(watchOnlyWidget, SIGNAL(activated(int)), this, SLOT(chooseWatchonly(int)));
    connect(addressWidget, SIGNAL(textChanged(QString)), this, SLOT(changedPrefix(QString)));
    connect(amountWidget, SIGNAL(textChanged(QString)), this, SLOT(changedAmount(QString)));

    connect(view, SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(doubleClicked(QModelIndex)));
    connect(view, SIGNAL(clicked(QModelIndex)), this, SLOT(computeSum()));
    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualMenu(QPoint)));

    connect(copyAddressAction, SIGNAL(triggered()), this, SLOT(copyAddress()));
    connect(copyLabelAction, SIGNAL(triggered()), this, SLOT(copyLabel()));
    connect(copyAmountAction, SIGNAL(triggered()), this, SLOT(copyAmount()));
    connect(copyTxIDAction, SIGNAL(triggered()), this, SLOT(copyTxID()));
    connect(editLabelAction, SIGNAL(triggered()), this, SLOT(editLabel()));
    connect(showDetailsAction, SIGNAL(triggered()), this, SLOT(showDetails()));
    connect(hideOrphansAction, SIGNAL(toggled(bool)), this, SLOT(updateHideOrphans(bool)));
}

void TransactionView::setModel(WalletModel* model)
{
    QSettings settings;
    this->model = model;
    if (model) {
        transactionProxyModel = new TransactionFilterProxy(this);
        transactionProxyModel->setSourceModel(model->getTransactionTableModel());
        transactionProxyModel->setDynamicSortFilter(true);
        transactionProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        transactionProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

        transactionProxyModel->setSortRole(Qt::EditRole);

        transactionView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        transactionView->setModel(transactionProxyModel);
        transactionView->setAlternatingRowColors(true);
        transactionView->setSelectionBehavior(QAbstractItemView::SelectRows);
        transactionView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        transactionView->setSortingEnabled(true);
        transactionView->sortByColumn(TransactionTableModel::Date, Qt::DescendingOrder);
        transactionView->verticalHeader()->hide();

        transactionView->setColumnWidth(TransactionTableModel::Status, STATUS_COLUMN_WIDTH);
        transactionView->setColumnWidth(TransactionTableModel::Watchonly, WATCHONLY_COLUMN_WIDTH);
        transactionView->setColumnWidth(TransactionTableModel::Date, DATE_COLUMN_WIDTH);
        transactionView->setColumnWidth(TransactionTableModel::Type, TYPE_COLUMN_WIDTH);
        transactionView->setColumnWidth(TransactionTableModel::Amount, AMOUNT_MINIMUM_COLUMN_WIDTH);

        // Note: it's a good idea to connect this signal AFTER the model is set
        connect(transactionView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(computeSum()));

        columnResizingFixer = new GUIUtil::TableViewLastColumnResizingFixer(transactionView, AMOUNT_MINIMUM_COLUMN_WIDTH, MINIMUM_COLUMN_WIDTH);

        if (model->getOptionsModel()) {
            // Add third party transaction URLs to context menu
            QStringList listUrls = model->getOptionsModel()->getThirdPartyTxUrls().split("|", QString::SkipEmptyParts);
            for (int i = 0; i < listUrls.size(); ++i) {
                QString host = QUrl(listUrls[i].trimmed(), QUrl::StrictMode).host();
                if (!host.isEmpty()) {
                    QAction* thirdPartyTxUrlAction = new QAction(host, this); // use host as menu item label
                    if (i == 0)
                        contextMenu->addSeparator();
                    contextMenu->addAction(thirdPartyTxUrlAction);
                    connect(thirdPartyTxUrlAction, SIGNAL(triggered()), mapperThirdPartyTxUrls, SLOT(map()));
                    mapperThirdPartyTxUrls->setMapping(thirdPartyTxUrlAction, listUrls[i].trimmed());
                }
            }

            connect(model->getOptionsModel(), SIGNAL(hideOrphansChanged(bool)), this, SLOT(updateHideOrphans(bool)));
        }

        // show/hide column Watch-only
        updateWatchOnlyColumn(model->haveWatchOnly());

        // Watch-only signal
        connect(model, SIGNAL(notifyWatchonlyChanged(bool)), this, SLOT(updateWatchOnlyColumn(bool)));

        // Update transaction list with persisted settings
        chooseType(settings.value("transactionType").toInt());
        chooseDate(settings.value("transactionDate").toInt());

        // Hide orphans
        hideOrphans(settings.value("fHideOrphans", false).toBool());
    }
}

void TransactionView::chooseDate(int idx)
{
    if (!transactionProxyModel)
        return;
    QDate current = QDate::currentDate();
    dateRangeWidget->setVisible(false);
    switch (dateWidget->itemData(idx).toInt()) {
    case All:
        transactionProxyModel->setDateRange(
            TransactionFilterProxy::MIN_DATE,
            TransactionFilterProxy::MAX_DATE);
        break;
    case Today:
        transactionProxyModel->setDateRange(
            QDateTime(current),
            TransactionFilterProxy::MAX_DATE);
        break;
    case ThisWeek: {
        // Find last Monday
        QDate startOfWeek = current.addDays(-(current.dayOfWeek() - 1));
        transactionProxyModel->setDateRange(
            QDateTime(startOfWeek),
            TransactionFilterProxy::MAX_DATE);

    } break;
    case ThisMonth:
        transactionProxyModel->setDateRange(
            QDateTime(QDate(current.year(), current.month(), 1)),
            TransactionFilterProxy::MAX_DATE);
        break;
    case LastMonth:
        transactionProxyModel->setDateRange(
            QDateTime(QDate(current.year(), current.month() - 1, 1)),
            QDateTime(QDate(current.year(), current.month(), 1)));
        break;
    case ThisYear:
        transactionProxyModel->setDateRange(
            QDateTime(QDate(current.year(), 1, 1)),
            TransactionFilterProxy::MAX_DATE);
        break;
    case Range:
        dateRangeWidget->setVisible(true);
        dateRangeChanged();
        break;
    }
    // Persist settings
    if (dateWidget->itemData(idx).toInt() != Range) {
        QSettings settings;
        settings.setValue("transactionDate", idx);
    }
}

void TransactionView::chooseType(int idx)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setTypeFilter(
        typeWidget->itemData(idx).toInt());
    // Persist settings
    QSettings settings;
    settings.setValue("transactionType", idx);
}

void TransactionView::hideOrphans(bool fHide)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setHideOrphans(fHide);
}

void TransactionView::updateHideOrphans(bool fHide)
{
    QSettings settings;
    if (settings.value("fHideOrphans", false).toBool() != fHide) {
        settings.setValue("fHideOrphans", fHide);
        if (model && model->getOptionsModel())
            emit model->getOptionsModel()->hideOrphansChanged(fHide);
    }
    hideOrphans(fHide);
    // retain consistency with other checkboxes
    if (hideOrphansAction->isChecked() != fHide)
        hideOrphansAction->setChecked(fHide);

}


void TransactionView::chooseWatchonly(int idx)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setWatchOnlyFilter(
        (TransactionFilterProxy::WatchOnlyFilter)watchOnlyWidget->itemData(idx).toInt());
}

void TransactionView::changedPrefix(const QString& prefix)
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setAddressPrefix(prefix);
}

void TransactionView::changedAmount(const QString& amount)
{
    if (!transactionProxyModel)
        return;
    CAmount amount_parsed = 0;

    if (model) {
        // Replace "," by "." so BitcoinUnits::parse will not fail for users entering "," as decimal separator
        QString newAmount = amount;
        newAmount.replace(QString(","), QString("."));

        if (BitcoinUnits::parse(model->getOptionsModel()->getDisplayUnit(), newAmount, &amount_parsed)) {
            transactionProxyModel->setMinAmount(amount_parsed);
        } else {
            transactionProxyModel->setMinAmount(0);
        }
    }
}

void TransactionView::exportClicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Transaction History"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);
    bool fExport = false;

    if (model) {
        // name, column, role
        writer.setModel(transactionProxyModel);
        writer.addColumn(tr("Confirmed"), 0, TransactionTableModel::ConfirmedRole);
        if (model->haveWatchOnly())
            writer.addColumn(tr("Watch-only"), TransactionTableModel::Watchonly);
        writer.addColumn(tr("Date"), 0, TransactionTableModel::DateRole);
        writer.addColumn(tr("Type"), TransactionTableModel::Type, Qt::EditRole);
        writer.addColumn(tr("Label"), 0, TransactionTableModel::LabelRole);
        writer.addColumn(tr("Address"), 0, TransactionTableModel::AddressRole);
        writer.addColumn(BitcoinUnits::getAmountColumnTitle(model->getOptionsModel()->getDisplayUnit()), 0, TransactionTableModel::FormattedAmountRole);
        writer.addColumn(tr("ID"), 0, TransactionTableModel::TxIDRole);

        fExport = writer.write();
    }

    if (fExport) {
        emit message(tr("Exporting Successful"), tr("The transaction history was successfully saved to %1.").arg(filename),
                     CClientUIInterface::MSG_INFORMATION);
    }
    else {
        emit message(tr("Exporting Failed"), tr("There was an error trying to save the transaction history to %1.").arg(filename),
                     CClientUIInterface::MSG_ERROR);
    }
}

void TransactionView::contextualMenu(const QPoint& point)
{
    QModelIndex index = transactionView->indexAt(point);
    if (index.isValid()) {
        contextMenu->exec(QCursor::pos());
    }
}

void TransactionView::copyAddress()
{
    GUIUtil::copyEntryData(transactionView, 0, TransactionTableModel::AddressRole);
}

void TransactionView::copyLabel()
{
    GUIUtil::copyEntryData(transactionView, 0, TransactionTableModel::LabelRole);
}

void TransactionView::copyAmount()
{
    GUIUtil::copyEntryData(transactionView, 0, TransactionTableModel::FormattedAmountRole);
}

void TransactionView::copyTxID()
{
    GUIUtil::copyEntryData(transactionView, 0, TransactionTableModel::TxIDRole);
}

void TransactionView::editLabel()
{
    if (!transactionView->selectionModel() || !model)
        return;
    QModelIndexList selection = transactionView->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        AddressTableModel* addressBook = model->getAddressTableModel();
        if (!addressBook)
            return;
        QString address = selection.at(0).data(TransactionTableModel::AddressRole).toString();
        if (address.isEmpty()) {
            // If this transaction has no associated address, exit
            return;
        }
        // Is address in address book? Address book can miss address when a transaction is
        // sent from outside the UI.
        int idx = addressBook->lookupAddress(address);
        if (idx != -1) {
            // Edit sending / receiving address
            QModelIndex modelIdx = addressBook->index(idx, 0, QModelIndex());
            // Determine type of address, launch appropriate editor dialog type
            QString type = modelIdx.data(AddressTableModel::TypeRole).toString();

            EditAddressDialog dlg(
                type == AddressTableModel::Receive ? EditAddressDialog::EditReceivingAddress : EditAddressDialog::EditSendingAddress, this);
            dlg.setModel(addressBook);
            dlg.loadRow(idx);
            dlg.exec();
        } else {
            // Add sending address
            EditAddressDialog dlg(EditAddressDialog::NewSendingAddress,
                this);
            dlg.setModel(addressBook);
            dlg.setAddress(address);
            dlg.exec();
        }
    }
}

void TransactionView::showDetails()
{
    if (!transactionView->selectionModel())
        return;
    QModelIndexList selection = transactionView->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        TransactionDescDialog dlg(selection.at(0));
        dlg.exec();
    }
}

/** Compute sum of all selected transactions */
void TransactionView::computeSum()
{
    qint64 amount = 0;
    int nDisplayUnit = model->getOptionsModel()->getDisplayUnit();
    if (!transactionView->selectionModel())
        return;
    QModelIndexList selection = transactionView->selectionModel()->selectedRows();

    foreach (QModelIndex index, selection) {
        amount += index.data(TransactionTableModel::AmountRole).toLongLong();
    }
    QString strAmount(BitcoinUnits::formatWithUnit(nDisplayUnit, amount, true, BitcoinUnits::separatorAlways));
    if (amount < 0) strAmount = "<span style='color:red;'>" + strAmount + "</span>";
    emit trxAmount(strAmount);
}

void TransactionView::openThirdPartyTxUrl(QString url)
{
    if (!transactionView || !transactionView->selectionModel())
        return;
    QModelIndexList selection = transactionView->selectionModel()->selectedRows(0);
    if (!selection.isEmpty())
        QDesktopServices::openUrl(QUrl::fromUserInput(url.replace("%s", selection.at(0).data(TransactionTableModel::TxHashRole).toString())));
}

QWidget* TransactionView::createDateRangeWidget()
{
    dateRangeWidget = new QFrame();
    dateRangeWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dateRangeWidget->setContentsMargins(1, 1, 1, 1);
    QHBoxLayout* layout = new QHBoxLayout(dateRangeWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addSpacing(23);
    layout->addWidget(new QLabel(tr("Range:")));

    dateFrom = new QDateTimeEdit(this);
    dateFrom->setDisplayFormat("dd/MM/yy");
    dateFrom->setCalendarPopup(true);
    dateFrom->setMinimumWidth(100);
    dateFrom->setDate(QDate::currentDate().addDays(-7));
    layout->addWidget(dateFrom);
    layout->addWidget(new QLabel(tr("to")));

    dateTo = new QDateTimeEdit(this);
    dateTo->setDisplayFormat("dd/MM/yy");
    dateTo->setCalendarPopup(true);
    dateTo->setMinimumWidth(100);
    dateTo->setDate(QDate::currentDate());
    layout->addWidget(dateTo);
    layout->addStretch();

    // Hide by default
    dateRangeWidget->setVisible(false);

    // Notify on change
    connect(dateFrom, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));
    connect(dateTo, SIGNAL(dateChanged(QDate)), this, SLOT(dateRangeChanged()));

    return dateRangeWidget;
}

void TransactionView::dateRangeChanged()
{
    if (!transactionProxyModel)
        return;
    transactionProxyModel->setDateRange(
        QDateTime(dateFrom->date()),
        QDateTime(dateTo->date()).addDays(1));
}

void TransactionView::focusTransaction(const QModelIndex& idx)
{
    if (!transactionProxyModel)
        return;
    QModelIndex targetIdx = transactionProxyModel->mapFromSource(idx);
    transactionView->selectRow(targetIdx.row());
    computeSum();
    transactionView->scrollTo(targetIdx);
    transactionView->setCurrentIndex(targetIdx);
    transactionView->setFocus();
}

// We override the virtual resizeEvent of the QWidget to adjust tables column
// sizes as the tables width is proportional to the dialogs width.
void TransactionView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    columnResizingFixer->stretchColumnWidth(TransactionTableModel::ToAddress);
}

// Need to override default Ctrl+C action for amount as default behaviour is just to copy DisplayRole text
bool TransactionView::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_C && ke->modifiers().testFlag(Qt::ControlModifier)) {
            QModelIndex i = this->transactionView->currentIndex();
            if (i.isValid() && i.column() == TransactionTableModel::Amount) {
                GUIUtil::setClipboard(i.data(TransactionTableModel::FormattedAmountRole).toString());
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

// show/hide column Watch-only
void TransactionView::updateWatchOnlyColumn(bool fHaveWatchOnly)
{
    watchOnlyWidget->setVisible(fHaveWatchOnly);
    transactionView->setColumnHidden(TransactionTableModel::Watchonly, !fHaveWatchOnly);
}
