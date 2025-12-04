/**
 * @file GameState.h
 *
 * @author Yuzhe Guo
 * @date 2025.11.16
 */
#ifndef UNO_GAME_GAMESTATE_H
#define UNO_GAME_GAMESTATE_H
#include "CardTile.h"
#include "Player.h"

#include <stdexcept>
#include <string>

namespace UNO::GAME {

    /**
     * 玩家状态
     */
    class PlayerState {
    protected:
        std::string name_;
        size_t remainingCardCount_;
        bool isUno_;

        PlayerState(std::string name, size_t remainingCardCount, bool isUno);

    public:
        virtual ~PlayerState() = default;

        /**
         * @return 玩家名字
         */
        [[nodiscard]] std::string getName() const;

        /**
         * @return 玩家是否处于 UNO 状态
         */
        [[nodiscard]] bool getIsUno() const;

        /**
         * @return 玩家手中剩余的牌的数量
         */
        [[nodiscard]] size_t getRemainingCardCount() const;

        /**
         * 设置 Uno 状态
         * @param x Uno 状态
         */
        void setIsUno(bool x);

        /**
         * 摸牌
         * @param n 摸的张数
         * @param cards 摸到的牌
         */
        void virtual draw(size_t n, const std::vector<Card> &cards);

        /**
         * 出一张牌
         */
        Card virtual play(const Card &card);
    };

    /**
     * （供客户端使用）玩家状态
     */
    class ClientPlayerState final : public PlayerState {
    public:
        ClientPlayerState(std::string name, size_t remainingCardCount, bool isUno);

        /**
         * 摸牌
         * @param n 摸的张数
         * @param cards 摸的牌
         */
        void draw(size_t n, const std::vector<Card> &cards) override;

        /**
         * 出一张牌
         */
        Card play(const Card &card) override;
    };

    /**
     * （供服务端使用）玩家状态
     */
    class ServerPlayerState final : public PlayerState {
    private:
        HandCard handCard_;

    public:
        explicit ServerPlayerState(std::string name, size_t remainingCardCount, bool isUno);

        /**
         * 获得当前手牌
         * @return 当前手牌的集合
         */
        [[nodiscard]] const std::multiset<Card> &getCards() const;


        /**
         * 摸牌
         * @param x 摸的张数
         * @param cards 摸的牌
         */
        void draw(size_t x, const std::vector<Card> &cards) override;

        /**
         * 打出一张牌
         * @param card 要打出的手牌
         * @return 打出的手牌
         */
        Card play(const Card &card) override;

        /**
         * @return 手牌是否为空
         */
        [[nodiscard]] bool isEmpty() const;
    };

    /**
     * 保证模板使用的是 PlayerState 的派生类
     */
    template<typename PlayerStateType>
    concept PlayerStateTypeConcept = std::is_base_of_v<PlayerState, PlayerStateType>;

    /**
     * 游戏具体状态
     * @tparam PlayerStateType PlayerState 的派生类
     */
    template<PlayerStateTypeConcept PlayerStateType>
    class GameState {
    protected:
        explicit GameState();
        DiscardPile discardPile_;
        bool isReversed_;
        size_t drawCount_;
        std::vector<PlayerStateType> players_;
        std::vector<PlayerStateType>::iterator currentPlayer_;

        /**
         * 反转
         */
        void reverse();

        /**
         * 下一个玩家
         */
        void nextPlayer();

    public:
        virtual ~GameState() = default;
        /**
         * @return 所有玩家状态
         */
        [[nodiscard]] const std::vector<PlayerStateType> &getPlayers() const;

        /**
         * @return 当前玩家对应的 iterator
         */
        [[nodiscard]] std::vector<PlayerStateType>::iterator getCurrentPlayer() const;

        /**
         * @return 弃牌堆
         */
        [[nodiscard]] const DiscardPile &getDiscardPile() const;

        /**
         * @return 是否反转
         */
        [[nodiscard]] bool getIsReversed() const;

        [[nodiscard]] size_t getDrawCount() const;

        /**
         * 向对局中添加玩家
         * @param playerState 要添加的玩家状态
         */
        void addPlayer(PlayerStateType playerState);

        /**
         * 清空玩家
         */
        void clearPlayers();

        /**
         * 由于用户出牌而改变状态
         * @param card 用户出的牌
         */
        void virtual updateStateByCard(const Card &card);

        /**
         * 由于用户摸牌而改变状态
         */
        std::vector<Card> virtual updateStateByDraw();
    };

