#pragma once

namespace al {

struct ActorInitInfo;
class LiveActor;
}

class SwitchKeepOnAreaGroup;
class SwitchOnAreaGroup;

namespace al {
class RailMoveMapParts : public LiveActor {
  SwitchKeepOnAreaGroup* mSwitchKeepOnAreaGroup;
  SwitchOnAreaGroup* mSwitchOnAreaGroup;
  sead::Vector3f mRailClippingInfo;
  float mRailCoord;
};
}