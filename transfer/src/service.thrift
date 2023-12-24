// ----------------------------------------------
exception Error {
    1: string errorSource,
    2: string errorType,
    3: i64 errorCode,
    4: string errorMessage
}

// ----------------------------------------------
enum ColumnType {
    COLUMN_TYPE_INT32,
    COLUMN_TYPE_UINT64,
    COLUMN_TYPE_FLOAT,
    COLUMN_TYPE_STRING,
    COLUMN_TYPE_BOOL,
}

union ColumnValue {
    1: i32 i32_val,
    2: i64 u64_val,
    3: double float_val;
    4: string string_val;
    5: bool bool_val;
}

struct ColumnSchema {
    1: string columnName,
    2: ColumnType columnType
}

struct TableSchema {
    1: string tableName,
    2: list<ColumnSchema> columns
}

// ----------------------------------------------
enum ArithmethicOperatorValue {
     PLUS,
     MINUS,
     MUL,
     DIV
}
struct ArithmethicOperator {
    1: ArithmethicOperatorValue operator,
    2: ColumnType operandType
}

enum ComparisonOperatorValue {
    EQ,
    NEQ,
    LESS,
    LEQ,
    GREATER,
    GEQ
}
struct ComparisonOperator {
    1: ComparisonOperatorValue opereator,
    2: ColumnType opereandType
}

enum LogicalOperatorValue {
    AND,
    OR,
    NOT
}
struct LogicalOperator {
    1: LogicalOperatorValue operator
}

struct ExpressionOperator {
    1: ArithmethicOperator arithmethicOper,
    2: ComparisonOperator compaarisonOper,
    3: LogicalOperator logicalOper
}

// ----------------------------------------------
struct LiteralExpr {
    1: ColumnValue val;
    2: ColumnType columnType;
}

struct ColumnExpr {
    1: string tableName,
    2: string columnName,
    3: ColumnType columnType
}

//struct ComplexExpr {
//    1: Expression first,
//    2: Expression second,
//    3: ExpressionOperator op
//}
union Expression {
    1: LiteralExpr literal,
    2: ColumnExpr column,
}
struct Predicate {
    1: Expression expression
}

// ----------------------------------------------
struct Join {
    1: string what,
    2: Predicate on
}

// ----------------------------------------------
struct Record {
//    1: list<ColumnSchema> columnSchemas,
    1: list<ColumnValue> columnValues
}

struct RecordGroup {
    1: TableSchema schema,
    2: list<Record> records
}

struct RecordUpdate {
    1: string tableName,
    2: list<ColumnSchema> columnSchemas,
    3: list<Expression> values
}

// ----------------------------------------------
struct CreateTableStmt {
    1: TableSchema schema
}

struct DropTableStmt {
    1: string tableName
}

struct QueryStmt {
    1: string fromTable,
    2: Predicate where,
    3: list<Join> joins
}

struct InsertStmt {
    1: string into,
    2: RecordGroup records
}

struct UpdateStmt {
    1: string what,
    2: RecordUpdate setValues,
    3: Predicate where
}

struct DeleteStmt {
    1: string fromTable,
    2: Predicate where,
}

union Statement {
    1: CreateTableStmt createTableStmt,
    2: DropTableStmt dropTableStmt,
    3: QueryStmt queryStmt,
    4: InsertStmt insertStmt,
    5: UpdateStmt updateStmt,
    6: DeleteStmt deleteStmt
}

// ----------------------------------------------
struct RecordView {
    1: list<Record> records,
    2: list<ColumnSchema> columns
}
struct StatementResult {
    1: RecordView view
}

// ----------------------------------------------
service DatabaseService {
    StatementResult execute(1: Statement stmt) throws (1: Error err);
}
