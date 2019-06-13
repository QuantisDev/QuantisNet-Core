QuantisNet Core version 1.2.0 Release Notes
=======================================


QuantisNet Core version 1.2.0 is now available from:

  https://quantisnet.world/downloads

Please report bugs using the issue tracker at github:

  https://github.com/QuantisDev/QuantisNet-Core-v2.1.2/issues


How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then depending on
your operation system:

* for Window run the installer
* for macos run the installer or just copy /Applications/QuantisNet-Qt
* for Linux unpack to a separate folder and run from there


v1.2.0 changelog
----------------

Major changes:

- NEW: much fast peer connectivity on startup
- NEW: hardcoded dnsseed addresses
- NEW: BIP44/SLIP44 HD wallet code 9797 for newly generated wallets
- NEW: updated list of checkpoints
- FIXED: IPv6-related connectivity issues with IPv4-only internet connection
- FIXED: growing memory usage on long run due to not unloading old DAG
- FIXED: improved UI performance of wallet with large number of transactions
- FIXED: Spork 8 Master Payment Enforcement value

Minor changes:

- FIXED: version fee estimation file
- FIXED: getblocktemplate result for backbone "payee" on SuperBlock
- FIXED: QUAN branding issues
- NEW: enforced strict warning checking on build
- NEW: created easily reproducible development environment
