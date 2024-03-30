#pragma once

#include "Library/Factory/Factory.h"

namespace al {
class CameraPoser;
}

namespace cc {
    template <class T>
    al::CameraPoser *createCustomCameraPoser(const char *name)
    {
        return new T(name);
    };
}

namespace al {

    template <class T>
    CameraPoser* createCameraPoserFunction(const char *name);

    typedef CameraPoser* (*createCameraPoser)(const char* name);
    
    class CameraPoserFactory : public Factory<createCameraPoser> {
        public:
            CameraPoserFactory(const char *fName) __attribute__((noinline)) : Factory(fName) {
                this->mFactoryName = fName;
                this->mFactoryEntries = nullptr;
                this->mNumFactoryEntries = 0;
            };

            virtual CameraPoser *createEntranceCameraPoser(void) const;
            // return new al::CameraPoserEntrance(スタート);

            int mLastUsedIndex = 0;
    };
}

namespace alCameraPoserFactoryFunction {
    void initAndCreateTableFromOtherTable2(al::CameraPoserFactory *,al::NameToCreator<al::createCameraPoser> const*,int,al::NameToCreator<al::createCameraPoser> const*,int);
    void initAndCreateTableWithAnotherFactory(al::CameraPoserFactory *,al::CameraPoserFactory const*,al::NameToCreator<al::createCameraPoser> const*,int);
    void initAndCreateTableWithPresetPosers(al::CameraPoserFactory *,al::NameToCreator<al::createCameraPoser> const*,int);
}
