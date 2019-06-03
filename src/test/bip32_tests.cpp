// Copyright (c) 2017-2019 The QuantisNet Core developers
// Copyright (c) 2013-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include "base58.h"
#include "key.h"
#include "uint256.h"
#include "util.h"
#include "utilstrencodings.h"
#include "test/test_quantisnet.h"

#include <string>
#include <vector>

struct TestDerivation {
    std::string pub;
    std::string prv;
    unsigned int nChild;
};

struct TestVector {
    std::string strHexMaster;
    std::vector<TestDerivation> vDerive;

    TestVector(std::string strHexMasterIn) : strHexMaster(strHexMasterIn) {}

    TestVector& operator()(std::string pub, std::string prv, unsigned int nChild) {
        vDerive.push_back(TestDerivation());
        TestDerivation &der = vDerive.back();
        der.pub = pub;
        der.prv = prv;
        der.nChild = nChild;
        return *this;
    }
};

TestVector test1 =
  TestVector("000102030405060708090a0b0c0d0e0f")
    ("npub2qgivhE1LcAokvcW6W1JDHEp7ah4vZH5EtDRksCpeAMaJtoh4h42AuTJLxZNTBCdEQ2qWfxE92b43QiETh4t7idLziwdd74m6qsZLYaCHsM",
     "nprvGnNBAhy3NGmY3qWK2rKis7eg9evuPRjw18KsgU62SNkSEHByuheRraB2adBAhb5UinFoWAk5PeeccgrwkTJ4AxP6WvK2MrH3NpZcZyVTHFVg",
     0x80000000)
    ("npub2sx8vQXURsHnuXkDJFsw6KLvSQFRJdm5vVeKixawcMYrgB6sUvb7J3oi8E85CV7AEcKQNuVesff91ogNQioGBc65CMjtYQxycn17eAJZMsz",
     "nprvGnQSahgLqN2f2z7Sk45bVzgnFykTjopR1owJaSBQZLwdWeUH67sxwhKNzQSX2znrgVJhCY3QG31TVQcyU6tLMkTB2zBhW6EkGXaFRQdrMeMV",
     1)
    ("npub2v8G8C5MpaArjynf2Wmoh4EaVnEXJfVNrpKAKEMkYzM4uic1Cj1RFchwYJaecRpWCKwTAKwkzG9cc7RBt3y9sFv1VZak23yFFWXtw7QM8XR",
     "nprvGnSchuTtikjY6pZVBnLVNbRfv38Sqor9JkFyR2TBNHaRit1nDqgPFetHDpVi6bidiwPStLrzh7iQuYZVhEE8LXcfUVxfE9zX8CNZnwYoLx2f",
     0x80000002)
    ("npub2xjYAiuDXT2Gc9akH7BB44Giiw73aD5NCMYqZZeuiriWKZo81UqrA4Fs6KmV4JFpYr3BmhvNNCib4SZUb9Wgvi4r5TYt712vyCT32P4qJbt",
     "nprvGnVDywziaTcPWgjHH2vtjxRi4GHKN5PjJ5oD6GnUXTSoAHryLeSDgZKq9NXviZx9DSrUGwKwcY8EommESpF3Xei78raDnJvm4oeK1aCAJFJw",
     2)
    ("npub2zxw1A2AhvhFgd4TyNPFwZS2okPnCoPZdqS8pXBP3JH7gfRV5FYDMdinmeksU6QZ2AAMuiQKQwchcruUgSwteJY6zF74wBvkytMtAwAG2b8",
     "nprvGnXTNnRqXe64VmCkzjC6pqvsNM6c6hz3VXH6PXjzzmtMmexbhiCv3kuJ53rLB2cYeWebyZFBBSwgyEx8u1fasr6hBFpJss1amxcnB5i1axyS",
     1000000000)
    ("npub32ghUqdQq45TCxqv5uwzFGS2byy62JCbS5ph4oZXFNfcPnpsbfnscjsrQUfpKMVGVrSuR4L7x9KJDHJVnJSjVZhDrnX9jfWPnqyoR619ww7",
     "nprvGnZB9G7SmmDShHYYSqjfZ9dsN9LBQXUrXKXUwn2P8yxkGN616EdAi21T8gi7NwXfj841ELcA8HzE6F89GYjbby4LibK56yvLVExLXpqMhnMp",
     0);

