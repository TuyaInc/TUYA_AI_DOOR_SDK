//
// Created by 张树杰 on 2019/2/9.
//

#ifndef TUYASMART_AI_IPC_SINGLETON_H
#define TUYASMART_AI_IPC_SINGLETON_H

#include <spin_lock.h>
#include <atomic>
#include <memory>

namespace tuya {
    template<typename T>
    class Singleton {
    public:
        static T *getInstance() {
            if (!instancePtr) {
                spin_lock_t lock(lockFlag);
                if (!instancePtr) {
                    instance = std::shared_ptr<T>(instancePtr = new T);
                }
            }
            return instancePtr;
        }

        static std::shared_ptr<T> &getSharedPtr() {
            if (instance == nullptr) {
                spin_lock_t lock(lockFlag);
                if (instancePtr == nullptr) {
                    getInstance();
                }else{
                    instance = std::shared_ptr<T>(instancePtr);
                }
            }
            return instance;
        }

        ~Singleton() {}

    protected:
        Singleton() {
        }


    private:
        static std::shared_ptr<T> instance;
        static T* instancePtr;
        static std::atomic_flag lockFlag;

        Singleton(const Singleton &src) = delete;

        Singleton &operator=(const Singleton &) = delete;

        class Garbo {
            ~Garbo() {
                if (Singleton::instance) {
                    delete Singleton::instance;
                    Singleton::instance = nullptr;
                }
            }
        };

        static Garbo garbo;
    };

    template<class T>
    std::shared_ptr<T> Singleton<T>::instance = nullptr;

    template <class T>
    T* Singleton<T>::instancePtr = nullptr;

    template<class T>
    std::atomic_flag Singleton<T>::lockFlag(false);
}


#endif //TUYASMART_AI_IPC_SINGLETON_H
