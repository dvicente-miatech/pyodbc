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

### Batch Execution with `executebatch`

`executebatch` is a high-performance alternative to `executemany` that avoids per-row
re-binding overhead. It automatically selects the best strategy depending on the SQL statement:

- **`INSERT ... VALUES (?, ...)`** — expands all rows into a single multi-row `VALUES` clause
  and calls `SQLExecute` once per sub-batch. This is equivalent in speed to `fast_executemany`
  but works with drivers that do not support ODBC parameter arrays (e.g. IBM i Access ODBC).
- **`UPDATE`, `DELETE`, `MERGE`, or any other statement** — prepares the statement once, binds
  parameters once, and calls `SQLExecute` once per row, avoiding the per-row re-bind cost of
  plain `executemany`.

```python
import pyodbc

cnxn = pyodbc.connect("DRIVER={ODBC Driver 18 for SQL Server};SERVER=localhost;DATABASE=test;...")
cursor = cnxn.cursor()

# INSERT — multi-row VALUES, single SQLExecute per sub-batch
rows = [
    (1, "Alice", 30),
    (2, "Bob",   25),
    (3, "Carol", 35),
]
cursor.executebatch(
    "INSERT INTO employees (id, name, age) VALUES (?, ?, ?)",
    rows
)
cnxn.commit()

# UPDATE — prepare-once / execute-N (no per-row re-bind)
updates = [
    (31, 1),
    (26, 2),
]
cursor.executebatch(
    "UPDATE employees SET age = ? WHERE id = ?",
    updates
)
cnxn.commit()

cnxn.close()
```

#### Comparison with `executemany` and `fast_executemany`

| Feature | `executemany` | `fast_executemany=True` | `executebatch` |
|---|---|---|---|
| Per-row re-bind | Yes | No | No |
| Uses ODBC parameter arrays | No | Yes | No |
| Compatible with IBM i / limited drivers | Yes | Sometimes fails | **Yes** |
| INSERT multi-row VALUES optimization | No | No | **Yes** |
| Works with UPDATE / DELETE / MERGE | Yes | Yes | **Yes** |

Use `executebatch` when:
- You need bulk INSERT performance without enabling `fast_executemany`
- Your ODBC driver does not support `SQL_ATTR_PARAMSET_SIZE` (e.g. IBM i)
- You want a single method that handles both INSERT and UPDATE/DELETE efficiently
