#pragma once
#include <string>
#include <stdexcept>

// The tags to identify the type of data at runtime
enum class ValueType
{
    VAL_INT,
    VAL_BOOL,
    VAL_STRING
};

// The Tagged Value structure
struct Value
{
    ValueType type;

    // A union shares memory. Since a value can only be ONE type at a time,
    // this saves massive amounts of RAM in the VM.
    union
    {
        int number;
        bool boolean;
    } as;

    // Strings require heap allocation, so they live outside the union
    std::string stringObj;

    // --- Constructors (Helpers to create values instantly) ---
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

    // --- Type Checking Helpers ---
    bool isInt() const { return type == ValueType::VAL_INT; }
    bool isBool() const { return type == ValueType::VAL_BOOL; }
    bool isString() const { return type == ValueType::VAL_STRING; }
};