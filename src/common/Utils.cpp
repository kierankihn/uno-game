/**
 * @file utils.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#include "Utils.h"

namespace UNO::COMMON {
    Random::Random() : rd_(), gen_(rd_()) {}
    std::mt19937 &Random::getGenerator() {
        return gen_;
    }

    Utils* Utils::instance_ = nullptr;

    Utils *Utils::getInstance() {
        if (instance_ == nullptr) {
            instance_ = new Utils();
        }
        return instance_;
    }
    Random &Utils::getRandom() {
        return this->random_;
    }


} // UNO