# QuantisNet (QUAN) Information
![QuantisNet](https://cdn.discordapp.com/attachments/436649409535541248/528345762795487240/321x81.png)

| [**Whitepaper**](https://quantisnetwork.org/wp-content/uploads/2019/01/Quantis-WP.pdf) | [**Website**](https://quantisnetwork.org/) | [**Discord**](https://discord.gg/cFYF57h) | [**Telegram**](https://t.me/quantis) |
# QuantisNet Core

#### https://www.QuantisNetwork.org/


What is QuantisNet?
----------------
For more information, as well as an immediately useable, binary version of
the QuantisNet Core software, see https://www.QuantisNetwork.org/ .

License
-------

QuantisNet Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is meant to be stable. Development is normally done in separate branches.
[Tags]( https://github.com/QuantisDev ) are created to indicate new official,
stable release versions of QuantisNet Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/qa) of the RPC interface, written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/qa) are installed) with: `qa/pull-tester/rpc-tests.py`

The Travis CI system makes sure that every pull request is built for Windows, Linux, and OS X, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.