    template<PlayerStateTypeConcept PlayerStateType>
    GameState<PlayerStateType>::GameState() : isReversed_(false), drawCount_(0), players_(), currentPlayer_(players_.begin())
    {
    }

    template<PlayerStateTypeConcept PlayerStateType>
    const std::vector<PlayerStateType> &GameState<PlayerStateType>::getPlayers() const
    {
        return this->players_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    std::vector<PlayerStateType>::iterator GameState<PlayerStateType>::getCurrentPlayer() const
    {
        return this->currentPlayer_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    const DiscardPile &GameState<PlayerStateType>::getDiscardPile() const
    {
        return this->discardPile_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    bool GameState<PlayerStateType>::getIsReversed() const
    {
        return this->isReversed_;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    size_t GameState<PlayerStateType>::getDrawCount() const
    {
        return this->drawCount_;
    }


    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::addPlayer(PlayerStateType playerState)
    {
        int currentPlayerIndex = this->currentPlayer_ - this->players_.begin();
        this->players_.push_back(std::move(playerState));
        this->currentPlayer_ = this->players_.begin() + currentPlayerIndex;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::nextPlayer()
    {
        if (this->isReversed_ == false) {
            this->currentPlayer_ = std::next(this->currentPlayer_);
            if (this->currentPlayer_ == this->players_.end()) {
                this->currentPlayer_ = this->players_.begin();
            }
        }
        else {
            if (this->currentPlayer_ == this->players_.begin()) {
                this->currentPlayer_ = this->players_.end();
            }
            this->currentPlayer_ = std::prev(this->currentPlayer_);
        }
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::clearPlayers()
    {
        this->players_.clear();
        this->currentPlayer_ = this->players_.begin();
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::reverse()
    {
        this->isReversed_ ^= 1;
    }

    template<PlayerStateTypeConcept PlayerStateType>
    void GameState<PlayerStateType>::updateStateByCard(const Card &card)
    {
        if (this->discardPile_.isEmpty() == false && card.canBePlayedOn(this->discardPile_.getFront(), this->drawCount_) == false) {
            throw std::invalid_argument("Card cannot be played");
        }
        this->currentPlayer_->play(card);
        if (card.getType() == CardType::DRAW2) {
            this->drawCount_ += 2;
        }
        if (card.getType() == CardType::WILDDRAWFOUR) {
            this->drawCount_ += 4;
        }
        if (card.getType() == CardType::REVERSE) {
            this->reverse();
        }
        if (card.getType() == CardType::SKIP) {
            this->nextPlayer();
        }
        this->nextPlayer();
        this->discardPile_.add(card);
    }

    template<PlayerStateTypeConcept PlayerStateType>
    std::vector<Card> GameState<PlayerStateType>::updateStateByDraw()
    {
        if (this->drawCount_ == 0) {
            this->drawCount_ = 1;
        }
        this->currentPlayer_->draw(this->drawCount_, {});
        this->drawCount_ = 0;
        this->nextPlayer();
        return {};
    }

    class ClientGameState final : public GameState<ClientPlayerState> {
    private:
        Player player_;

    public:
        explicit ClientGameState(std::string name);

        /**
         * 获得当前手牌
         * @return 当前手牌的集合
         */
        [[nodiscard]] const std::multiset<Card> &getCards() const;

        /**
         * 初始化客户端状态
         */
        void init(DiscardPile discardPile);

        /**
         * 摸一张牌
         * @param card 摸的牌
         */
        void draw(const Card &card);

        /**
         * 摸多张牌
         * @param cards 摸的牌
         */
        void draw(const std::vector<Card> &cards);

        /**
         * 打出一张牌
         * @param it 要打出的手牌的迭代器
         * @return 打出的手牌
         */
        Card play(const std::multiset<Card>::iterator &it);

        /**
         * @return 手牌是否为空
         */
        [[nodiscard]] bool isEmpty() const;
    };

    class ServerGameState final : public GameState<ServerPlayerState> {
    private:
        Deck deck_;

    public:
        ServerGameState();

        /**
         * 开始游戏
         */
        void init();

        /**
         * 由于用户摸牌而改变状态
         */
        std::vector<Card> updateStateByDraw() override;

        /**
         * 重置游戏状态（用于重新开始）
         */
        void reset();
    };
}   // namespace UNO::GAME

#endif   // UNO_GAME_GAMESTATE_H