TestVector test2 =
  TestVector("fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8a8784817e7b7875726f6c696663605d5a5754514e4b484542")
    ("npub2qgivhE1LcAokY7qCf5afUxaMmqVcBXtvU1A3owecYUSbsptEqpxbocth4eMARZb8xGdRLcPY76JmCSTasz3FddHrf96TbZRRDYfcQwvekW",
     "nprvGnNBAhy3NGmY3q7pMxUo9ZrPuu83p7NBpoufQm2mGM8Z6aB16soCo15CAyEwkmb2p2JYvv2XqvkeonPfrViSsZkRHNj6bzcx7PpEoLTh3PQx",
     0)
    ("npub2txUCQuxpcztGoi8LSTTjHN6ijakKA2BUdkXDcy6gxLQHrGrp7B2keC9GXcExWa2MGkoHZc8wBwGvMs9U2gTbEHa8hsmMwhoYhfL3g9Zqev",
     "nprvGnRSuygjKknN8MPQf6GB2deoSG5o4pLg7N5QmvqniRYR4G9T5T4Ys9umRYkDZQeseRxnkp2waMv4w8uJg3zwvVyagNuWfP77rXE9MhjKFzsZ",
     0xFFFFFFFF)
    ("npub2v7XT1wVCXAzSfvJktT7Cd3f7UoereBsZnq491ssLh6nk9WKRzbLxhyjsurMiFB88ngLwQTkEQMsb2jVEwAaKNkCfnVpifxpR5vvtQYSxBb",
     "nprvGnSbyEHkr8gYEXFcqWiAg6zUzeq1yMpqoTEVJrEhV5HBSiSgY4wyBMyZ2A6hY8uc8LC7eb49DQva3a93SnNZ45H2Eodk19cr7K1UtUAQX9wT",
     1)
    ("npub2xvVs2vRNAUBYHbSpYbgAZytT1g9xq78tEKuz5YCeaTEQyNUjW83rcN7ii3VW7sQiy8tn3YysXfhfpzW9qbXaiD7G4hfwDQ2Lrbt8vBTsqH",
     "nprvGnVQweJjnJKqRcsHyaNKF4wRDzMtUU1m4mfzAhJMpPAXtPGYhNTVtFswPztuAAcope9UMLMDbpUFFtqpbqG5S5kqXwQGozipVnoucwqufDoQ",
     0xFFFFFFFE)
    ("npub2z6XmzrmyZRtqEq3Z2RDY5g5Mk9X1ojduw3oWoEcVLiUsKhQqB44P8jDMNAzhVPrgQnXaYcgv8zZKU9tqii3J4uZXy2AVWVSD5gTfncVMm2",
     "nprvGnWayZGg8uio8upXaJr8nST7Qu6MqWzPZoNi4E24EDvo8qcsdU8RtnQJVdbdYo3C7Bim3eAzbCqpqhxuyMCyvmbWhmLp88vr88Pv91dQcq8u",
     2)
    ("npub31TZjS5HVjk8aW1wH6YuWq43kogdG8eia6fgvab788a4wX2yzoypZhgDRmFGq4kZCTzkrU86or1YouDPfpPwcKm9B8rFRsnAtNseLV7kDqU",
     "nprvGnXx1WhteRu7Nf5iU2vGURCVPJ9twmKJeTYKwdoQirieiupDCdmMexyFVi1JYx4HBE2cmvvdQewfXkWoV5XM9paFRaCKdCLyqpmFh69j6F8k",
     0);

void RunTest(const TestVector &test) {
    std::vector<unsigned char> seed = ParseHex(test.strHexMaster);
    CExtKey key;
    CExtPubKey pubkey;
    key.SetMaster(&seed[0], seed.size());
    pubkey = key.Neuter();
    BOOST_FOREACH(const TestDerivation &derive, test.vDerive) {
        unsigned char data[74];
        key.Encode(data);
        pubkey.Encode(data);

        // Test private key
        CBitcoinExtKey b58key; b58key.SetKey(key);
        BOOST_CHECK(b58key.ToString() == derive.prv);

        CBitcoinExtKey b58keyDecodeCheck(derive.prv);
        CExtKey checkKey = b58keyDecodeCheck.GetKey();
        assert(checkKey == key); //ensure a base58 decoded key also matches

        // Test public key
        CBitcoinExtPubKey b58pubkey; b58pubkey.SetKey(pubkey);
        BOOST_CHECK(b58pubkey.ToString() == derive.pub);

        CBitcoinExtPubKey b58PubkeyDecodeCheck(derive.pub);
        CExtPubKey checkPubKey = b58PubkeyDecodeCheck.GetKey();
        assert(checkPubKey == pubkey); //ensure a base58 decoded pubkey also matches

        // Derive new keys
        CExtKey keyNew;
        BOOST_CHECK(key.Derive(keyNew, derive.nChild));
        CExtPubKey pubkeyNew = keyNew.Neuter();
        if (!(derive.nChild & 0x80000000)) {
            // Compare with public derivation
            CExtPubKey pubkeyNew2;
            BOOST_CHECK(pubkey.Derive(pubkeyNew2, derive.nChild));
            BOOST_CHECK(pubkeyNew == pubkeyNew2);
        }
        key = keyNew;
        pubkey = pubkeyNew;

        CDataStream ssPub(SER_DISK, CLIENT_VERSION);
        ssPub << pubkeyNew;
        BOOST_CHECK(ssPub.size() == 75);

        CDataStream ssPriv(SER_DISK, CLIENT_VERSION);
        ssPriv << keyNew;
        BOOST_CHECK(ssPriv.size() == 75);

        CExtPubKey pubCheck;
        CExtKey privCheck;
        ssPub >> pubCheck;
        ssPriv >> privCheck;

        BOOST_CHECK(pubCheck == pubkeyNew);
        BOOST_CHECK(privCheck == keyNew);
    }
}

BOOST_FIXTURE_TEST_SUITE(bip32_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(bip32_test1) {
    RunTest(test1);
}

BOOST_AUTO_TEST_CASE(bip32_test2) {
    RunTest(test2);
}

BOOST_AUTO_TEST_SUITE_END()
