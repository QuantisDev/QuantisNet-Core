QuantisNet Core version 2.0.1 Release Notes
=======================================

QuantisNet Core version 2.0.1 is now available from:

  https://quantisnet.world/downloads

Please report bugs using the issue tracker at github:

  https://github.com/QuantisDev/QuantisNet-Core/issues


How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then depending on
your operation system:

* for Window run the installer
* for macos run the installer
* for Linux unpack to a separate folder and run from there

**NOTE**: version of QuantisNet Core prior to v2.0.0 must not be used with the same data files.


v2.0.1 changelog
----------------

Major changes:

* FIXED: reindex requirement, if blockchain data contains chains violating checkpoints
* NEW: auto-invalidation of chains violating checkpoints


Minor changes:

* FIXED: to obey -checkpoints=0 on startup validation
* NEW: custom -addcheckpoint option
