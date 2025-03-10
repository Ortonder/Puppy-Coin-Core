// Copyright (c) 2017-2018 The Puppycoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_BIP38DIALOG_H
#define BITCOIN_QT_BIP38DIALOG_H

#include <QDialog>

class WalletModel;

namespace Ui
{
class Bip38ToolDialog;
}

class Bip38ToolDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Bip38ToolDialog(QWidget* parent);
    ~Bip38ToolDialog();

    void setModel(WalletModel* model);
    void setAddress_ENC(const QString& address);
    void setAddress_DEC(const QString& address);

    void showTab_ENC(bool fShow);
    void showTab_DEC(bool fShow);

protected:
    bool eventFilter(QObject* object, QEvent* event);

private:
    Ui::Bip38ToolDialog* ui;
    WalletModel* model;

private slots:
    /* encrypt key */
    void on_addressBookButton_ENC_clicked();
    void on_pasteButton_ENC_clicked();
    void on_encryptKeyButton_ENC_clicked();
    void on_copyKeyButton_ENC_clicked();
    void on_clearButton_ENC_clicked();
    /* decrypt key */
    void on_pasteButton_DEC_clicked();
    void on_decryptKeyButton_DEC_clicked();
    void on_importAddressButton_DEC_clicked();
    void on_clearButton_DEC_clicked();
};

#endif // BITCOIN_QT_BIP38TOOLDIALOG_H
