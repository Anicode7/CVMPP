#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <cstdint>

// --- NEW: The Function Object ---
// Stores the function's name, how many arguments it takes, and its compiled instructions
struct FunctionObj
{
    std::string name;
    int arity;
    std::vector<uint8_t> chunk;

    FunctionObj(const std::string &n, int a) : name(n), arity(a) {}
};

// The tags to identify the type of data at runtime
enum class ValueType
{
    VAL_INT,
    VAL_BOOL,
    VAL_STRING,
    VAL_FUNC // <-- NEW: Functions are now first-class values
};

// The Tagged Value structure
struct Value
{
    ValueType type;

    // A union shares memory.
    union
    {
        int number;
        bool boolean;
    } as;

    // Heap-allocated objects
    std::string stringObj;
    std::shared_ptr<FunctionObj> funcObj; // <-- NEW: Shared pointer to function data

    // --- Constructors ---
    static Value createInt(int value)
    {
        Value v;
        v.type = ValueType::VAL_INT;
        v.as.number = value;
        return v;
    }

    static Value createBool(bool value)
    {
        Value v;
        v.type = ValueType::VAL_BOOL;
        v.as.boolean = value;
        return v;
    }

    static Value createString(const std::string &value)
    {
        Value v;
        v.type = ValueType::VAL_STRING;
        v.stringObj = value;
        return v;
    }

    static Value createFunction(std::shared_ptr<FunctionObj> func)
    {
        Value v;
        v.type = ValueType::VAL_FUNC;
        v.funcObj = func;
        return v;
    }

    // --- Type Checking Helpers ---
    bool isInt() const { return type == ValueType::VAL_INT; }
    bool isBool() const { return type == ValueType::VAL_BOOL; }
    bool isString() const { return type == ValueType::VAL_STRING; }
    bool isFunc() const { return type == ValueType::VAL_FUNC; } // <-- NEW
};