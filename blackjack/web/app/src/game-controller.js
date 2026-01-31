/**
 * Game Controller - Wraps the WASM Game instance and provides a clean JS API.
 */

import { rankToString, suitToSymbol } from './card-renderer.js';

/**
 * Map GameState enum values to string names.
 */
const GameStateNames = {
    0: 'WaitingForDeal',
    1: 'PlayerTurn',
    2: 'DealerTurn',
    3: 'RoundOver'
};

/**
 * Map HandResult enum values to string names.
 */
const HandResultNames = {
    0: 'None',
    1: 'Win',
    2: 'Lose',
    3: 'Push',
    4: 'Blackjack'
};

/**
 * Map PlayerAction enum values to string names.
 */
const PlayerActionNames = {
    0: 'Hit',
    1: 'Stand',
    2: 'Split'
};

/**
 * GameController manages the WASM Game instance and provides state snapshots.
 */
export class GameController {
    /**
     * @param {Object} Module - The Emscripten module
     * @param {Object} game - The Game instance from WASM
     */
    constructor(Module, game) {
        this.Module = Module;
        this.game = game;
    }

    /**
     * Start a new round.
     * @returns {number} ActionResult enum value
     */
    deal() {
        return this.game.deal();
    }

    /**
     * Hit - draw a card for the active hand.
     * @returns {number} ActionResult enum value
     */
    hit() {
        return this.game.hit();
    }

    /**
     * Stand - end the active hand's turn.
     * @returns {number} ActionResult enum value
     */
    stand() {
        return this.game.stand();
    }

    /**
     * Split the active hand.
     * @returns {number} ActionResult enum value
     */
    split() {
        return this.game.split();
    }

    /**
     * Execute the dealer's turn.
     * @returns {number} ActionResult enum value
     */
    playDealer() {
        return this.game.play_dealer();
    }

    /**
     * Get a plain JS snapshot of the current game state.
     * @returns {Object} State snapshot
     */
    getState() {
        const state = this.game.state();
        const playerHandsVector = this.game.player_hands();
        const dealerHand = this.game.dealer_hand();
        const dealerUpCardOpt = this.game.dealer_up_card();
        const availableActionsVector = this.game.available_actions();

        // Convert player hands
        const playerHands = [];
        for (let i = 0; i < playerHandsVector.size(); i++) {
            const handState = playerHandsVector.get(i);
            const hand = handState.hand;
            const cardsVector = hand.cards();
            const cards = [];
            for (let j = 0; j < cardsVector.size(); j++) {
                const card = cardsVector.get(j);
                cards.push({
                    rank: card.rank.value,
                    suit: card.suit.value
                });
            }
            playerHands.push({
                cards: cards,
                value: hand.value(),
                isSoft: hand.is_soft(),
                isBust: hand.is_bust(),
                result: HandResultNames[handState.result.value] || 'None',
                isStood: handState.is_stood,
                isFromSplit: handState.is_from_split,
                isSplitAces: handState.is_split_aces
            });
        }

        // Convert dealer hand
        const dealerCardsVector = dealerHand.cards();
        const dealerCards = [];
        for (let i = 0; i < dealerCardsVector.size(); i++) {
            const card = dealerCardsVector.get(i);
            dealerCards.push({
                rank: card.rank.value,
                suit: card.suit.value
            });
        }

        // Convert dealer up card
        let dealerUpCard = null;
        if (dealerUpCardOpt.has_value) {
            dealerUpCard = {
                rank: dealerUpCardOpt.card.rank.value,
                suit: dealerUpCardOpt.card.suit.value
            };
        }

        // Convert available actions
        const availableActions = [];
        for (let i = 0; i < availableActionsVector.size(); i++) {
            const action = availableActionsVector.get(i);
            availableActions.push(PlayerActionNames[action.value] || 'Unknown');
        }

        return {
            state: GameStateNames[state.value] || 'Unknown',
            playerHands: playerHands,
            activeHandIndex: this.game.active_hand_index(),
            dealerHand: {
                cards: dealerCards,
                value: dealerHand.value(),
                isSoft: dealerHand.is_soft()
            },
            isDealerHoleCardVisible: this.game.is_dealer_hole_card_visible(),
            dealerUpCard: dealerUpCard,
            availableActions: availableActions
        };
    }

    /**
     * Cleanup WASM resources.
     */
    destroy() {
        if (this.game) {
            this.game.delete();
            this.game = null;
        }
    }
}
