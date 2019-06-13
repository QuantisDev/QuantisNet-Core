QuantisNet Core version 1.3.0 Release Notes
=======================================


QuantisNet Core version 1.3.0 is now available from:

  https://quantisnet.world/downloads

Please report bugs using the issue tracker at github:

  https://github.com/QuantisDev/QuantisNet-Core-v2.1.2/issues


How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), then depending on
your operation system:

* for Window run the installer
* for macos run the installer
* for Linux unpack to a separate folder and run from there


v1.3.0 changelog
----------------

Major changes:

- NEW: one time authentication code for dump- family of commands
    - Due to common scam approach, dump functions are additionally protected.
    - Users get a big fat warning in command help now.
    - The help includes a 6 hex digit authentication code which changes on every call.
    - dumpprivkey/dumphdinfo/dumpwallet require an extra parameter with the previous auth code.
- NEW: static linking of most dependencies for Linux
    - No need to install tons of runtime dependencies.
- NEW: windows 32-bit builds.
- CHANGED: to forcibly disable full DAG in UI
    > Solves long startup issue with not justified configuration `usedag=1`.
- CHANGED: updated to latest checkpoints

Minor changes:

- CHANGED: upgraded to newer versions of dependencies
    - Boost 1.69, Qt 5.9, OpenSSL 1.0.2q and others
- CHANGED: minor build system improvements
- CHANGED: enforced MN Sentinel spork 14 also in code
- FIXED: not to aggressively connect on lack of suitable peers
- FIXED: unknown version bit 1 warning
- NEW: added CentOS 7 build support
