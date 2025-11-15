/**
 * @file utils.h
 * 工具
 *
 * @author Yuzhe Guo
 * @date 2025.11.15
 */
#ifndef UNO_GAME_UTILS_H
#define UNO_GAME_UTILS_H
#include <random>

namespace UNO::COMMON {
    /**
     * 随机数生成器
     */
    class Random {
    private:
        std::random_device rd_;
        std::mt19937 gen_;

    public:
        Random();

        /**
         * @return 一个 std::mt19937
         */
        std::mt19937 &getGenerator();
    };

    class Utils {
    private:
        Utils() = default;
        Random random_;
        static Utils* instance_;

    public:
        /**
         * @return 随机数生成器
         */
        Random &getRandom();

        /**
         * @return 工具实例
         */
        static Utils *getInstance();

    };
} // UNO

#endif //UNO_GAME_UTILS_H
