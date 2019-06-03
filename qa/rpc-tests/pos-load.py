#!/usr/bin/env python3
# Copyright (c) 2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
import logging

class PoSSyncTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 3
        node_args = ["-keypool=10", "-debug=stake", "-debug=net",
                     "-checklevel=4", "-checkblocks=6"]
        self.extra_args = [node_args, node_args, node_args, node_args]

    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)
        connect_nodes_bi(self.nodes, 0, 1)
        connect_nodes_bi(self.nodes, 0, 2)
        connect_nodes_bi(self.nodes, 1, 2)
        self.is_network_split=False
        self.sync_all()

    def run_test(self):
        self.nodes[0].spork('SPORK_15_FIRST_POS_BLOCK', 103)

        logging.info("Generating initial blockchain")
        self.nodes[0].generate(100)
        self.sync_all()
        self.nodes[1].generate(1)
        self.sync_all()
        self.nodes[2].generate(1)
        self.sync_all()

        assert_equal(self.nodes[0].getbalance(), 2*MINER_REWARD_DEC)
        assert_equal(self.nodes[1].getbalance(), 0)
        assert_equal(self.nodes[2].getbalance(), 0)
        
        logging.info("Switching to PoS")
        self.sync_all()
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], False)
        self.nodes[0].generate(1, 10)
        self.sync_all()
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], True)
        
        logging.info("Reloading with only the last PoS block")
        stop_nodes(self.nodes)
        self.setup_network()
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], True)
        assert_equal(self.nodes[0].getblockcount(), 103)


if __name__ == '__main__':
    PoSSyncTest().main()
