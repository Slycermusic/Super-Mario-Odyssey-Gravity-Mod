#pragma once

#include <basis/seadTypes.h>
#include "al/hio/HioNode.h"
#include "al/nerve/Nerve.h"

namespace al {
class Nerve;
class NerveKeeper;

class NerveExecutor : public IUseNerve, public HioNode {
public:
    NerveExecutor(const char* name);

    virtual NerveKeeper* getNerveKeeper() const;
    virtual ~NerveExecutor();

    void initNerve(const Nerve* nerve, s32 stateCount);
    void updateNerve();

private:
    NerveKeeper* mKeeper = nullptr;
};
};  // namespace al