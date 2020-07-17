//
//  spin_lock.h
//  fsys
//
//  Created by 薯条 on 2018/6/20.
//  Copyright © 2018年 薯条. All rights reserved.
//

#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

namespace tuya {

struct spin_lock_t{
    spin_lock_t(std::atomic_flag& mutex):mutex(mutex){while(mutex.test_and_set(std::memory_order_consume));}
    
    ~spin_lock_t(){mutex.clear(std::memory_order_release);}
    
private:
    std::atomic_flag& mutex;
};
    
}
    
#endif /* SPIN_LOCK_H */
