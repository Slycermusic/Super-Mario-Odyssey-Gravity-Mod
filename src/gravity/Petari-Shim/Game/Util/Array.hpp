#pragma once

namespace MR {
    template<class T>
    class AssignableArray {
    public:
        inline AssignableArray() {
            mArr = 0;
            mMaxSize = 0;
        }

        inline void init(s32 cnt) {
            mArr = new T[cnt];
            mMaxSize = cnt;
        }

        inline void assign(T what, s32 where) {
            mArr[where] = what;
        }

        inline T* begin() {
            return mArr;
        }

        inline T* end() {
            return &mArr[mMaxSize];
        }

        ~AssignableArray() {
            if (mArr) {
                delete[] mArr;
            }
        }

        inline T* getAtIdx(u32 idx) {
            return mArr[idx];
        }

        typedef T Item;
    
        T* mArr;        // _0
        s32 mMaxSize;   // _4
    };
}

