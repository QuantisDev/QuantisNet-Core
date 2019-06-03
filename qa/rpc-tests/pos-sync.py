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
        self.num_nodes = 4
        node_args = ["-keypool=10", "-debug=stake", "-debug=net"]
        self.extra_args = [node_args, node_args, node_args, node_args]

    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)
        connect_nodes_bi(self.nodes, 0, 1)
        connect_nodes_bi(self.nodes, 0, 2)
        connect_nodes_bi(self.nodes, 1, 2)
        self.is_network_split=False
        sync_blocks(self.nodes[:3])
        self.nodes[0].spork('SPORK_15_FIRST_POS_BLOCK', 103)

    def run_test(self):
        logging.info("Generating initial blockchain")
        self.nodes[0].generate(100)
        sync_blocks(self.nodes[:3])
        self.nodes[1].generate(1)
        sync_blocks(self.nodes[:3])
        self.nodes[2].generate(1)
        sync_blocks(self.nodes[:3])

        assert_equal(self.nodes[0].getbalance(), 2*MINER_REWARD_DEC)
        assert_equal(self.nodes[1].getbalance(), 0)
        assert_equal(self.nodes[2].getbalance(), 0)
        assert_equal(self.nodes[3].getbalance(), 0)
        
        logging.info("Switching to PoS")
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], False)
        self.nodes[0].generate(1, 10)
        sync_blocks(self.nodes[:3])
        assert_equal(self.nodes[0].getblockchaininfo()['pos'], True)
        assert_equal(self.nodes[3].getblockchaininfo()['pos'], False)
        
        logging.info("Adding more PoS blocks")
        for i in range(1000):
            set_node_times(self.nodes, GENESISTIME + i*180)
            assert_equal(len(self.nodes[0].generate(5, 50)), 5)

        return
        logging.info("Syncing from scratch")
        connect_nodes_bi(self.nodes, 3, 0)
        connect_nodes_bi(self.nodes, 3, 1)
        connect_nodes_bi(self.nodes, 3, 2)
        self.sync_all()
        assert_equal(self.nodes[3].getblockchaininfo()['pos'], True)
        assert_equal(self.nodes[0].getinfo()['blocks'], 5103)
        assert_equal(self.nodes[3].getinfo()['blocks'], 5103)


if __name__ == '__main__':
    PoSSyncTest().main()
