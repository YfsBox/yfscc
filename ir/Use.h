//
// Created by 杨丰硕 on 2023/4/15.
//

#ifndef YFSCC_USE_H
#define YFSCC_USE_H

class Value;
class User;

class Use {     // 表达了一种引用关系,value指的是所引用的value，
    // User则是引用这个value的对象，比如说该User将这个Value作为操作数，idx表示的是作为操作数的序号
public:
    Use() = default;

    Use(User *user, int idx = -1): idx_(idx), user_(user) {}

    ~Use() = default;

    void setIdx(int idx) {
        idx_ = idx;
    }

    Value *getValue() const {
        return value_;
    }

    User *getUser() const {
        return user_;
    }

    int getIdx() const {
        return idx_;
    }

    void setNoUser() {
        user_ = nullptr;
        idx_ = -1;
    }

private:
    int idx_{-1};
    Value *value_{nullptr};
    User *user_{nullptr};
};

#endif //YFSCC_USE_H
