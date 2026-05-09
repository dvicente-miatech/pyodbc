
#ifndef PARAMS_H
#define PARAMS_H

bool Params_init();

struct Cursor;

bool PrepareAndBind(Cursor* cur, PyObject* pSql, PyObject* params, bool skip_first);

// Returns:  1 = success
//           0 = error (Python exception already set)
//          -1 = driver does not support parameter arrays; caller should fall back to ExecuteMultiFallback
int ExecuteMulti(Cursor* cur, PyObject* pSql, PyObject* paramArrayObj);

// Fallback for drivers that do not support ODBC parameter arrays (e.g. IBM i Access ODBC).
// Prepares the statement once, binds parameters once, then calls SQLExecute once per row
// updating the already-bound buffers in place.  Much faster than the default row-by-row
// path which re-binds on every iteration.
bool ExecuteMultiFallback(Cursor* cur, PyObject* pSql, PyObject* paramArrayObj);

// Optimised batch INSERT using a single multi-row VALUES clause.
// For INSERT ... VALUES (?,?) with N rows, expands to
//   INSERT ... VALUES (?,?),(?,?),...  and calls SQLExecute exactly once per
// sub-batch (sub-batches are used when the driver's SQL_MAX_PARAMS_IN_SELECT
// limit would be exceeded).
// For UPDATE / DELETE / MERGE, callers should use ExecuteMultiFallback instead.
bool ExecuteBatchInsert(Cursor* cur, PyObject* pSql, PyObject* paramArrayObj);
bool ExecuteBatch(Cursor* cur, PyObject* pSql, PyObject* paramArrayObj);
bool GetParameterInfo(Cursor* cur, Py_ssize_t index, PyObject* param, ParamInfo& info, bool isTVP);
bool PrepareOnCursor(Cursor* cur, PyObject* pSql); // Renamed to avoid conflicts if 'Prepare' is common
bool BindParameter(Cursor* cur, Py_ssize_t index, ParamInfo& info);
void FreeInfos(ParamInfo* a, Py_ssize_t count);
void FreeParameterData(Cursor* cur);
void FreeParameterInfo(Cursor* cur);

#endif
