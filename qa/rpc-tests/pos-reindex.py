#!/usr/bin/env python3
# Copyright (c) 2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
import logging

class PoSReindexTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 1
        node_args = ["-keypool=10", "-debug",
                     "-checklevel=4", "-checkblocks=6", "-reindex"]
        self.extra_args = [node_args]

    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)

    def run_test(self):
        self.nodes[0].spork('SPORK_15_FIRST_POS_BLOCK', 103)

        logging.info("Generating initial blockchain")
        self.nodes[0].generate(100)
        for i in range(5):
            self.nodes[0].generate(10)
            set_mocktime(get_mocktime() + 5*30)
            set_node_times(self.nodes, get_mocktime())
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], True)
        assert_equal(self.nodes[0].getblockcount(), 150)
        
        logging.info("Reindex")
        stop_nodes(self.nodes)
        self.setup_network()
        to_sleep = 0
        while self.nodes[0].getblockcount() < 103:
            to_sleep += 0.1
            time.sleep(0.1)
        time.sleep(to_sleep)
        assert_equal(self.nodes[0].getblockcount(), 150)
        assert_equal(self.nodes[0].spork('show')['SPORK_15_FIRST_POS_BLOCK'], 999999)
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], True)

        logging.info("Continue PoS chain")
        self.nodes[0].generate(10)
        assert_equal(self.nodes[0].getblockcount(), 160)

if __name__ == '__main__':
    PoSReindexTest().main()
