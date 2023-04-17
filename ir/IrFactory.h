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

class IrFactory {
public:
    using ValuePtr = std::unique_ptr<Value>;

    IrFactory() = default;

    ~IrFactory() = default;

    static ValuePtr createFConstantVar(float value);

    static ValuePtr createIConstantVar(int32_t value);

    static ValuePtr createFConstantArray(int dimension);

    static ValuePtr createIConstantArray(int dimension);

    static ValuePtr createIGlobalVar(const std::string &name, int32_t initval);

    static ValuePtr createFGlobalVar(const std::string &name, float initval);
    // static ValuePtr createIGlobalArray(const std::string &name, int dimension);
    static ValuePtr createBasicBlock(const std::string &name);

    static ValuePtr createFloatFunction(const std::vector<std::string> &params, const std::string &name);

    static ValuePtr createIntFunction(const std::vector<std::string> &params, const std::string &name);

    static ValuePtr createVoidFunction(const std::vector<std::string> &params, const std::string &name);

    static ValuePtr createAddInstruction(Value *left, Value *right);

    static ValuePtr createSubInstruction(Value *left, Value *right);

    static ValuePtr createMulInstruction(Value *left, Value *right);

    static ValuePtr createDivInstruction(Value *left, Value *right);

    static ValuePtr createAndInstruction(Value *left, Value *right);

    static ValuePtr createOrInstruction(Value *left, Value *right);

    static ValuePtr createXorInstruction(Value *left, Value *right);

    static ValuePtr createFAllocaInstruction();

    static ValuePtr createIAllocaInstruction();

    static ValuePtr createIArrayAllocaInstruction(size_t array_size);

    static ValuePtr createFArrayAllocaInstruction(size_t array_size);

    static ValuePtr createILoadInstruction(Value *ptr);

    static ValuePtr createFLoadInstruction(Value *ptr);

    static ValuePtr createStoreInstruction(Value *value, Value *ptr);

    static ValuePtr createVoidRetInstruction();

    static ValuePtr createRetInstruction(Value *value);

    static ValuePtr createBrInstruction(Value *label);

    static ValuePtr createCondBrInstruction(Value *cond, Value *truelabel, Value *falselabel);

    static ValuePtr createCallInstruction(const std::vector<Value *> &actuals);

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

    static ValuePtr createPhiInstruction(const std::vector<Value *> &values, const std::vector<BasicBlock *> &bbs);

    static ValuePtr createI2FCastInstruction(Value *value);

    static ValuePtr createF2ICastInstruction(Value *value);

private:
    static IrContext *context_;
};

#endif //YFSCC_IRFACTORY_H
