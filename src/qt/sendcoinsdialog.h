// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2017-2018 The Puppycoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_SENDCOINSDIALOG_H
#define BITCOIN_QT_SENDCOINSDIALOG_H

#include "walletmodel.h"

#include <QDialog>
#include <QString>

static const int MAX_SEND_POPUP_ENTRIES = 10;

class ClientModel;
class OptionsModel;
class SendCoinsEntry;
class SendCoinsRecipient;

namespace Ui
{
class SendCoinsDialog;
}

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Dialog for sending bitcoins */
class SendCoinsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendCoinsDialog(QWidget* parent = 0);
    ~SendCoinsDialog();

    void setClientModel(ClientModel* clientModel);
    void setModel(WalletModel* model);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue https://bugreports.qt-project.org/browse/QTBUG-10907).
     */
    QWidget* setupTabChain(QWidget* prev);

    void setAddress(const QString& address);
    void pasteEntry(const SendCoinsRecipient& rv);
    bool handlePaymentRequest(const SendCoinsRecipient& recipient);
    bool fSplitBlock;

public slots:
    void clear();
    void reject();
    void accept();
    SendCoinsEntry* addEntry();
    void updateTabsAndLabels();
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance, 
                    const CAmount& zerocoinBalance, const CAmount& unconfirmedZerocoinBalance, const CAmount& immatureZerocoinBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);

private:
    Ui::SendCoinsDialog* ui;
    ClientModel* clientModel;
    WalletModel* model;
    bool fNewRecipientAllowed;
    void send(QList<SendCoinsRecipient> recipients, QString strFee, QStringList formatted);
    bool fFeeMinimized;

    // Process WalletModel::SendCoinsReturn and generate a pair consisting
    // of a message and message flags for use in emit message().
    // Additional parameter msgArg can be used via .arg(msgArg).
    void processSendCoinsReturn(const WalletModel::SendCoinsReturn& sendCoinsReturn, const QString& msgArg = QString(), bool fPrepare = false);
    void minimizeFeeSection(bool fMinimize);
    void updateFeeMinimizedLabel();

private slots:
    void on_sendButton_clicked();
    void on_buttonChooseFee_clicked();
    void on_buttonMinimizeFee_clicked();
    void removeEntry(SendCoinsEntry* entry);
    void updateDisplayUnit();
    void updateSwiftTX();
    void coinControlFeatureChanged(bool);
    void coinControlButtonClicked();
    void coinControlChangeChecked(int);
    void coinControlChangeEdited(const QString&);
    void coinControlUpdateLabels();
    void coinControlClipboardQuantity();
    void coinControlClipboardAmount();
    void coinControlClipboardFee();
    void coinControlClipboardAfterFee();
    void coinControlClipboardBytes();
    void coinControlClipboardPriority();
    void coinControlClipboardLowOutput();
    void coinControlClipboardChange();
    void splitBlockChecked(int);
    void splitBlockLineEditChanged(const QString& text);
    void setMinimumFee();
    void updateFeeSectionControls();
    void updateMinFeeLabel();
    void updateSmartFeeLabel();
    void updateGlobalFeeVariables();

signals:
    // Fired when a message should be reported to the user
    void message(const QString& title, const QString& message, unsigned int style);
};

#endif // BITCOIN_QT_SENDCOINSDIALOG_H
