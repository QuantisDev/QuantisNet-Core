// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2011-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_TRANSACTIONFILTERPROXY_H
#define BITCOIN_QT_TRANSACTIONFILTERPROXY_H

#include "amount.h"
#include "transactionrecord.h"

#include <QDateTime>
#include <QSortFilterProxyModel>

// Workaround "called in a constant expression before its definition is complete"
struct TransactionFilterProxyUtil {
    static constexpr inline quint32 TYPE(int type) { return 1<<type; }
};

/** Filter the transaction list according to pre-specified rules. */
class TransactionFilterProxy : public QSortFilterProxyModel, public TransactionFilterProxyUtil
{
    Q_OBJECT

public:
    explicit TransactionFilterProxy(QObject *parent = 0);

    /** Earliest date that can be represented (far in the past) */
    static const QDateTime MIN_DATE;
    /** Last date that can be represented (far in the future) */
    static const QDateTime MAX_DATE;
    /** Type filter bit field (all types) */
    static constexpr quint32 ALL_TYPES = 0xFFFFFFFF;
    /** Type filter bit field (all types but Darksend-SPAM) */
    static constexpr quint32 COMMON_TYPES = (
        ALL_TYPES
        ^ TYPE(TransactionRecord::PrivateSendDenominate)
        ^ TYPE(TransactionRecord::PrivateSendCollateralPayment)
        ^ TYPE(TransactionRecord::PrivateSendMakeCollaterals)
        ^ TYPE(TransactionRecord::PrivateSendCreateDenominations)
    );

    enum WatchOnlyFilter
    {
        WatchOnlyFilter_All,
        WatchOnlyFilter_Yes,
        WatchOnlyFilter_No
    };

    void setDateRange(const QDateTime &from, const QDateTime &to);
    void setAddressPrefix(const QString &addrPrefix);
    /**
      @note Type filter takes a bit field created with TYPE() or ALL_TYPES
     */
    void setTypeFilter(quint32 modes);
    void setMinAmount(const CAmount& minimum);
    void setWatchOnlyFilter(WatchOnlyFilter filter);

    /** Set maximum number of rows returned, -1 if unlimited. */
    void setLimit(int limit);

    /** Set whether to show conflicted transactions. */
    void setShowInactive(bool showInactive);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private:
    QDateTime dateFrom;
    QDateTime dateTo;
    QString addrPrefix;
    quint32 typeFilter;
    WatchOnlyFilter watchOnlyFilter;
    CAmount minAmount;
    int limitRows;
    bool showInactive;
};

#endif // BITCOIN_QT_TRANSACTIONFILTERPROXY_H
