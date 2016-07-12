/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   socket_collector.h
 * Author: chenwz5
 *
 * Created on 2016年7月8日, 下午3:14
 */

#ifndef COLLECTOR_H
#define COLLECTOR_H

namespace N {
    //TODO: 优化接口
template<typename Key, typename Item> 
class Collector {
public:
    Item* Find(Key* key) = 0;
    void Add(Item* item) = 0;
    void Remove(Key* key) = 0;
    size_t Size() = 0;
    Item* GetItem() = 0;
};

}

#endif /* COLLECTOR_H */

