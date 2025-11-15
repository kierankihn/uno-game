/**
 * @file utils.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#include "utils.h"

namespace UNO::COMMON {
    Random::Random() : rd_(), gen_(rd_()) {}
    std::mt19937 &Random::getGenerator() {
        return gen_;
    }

    Utils *Utils::getInstance() {
        if (Utils::instance_ == nullptr) {
            Utils::instance_ = new Utils();
        }
        return Utils::instance_;
    }
    Random &Utils::getRandom() {
        return this->random_;
    }


} // UNO