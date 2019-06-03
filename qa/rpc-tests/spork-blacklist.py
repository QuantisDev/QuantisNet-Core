#!/usr/bin/env python3
# Copyright (c) 2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
import logging

class SporkBlacklistTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True

    def advance_time(self):
        # Spork override requires new time
        set_mocktime(get_mocktime() + 30)
        set_node_times(self.nodes, get_mocktime())

    def run_test(self):
        self.nodes[0].generate(101)
        sync_chain(self.nodes, timeout=10)

        addr0 = self.nodes[0].getaccountaddress("")
        addr1 = self.nodes[1].getaccountaddress("")

        logging.info("Sending OK")
        self.nodes[0].sendtoaddress(addr1, 2)
        self.nodes[0].generate(1)

        logging.info("Issuing blacklist")
        sync_chain(self.nodes, timeout=10)
        self.nodes[1].sendtoaddress(addr0, Decimal("0.5"))
        self.nodes[1].spork('blacklist', get_mocktime(), addr1)

        logging.info("Failed send")
        assert_raises_jsonrpc(None, "Error: The transaction was rejected! Reason given: blacklisted-input",
                              self.nodes[1].sendtoaddress, addr0, Decimal("0.7"))
        self.nodes[0].sendtoaddress(addr1, 2)
        sync_mempools(self.nodes, timeout=10)
        self.nodes[0].generate(1)

        sync_chain(self.nodes, timeout=10)
        assert_equal(self.nodes[1].getbalance(), 4)

        logging.info("Disabling blacklist")
        self.advance_time()
        self.nodes[1].spork('blacklist', get_mocktime()+1, addr1)
        self.nodes[1].sendtoaddress(addr0, 1)
        sync_mempools(self.nodes, timeout=10)
        self.nodes[0].generate(1)

        sync_chain(self.nodes, timeout=10)
        assert_greater_than(3, self.nodes[1].getbalance())


if __name__ == '__main__':
    SporkBlacklistTest().main()
