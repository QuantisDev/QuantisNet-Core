#!/usr/bin/env python3
# Copyright (c) 2015-2018 The QuantisNet Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
# blocktools.py - utilities for manipulating blocks and transactions
# Copyright (c) 2015-2016 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

from .mininode import *
from .script import CScript, OP_TRUE, OP_CHECKSIG, OP_HASH160, OP_EQUALVERIFY, OP_DUP
from binascii import unhexlify, hexlify

# Create a block (with regtest difficulty)
def create_block(hashprev, coinbase, nTime=None, nHeight=None):
    block = CBlock()
    if nTime is None:
        import time
        block.nTime = int(time.time()+600)
    else:
        block.nTime = nTime
    block.hashPrevBlock = hashprev
    block.nBits = 0x207fffff # Will break after a difficulty adjustment...
    block.vtx.append(coinbase)
    block.hashMerkleRoot = block.calc_merkle_root()
    
    if nHeight is None:
        block.nHeight = coinbase.nHeightWorkaround
    else :
        block.nHeight = nHeight
    
    block.calc_sha256()
    return block

def serialize_script_num(value):
    r = bytearray(0)
    if value == 0:
        return r
    neg = value < 0
    absvalue = -value if neg else value
    while (absvalue):
        r.append(int(absvalue & 0xff))
        absvalue >>= 8
    if r[-1] & 0x80:
        r.append(0x80 if neg else 0)
    elif neg:
        r[-1] |= 0x80
    return r

# Create a coinbase transaction, assuming no miner fees.
# If pubkey is passed in, the coinbase output will be a P2PK output;
# otherwise an anyone-can-spend output.
def create_coinbase(height, pubkey = None):
    mnPaymentStart = 240
    superBlockCycle = 60
    
    coinbase = CTransaction()
    coinbase.vin.append(CTxIn(COutPoint(0, 0xffffffff), 
                ser_string(serialize_script_num(height)), 0xffffffff))
    
    backboneoutput = CTxOut()
    backboneoutput.nValue = 2280 * MCOIN
    backboneoutput.scriptPubKey = CScript([
        OP_DUP,
        OP_HASH160,
        unhexlify("b506a5b17506bab7a7e68ee557046d64a01a6f0d"),
        OP_EQUALVERIFY,
        OP_CHECKSIG])
    
    coinbaseoutput = CTxOut()
    coinbaseoutput.nValue = 2280 * MCOIN

    if (pubkey != None):
        coinbaseoutput.scriptPubKey = CScript([pubkey, OP_CHECKSIG])
    else:
        coinbaseoutput.scriptPubKey = CScript([OP_TRUE])
                
    coinbase.vout = [ coinbaseoutput, backboneoutput ]
    
    if height >= mnPaymentStart:
        pass
    
    if (height >= superBlockCycle) and (height % superBlockCycle) == 0:
        pass
    
    coinbase.calc_sha256()
    coinbase.nHeightWorkaround = height
    return coinbase

# Create a transaction.
# If the scriptPubKey is not specified, make it anyone-can-spend.
def create_transaction(prevtx, n, sig, value, scriptPubKey=CScript()):
    tx = CTransaction()
    assert(n < len(prevtx.vout))
    tx.vin.append(CTxIn(COutPoint(prevtx.sha256, n), sig, 0xffffffff))
    tx.vout.append(CTxOut(value, scriptPubKey))
    tx.calc_sha256()
    return tx

def get_legacy_sigopcount_block(block, fAccurate=True):
    count = 0
    for tx in block.vtx:
        count += get_legacy_sigopcount_tx(tx, fAccurate)
    return count

def get_legacy_sigopcount_tx(tx, fAccurate=True):
    count = 0
    for i in tx.vout:
        count += i.scriptPubKey.GetSigOpCount(fAccurate)
    for j in tx.vin:
        # scriptSig might be of type bytes, so convert to CScript for the moment
        count += CScript(j.scriptSig).GetSigOpCount(fAccurate)
    return count
