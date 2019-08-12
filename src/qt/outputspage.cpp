// Copyright (c) 2018-2019 The Phore developers
// Copyright (c) 2018-2019 The Curium developers
// Copyright (c) 2018-2019 The ZENZO developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/quantisnet-config.h"
#endif

#include "outputspage.h"
#include "ui_outputspage.h"

#include "activemasternode.h"
#include "bitcoingui.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "masternode-payments.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "wallet/wallet.h"

#include <QIcon>
#include <QMenu>
#include <QString>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <boost/tokenizer.hpp>
#include <fstream>

OutPutsPage::OutPutsPage(QWidget* parent) : QDialog(parent),
                                            ui(new Ui::OutPutsPage)
{
    ui->setupUi(this);

	key = OutPutsPage::getmasternodeoutputs();
	ui->outPutsWidget->append(key);

    // Build context menu
    contextMenu = new QMenu();
    contextMenu->addSeparator();


    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}



OutPutsPage::~OutPutsPage()
{
    delete ui;
}

QString OutPutsPage::getmasternodeoutputs ()
{
    // Find possible candidates
    std::vector<COutput> possibleCoins;
	pwalletMain->AvailableCoins(possibleCoins, true, NULL, false, ONLY_MN_COLLATERAL);

	std::string strPrint = "QuantisNet Masternode Outputs\n";

    BOOST_FOREACH (COutput& out, possibleCoins) {
        strPrint += out.tx->GetHash().ToString() + " " + std::to_string(out.i) + "\n";
    }

    return QString::fromStdString(strPrint);
}