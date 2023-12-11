#pragma once

class PlayerHitPointData {
public:
  virtual void someFunction();

  bool mKidsMode;
  int mCurrentHealth;
  bool mHaveMaxUpItem;
  bool mForceNormalMode;
};