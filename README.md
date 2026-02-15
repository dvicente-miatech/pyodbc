# pyodbc

[![Windows build](https://ci.appveyor.com/api/projects/status/github/mkleehammer/pyodbc?branch=master&svg=true&passingText=Windows%20build&failingText=Windows%20build)](https://ci.appveyor.com/project/mkleehammer/pyodbc)
[![Ubuntu build](https://github.com/mkleehammer/pyodbc/actions/workflows/ubuntu_build.yml/badge.svg)](https://github.com/mkleehammer/pyodbc/actions/workflows/ubuntu_build.yml)
[![PyPI](https://img.shields.io/pypi/v/pyodbc?color=brightgreen)](https://pypi.org/project/pyodbc/)

pyodbc is an open source Python module that makes accessing ODBC databases simple.  It
implements the [DB API 2.0](https://www.python.org/dev/peps/pep-0249) specification but is packed with even more Pythonic convenience.

The easiest way to install pyodbc is to use pip:

    python -m pip install pyodbc-mi

On Macs, you should probably install unixODBC first if you don't already have an ODBC
driver manager installed.  For example, using the [homebrew](https://brew.sh/) package manager:

    brew install unixodbc
    python -m pip install pyodbc-mi

Similarly, on Unix you should make sure you have an ODBC driver manager installed before
installing pyodbc.  See the [docs](https://github.com/mkleehammer/pyodbc/wiki/Install)
for more information about how to do this on different Unix flavors.  (On Windows, the
ODBC driver manager is built-in.)

Precompiled binary wheels are provided for multiple Python versions on most Windows, macOS,
and Linux platforms.  On other platforms pyodbc will be built from the source code.  Note,
pyodbc contains C++ extensions so you will need a suitable C++ compiler when building from
source.  See the [docs](https://github.com/mkleehammer/pyodbc/wiki/Install) for details.

[Documentation](https://github.com/dvicente-miatech/pyodbc)

[Release Notes](https://github.com/dvicente-miatech/pyodbc/releases)

## Examples

### Calling Stored Procedures

You can execute stored procedures using the `call_proc` method:

```python
import pyodbc

# Connect to database
cnxn = pyodbc.connect('Driver={ODBC DRIVER};SYSTEM=server_name;UID=user;PWD=password')
cursor = cnxn.cursor()

# Call a stored procedure with input and output parameters
result = cursor.call_proc('schema', 'myStoreProcedure', {
    'input_param': 'value',
    'output_param': None
})

# Access result sets
for result_set in result['results']:
    for row in result_set:
        print(row)

# Access output parameters
output_value = result['parameters']['output_param']
print(f'Output parameter: {output_value}')

cnxn.close()
```

The `call_proc` method returns a dictionary with:
- `results`: List of result sets returned by the procedure
- `parameters`: Dictionary of output and input/output parameter values
