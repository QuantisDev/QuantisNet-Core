#!/usr/bin/env python3
# Copyright (c) 2019 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *
import logging

class CheckpointLoadTest(BitcoinTestFramework):
    def __init__(self):
        super().__init__()
        self.setup_clean_chain = True
        self.num_nodes = 3
        node_args = ["-keypool=10", "-debug=stake", "-debug=net",
                     "-addcheckpoint=10:abcdef01234456789", "-checkpoints=0"]
        self.extra_args = [node_args, node_args, node_args]
        self.node_args = node_args

    def setup_network(self, split=False):
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)
        connect_nodes_bi(self.nodes, 0, 1)
        connect_nodes_bi(self.nodes, 0, 2)
        connect_nodes_bi(self.nodes, 1, 2)
        self.is_network_split=False

    def run_test(self):
        self.sync_all()

        logging.info("Generating initial blockchain")
        self.nodes[0].generate(20)
        self.sync_all()
        assert_equal(self.nodes[0].getinfo()['blocks'], 20)
        
        logging.info("Enabling checkpoints")
        stop_nodes(self.nodes)
        node_args = list(self.node_args)
        node_args[-1] = "-checkpoints=1"
        self.extra_args[0] = node_args;
        self.setup_network()
        sync_blocks(self.nodes[1:])
        
        assert_equal(self.nodes[0].getinfo()['blocks'], 9)
        assert_equal(self.nodes[1].getinfo()['blocks'], 20)
        assert_equal(self.nodes[2].getinfo()['blocks'], 20)
        
        logging.info("Adding more blocks")
        self.nodes[1].generate(3)
        sync_blocks(self.nodes[1:])

        assert_equal(self.nodes[0].getinfo()['blocks'], 9)
        assert_equal(self.nodes[1].getinfo()['blocks'], 23)
        assert_equal(self.nodes[2].getinfo()['blocks'], 23)
        
        logging.info("Adding more block on alt chain")
        stop_nodes(self.nodes)
        self.extra_args[0] = self.node_args
        self.nodes = start_nodes(1, self.options.tmpdir, self.extra_args)
        self.nodes[0].generate(30)
        stop_nodes(self.nodes)
        self.setup_network()
        self.sync_all()

        assert_equal(self.nodes[0].getinfo()['blocks'], 39)
        assert_equal(self.nodes[1].getinfo()['blocks'], 39)
        assert_equal(self.nodes[2].getinfo()['blocks'], 39)

        logging.info("Restart to check no issues appear")
        stop_nodes(self.nodes)
        self.nodes = start_nodes(self.num_nodes, self.options.tmpdir, self.extra_args)
        self.sync_all()
        assert_equal(self.nodes[0].getinfo()['blocks'], 39)
        assert_equal(self.nodes[1].getinfo()['blocks'], 39)
        assert_equal(self.nodes[2].getinfo()['blocks'], 39)

if __name__ == '__main__':
    CheckpointLoadTest().main()
