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

    public:
        PlayerState(std::string name, size_t remainingCardCount, bool isUno);

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
         * 将剩余手牌设置为 x 张
         * @param x 要设置的张数
         */
        void setRemainingCardCount(size_t x);

        /**
         * 设置 Uno 状态
         * @param x Uno 状态
         */
        void setIsUno(bool x);
    };

    /**
     * （供客户端使用）玩家状态
     */
    class ClientPlayerState : public PlayerState {
    public:
        ClientPlayerState(std::string name, size_t remainingCardCount, bool isUno);
    };

    /**
     * （供服务端使用）玩家状态
     */
    class ServerPlayerState : public PlayerState {
    private:
        HandCard *handCard_;

    public:
        explicit ServerPlayerState(std::string name, size_t remainingCardCount, bool isUno, HandCard *handCard);

        template<typename... T>
        void draw(T... args);
    };

    /**
     * 游戏状态
     */
    enum class GameStatus : uint8_t {
        WAITING_PLAYERS_TO_JOIN,
        WAITING_PLAYERS_TO_START,
        WAITING_PLAYERS_TO_NEXT_TURN,
        WAITING_PLAYERS_TO_NEXT_ROUND
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
        explicit GameState(GameStatus gameStatus);
        GameStatus gameStatus_;
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
        void updateStateByCard(const Card &card);

        /**
         * 由于用户摸牌而改变状态
         */
        void virtual updateStateByDraw();
    };

    template<PlayerStateTypeConcept PlayerStateType>
    GameState<PlayerStateType>::GameState(GameStatus gameStatus) :
        gameStatus_(gameStatus), isReversed_(false), drawCount_(0), players_(), currentPlayer_(players_.begin())
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
            if (this->currentPlayer_ == this->players_.end()) {
                this->currentPlayer_ = std::next(this->currentPlayer_);
                this->currentPlayer_ = this->players_.begin();
            }
        }
        else {
            if (this->currentPlayer_ == this->players_.begin()) {
                this->currentPlayer_ = std::prev(this->players_.end());
            }
            else {
                this->currentPlayer_ = std::prev(this->currentPlayer_);
            }
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
        if (this->discardPile_.isEmpty() == false && card.canBePlayedOn(this->discardPile_.getFront()) == false) {
            throw std::invalid_argument("Card cannot be played");
        }
        this->currentPlayer_->setRemainingCardCount(this->currentPlayer_->getRemainingCardCount() - 1);
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
    void GameState<PlayerStateType>::updateStateByDraw()
    {
        if (this->drawCount_ != 0) {
            this->drawCount_ = 0;
        }
    }

    class ClientGameState final : public GameState<ClientPlayerState> {
    public:
        Player player;

        ClientGameState(GameStatus gameStatus, Player player);
    };

    class ServerGameState final : public GameState<ServerPlayerState> {
    private:
        Deck deck_;

    public:
        ServerGameState();

        /**
         * 由于用户摸牌而改变状态
         */
        void updateStateByDraw() override;
    };
}   // namespace UNO::GAME

#endif   // UNO_GAME_GAMESTATE_H
