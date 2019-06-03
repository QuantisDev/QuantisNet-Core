#!/usr/bin/env python3
# Copyright (c) 2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
import logging

class SporkCheckpointTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True

    def advance_time(self):
        # Spork override requires new time
        set_mocktime(get_mocktime() + 30)
        set_node_times(self.nodes, get_mocktime())        

    def run_test(self):
        self.nodes[0].generate(10)

        blockhash5 = self.nodes[0].getblockhash(5)
        blockhash7 = self.nodes[0].getblockhash(7)
        blockhash8 = self.nodes[0].getblockhash(8)

        sync_chain(self.nodes, timeout=10)

        logging.info("Issuing invalid checkpoint")
        self.nodes[0].spork('checkpoint', 7, '0123')
        sync_chain(self.nodes, timeout=10)
        assert_equal(self.nodes[1].getblockhash(5), blockhash5)
        assert_equal(self.nodes[1].getblockchaininfo()['headers'], 6)

        logging.info("Issuing valid checkpoint")
        self.advance_time()
        self.nodes[0].spork('checkpoint', 7, blockhash7)
        sync_chain(self.nodes, timeout=10)
        assert_equal(self.nodes[1].getblockhash(7), blockhash7)

        logging.info("Making alt chains")
        self.advance_time()
        self.nodes[0].invalidateblock(blockhash8)
        self.nodes[0].generate(5)
        sync_chain(self.nodes, timeout=10)

        self.advance_time()
        alt_blockhash8 = self.nodes[0].getblockhash(8)
        assert(alt_blockhash8 != blockhash8)
        self.nodes[1].invalidateblock(blockhash8)
        self.nodes[1].invalidateblock(alt_blockhash8)
        self.nodes[1].generate(7)
        sync_chain(self.nodes, timeout=10)
        alt2_blockhash8 = self.nodes[0].getblockhash(8)
        assert_equal(alt2_blockhash8, self.nodes[1].getblockhash(8))

        assert_equal(len(self.nodes[2].getchaintips()), 3)

        logging.info("Overriding alt chains")
        self.advance_time()
        self.nodes[0].spork('checkpoint', 8, blockhash8)
        sync_chain(self.nodes, timeout=10)
        assert_equal(blockhash8, self.nodes[1].getblockhash(8))


if __name__ == '__main__':
    SporkCheckpointTest().main()
