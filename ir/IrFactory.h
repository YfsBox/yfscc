//
// Created by 杨丰硕 on 2023/4/17.
//
#ifndef YFSCC_IRFACTORY_H
#define YFSCC_IRFACTORY_H

#include <memory>
#include <vector>

class Value;
class Module;
class BasicBlock;
class IrContext;
class Function;

class IrFactory {
public:
    using ValuePtr = Value *;

    IrFactory() = default;

    ~IrFactory() = default;

    static ValuePtr createArgument(bool is_float, Function *function, const std::string &name);

    static ValuePtr createArrayArgument(bool is_float, Function *function, const std::vector<int32_t> &dimension, const std::string &name);

    static ValuePtr createFConstantVar(float value);

    static ValuePtr createIConstantVar(int32_t value);

    static ValuePtr createFConstantArray(const std::vector<int32_t> &dimensions, const std::string &name);

    static ValuePtr createIConstantArray(const std::vector<int32_t> &dimensions, const std::string &name);

    static ValuePtr createIGlobalVar(const std::string &name, int32_t initval);

    static ValuePtr createFGlobalVar(const std::string &name, float initval);

    static ValuePtr createIGlobalArray(bool is_const, ConstantArray *constarray, const std::string &name);

    static ValuePtr createFGlobalArray(bool is_const, ConstantArray *constarray, const std::string &name);
    // static ValuePtr createIGlobalArray(const std::string &name, int dimension);
    static ValuePtr createBasicBlock(const std::string &name);

    static ValuePtr createFloatFunction(const std::string &name);

    static ValuePtr createIntFunction(const std::string &name);

    static ValuePtr createVoidFunction(const std::string &name);

    static ValuePtr createAddInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createSubInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createMulInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createDivInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createAndInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createOrInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createXorInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createModInstruction(Value *left, Value *right, BasicType btype);

    static ValuePtr createFAllocaInstruction(const std::string &name);

    static ValuePtr createFAllocaInstruction();

    static ValuePtr createIAllocaInstruction(const std::string &name);

    static ValuePtr createIAllocaInstruction();

    static ValuePtr createIArrayAllocaInstruction(const std::vector<int32_t> &dimensions, const std::string &name);

    static ValuePtr createFArrayAllocaInstruction(const std::vector<int32_t> &dimensions, const std::string &name);

    static ValuePtr createILoadInstruction(Value *ptr);

    static ValuePtr createFLoadInstruction(Value *ptr);

    static ValuePtr createFStoreInstruction(Value *value, Value *ptr);

    static ValuePtr createIStoreInstruction(Value *value, Value *ptr);

    static ValuePtr createVoidRetInstruction();

    static ValuePtr createIRetInstruction(Value *value);

    static ValuePtr createFRetInstruction(Value *value);

    static ValuePtr createBrInstruction(Value *label);

    static ValuePtr createCondBrInstruction(Value *cond, Value *truelabel, Value *falselabel);

    static ValuePtr createCallInstruction(const std::vector<Value *> &actuals, Function *function);

    static ValuePtr createEqICmpInstruction(Value *left, Value *right);

    static ValuePtr createNeICmpInstruction(Value *left, Value *right);

    static ValuePtr createLeICmpInstruction(Value *left, Value *right);

    static ValuePtr createGeICmpInstruction(Value *left, Value *right);

    static ValuePtr createLtICmpInstruction(Value *left, Value *right);

    static ValuePtr createGtICmpInstruction(Value *left, Value *right);

    static ValuePtr createEqFCmpInstruction(Value *left, Value *right);

    static ValuePtr createNeFCmpInstruction(Value *left, Value *right);

    static ValuePtr createLeFCmpInstruction(Value *left, Value *right);

    static ValuePtr createGeFCmpInstruction(Value *left, Value *right);

    static ValuePtr createLtFCmpInstruction(Value *left, Value *right);

    static ValuePtr createGtFCmpInstruction(Value *left, Value *right);

    static ValuePtr createPhiInstruction(BasicType basic_type, const std::vector<Value *> &values, const std::vector<BasicBlock *> &bbs);

    static ValuePtr createI2FCastInstruction(Value *value);

    static ValuePtr createF2ICastInstruction(Value *value);

    static ValuePtr createFGEPInstruction(Value *ptr, Value *offset);

    static ValuePtr createIGEPInstruction(Value *ptr, Value *offset);

    static ValuePtr createIGEPInstruction(Value *base, const std::vector<Value *> &indexes);

    static ValuePtr createFGEPInstruction(Value *base, const std::vector<Value *> &indexes);

    static ValuePtr createConstIGlobalVar(int32_t initval, const std::string &name);

    static ValuePtr createConstFGlobalVar(float initval, const std::string &name);

    static ValuePtr createINotInstruction(Value *value);

    static ValuePtr createFNotInstruction(Value *value);

    static ValuePtr createINegInstruction(Value *value);

    static ValuePtr createFNegInstruction(Value *value);

    static ValuePtr createIMemSetInstruction(Value *base, Value *size, Value *value);

    static ValuePtr createFMemSetInstruction(Value *base, Value *size, Value *value);

    static void InitContext(IrContext *context);

private:
    static IrContext *context_;
};

#endif //YFSCC_IRFACTORY_H
