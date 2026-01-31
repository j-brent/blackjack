/**
 * UI Renderer - Updates the DOM based on game state.
 */

import { createCardElement, createFaceDownCard } from './card-renderer.js';

/**
 * UIRenderer handles all DOM updates.
 */
export class UIRenderer {
    constructor() {
        // Cache DOM elements
        this.dealerArea = document.getElementById('dealer-area');
        this.dealerCards = document.getElementById('dealer-cards');
        this.dealerValue = document.getElementById('dealer-value');

        this.playerArea = document.getElementById('player-area');
        this.playerCards = document.getElementById('player-cards');
        this.playerValue = document.getElementById('player-value');

        this.playerAreaSplit = document.getElementById('player-area-split');
        this.actions = document.getElementById('actions');
        this.statusMessage = document.getElementById('status-message');
    }

    /**
     * Main render function - updates all UI based on game state.
     * @param {Object} gameState - State snapshot from GameController
     */
    render(gameState) {
        this.renderDealerArea(gameState);
        this.renderPlayerArea(gameState);
        this.renderActions(gameState);
        this.renderStatusBar(gameState);
    }

    /**
     * Render the dealer's hand.
     */
    renderDealerArea(gameState) {
        this.dealerCards.innerHTML = '';

        const dealerHand = gameState.dealerHand;

        // Render cards
        dealerHand.cards.forEach((card, index) => {
            let cardElement;
            if (index === 1 && !gameState.isDealerHoleCardVisible) {
                // Second card is the hole card, show face-down if not visible
                cardElement = createFaceDownCard();
            } else {
                cardElement = createCardElement(card);
            }
            this.dealerCards.appendChild(cardElement);
        });

        // Render value
        if (gameState.isDealerHoleCardVisible) {
            // Show full hand value
            let valueText = `Value: ${dealerHand.value}`;
            if (dealerHand.isSoft) {
                valueText += ' (soft)';
            }
            this.dealerValue.textContent = valueText;
        } else if (gameState.dealerUpCard) {
            // Show only up card value during player turn
            // Calculate value of first card only
            const upCardValue = this.getCardValue(gameState.dealerUpCard.rank);
            this.dealerValue.textContent = `Value: ${upCardValue}`;
        } else {
            this.dealerValue.textContent = '';
        }
    }

    /**
     * Get blackjack value of a single card.
     */
    getCardValue(rank) {
        if (rank >= 10) return 10; // J, Q, K
        return rank; // Ace=1, 2-10=face
    }

    /**
     * Render the player's hand(s).
     */
    renderPlayerArea(gameState) {
        const playerHands = gameState.playerHands;

        if (playerHands.length === 0) {
            // No hands (waiting for deal)
            this.playerArea.style.display = 'none';
            this.playerAreaSplit.style.display = 'none';
            return;
        }

        if (playerHands.length === 1) {
            // Single hand
            this.playerArea.style.display = 'flex';
            this.playerAreaSplit.style.display = 'none';
            this.renderSingleHand(playerHands[0]);
        } else {
            // Split hands
            this.playerArea.style.display = 'none';
            this.playerAreaSplit.style.display = 'flex';
            this.renderSplitHands(playerHands, gameState.activeHandIndex);
        }
    }

    /**
     * Render a single player hand.
     */
    renderSingleHand(hand) {
        this.playerCards.innerHTML = '';

        // Render cards
        hand.cards.forEach(card => {
            const cardElement = createCardElement(card);
            this.playerCards.appendChild(cardElement);
        });

        // Render value
        this.playerValue.textContent = this.formatHandValue(hand);
        this.playerValue.className = hand.isBust ? 'value bust' : 'value';
    }

    /**
     * Render split hands.
     */
    renderSplitHands(hands, activeHandIndex) {
        const handContainers = this.playerAreaSplit.querySelectorAll('.hand-container');

        handContainers.forEach((container, index) => {
            if (index >= hands.length) {
                container.style.display = 'none';
                return;
            }

            container.style.display = 'flex';

            const hand = hands[index];
            const cardContainer = container.querySelector('.card-container');
            const valueDisplay = container.querySelector('.value');

            // Clear and render cards
            cardContainer.innerHTML = '';
            hand.cards.forEach(card => {
                const cardElement = createCardElement(card);
                cardContainer.appendChild(cardElement);
            });

            // Render value
            valueDisplay.textContent = this.formatHandValue(hand);
            valueDisplay.className = hand.isBust ? 'value bust' : 'value';

            // Highlight active hand
            if (index === activeHandIndex) {
                container.classList.add('active');
            } else {
                container.classList.remove('active');
            }
        });
    }

    /**
     * Format hand value with soft indicator and bust.
     */
    formatHandValue(hand) {
        let text = `Value: ${hand.value}`;
        if (hand.isSoft) {
            text += ' (soft)';
        }
        if (hand.isBust) {
            text += ' BUST';
        }
        return text;
    }

    /**
     * Render action buttons based on available actions.
     */
    renderActions(gameState) {
        this.actions.innerHTML = '';

        const state = gameState.state;
        const availableActions = gameState.availableActions;

        if (state === 'PlayerTurn') {
            // Show Hit, Stand, and Split (if available)
            if (availableActions.includes('Hit')) {
                this.actions.appendChild(this.createButton('btn-hit', 'Hit (H)', 'hit'));
            }
            if (availableActions.includes('Stand')) {
                this.actions.appendChild(this.createButton('btn-stand', 'Stand (S)', 'stand'));
            }
            if (availableActions.includes('Split')) {
                this.actions.appendChild(this.createButton('btn-split', 'Split (P)', 'split'));
            }
        } else if (state === 'RoundOver') {
            // Show New Game button
            this.actions.appendChild(this.createButton('btn-new-game', 'New Game (N)', 'new-game'));
        }
        // DealerTurn and WaitingForDeal show no buttons
    }

    /**
     * Create a button element.
     */
    createButton(id, text, action) {
        const button = document.createElement('button');
        button.id = id;
        button.textContent = text;
        button.dataset.action = action;
        return button;
    }

    /**
     * Render status bar message.
     */
    renderStatusBar(gameState) {
        const state = gameState.state;
        const playerHands = gameState.playerHands;
        const activeHandIndex = gameState.activeHandIndex;

        let message = '';
        let className = '';

        if (state === 'WaitingForDeal') {
            message = 'Loading...';
            className = '';
        } else if (state === 'PlayerTurn') {
            if (playerHands.length > 1) {
                message = `Your turn — Hand ${activeHandIndex + 1}`;
            } else {
                message = 'Your turn';
            }
            className = '';
        } else if (state === 'DealerTurn') {
            message = "Dealer's turn...";
            className = '';
        } else if (state === 'RoundOver') {
            // Determine result message
            const resultInfo = this.getResultMessage(playerHands);
            message = resultInfo.message;
            className = resultInfo.className;
        }

        this.statusMessage.textContent = message;
        this.statusMessage.className = className;
    }

    /**
     * Get result message for RoundOver state.
     */
    getResultMessage(playerHands) {
        if (playerHands.length === 0) {
            return { message: '', className: '' };
        }

        if (playerHands.length === 1) {
            // Single hand
            const hand = playerHands[0];
            if (hand.result === 'Blackjack') {
                return { message: 'Blackjack! You win!', className: 'success' };
            } else if (hand.result === 'Win') {
                return { message: 'You win!', className: 'success' };
            } else if (hand.result === 'Lose') {
                if (hand.isBust) {
                    return { message: 'Bust!', className: 'error' };
                }
                return { message: 'You lose.', className: 'error' };
            } else if (hand.result === 'Push') {
                return { message: 'Push.', className: 'warning' };
            }
        } else {
            // Split hands - show both results
            const results = playerHands.map((hand, index) => {
                let handResult = '';
                if (hand.result === 'Win' || hand.result === 'Blackjack') {
                    handResult = 'Win';
                } else if (hand.result === 'Lose') {
                    handResult = hand.isBust ? 'Bust' : 'Lose';
                } else if (hand.result === 'Push') {
                    handResult = 'Push';
                }
                return `Hand ${index + 1}: ${handResult}`;
            });
            return { message: results.join(' | '), className: '' };
        }

        return { message: '', className: '' };
    }

    /**
     * Show loading state.
     */
    showLoading() {
        document.getElementById('loading-state').style.display = 'flex';
        document.getElementById('error-state').style.display = 'none';
        document.getElementById('game-container').style.display = 'none';
    }

    /**
     * Show error state.
     */
    showError() {
        document.getElementById('loading-state').style.display = 'none';
        document.getElementById('error-state').style.display = 'flex';
        document.getElementById('game-container').style.display = 'none';
    }

    /**
     * Show game state (hide loading/error).
     */
    showGame() {
        document.getElementById('loading-state').style.display = 'none';
        document.getElementById('error-state').style.display = 'none';
        document.getElementById('game-container').style.display = 'flex';
    }

    /**
     * Focus on a specific button if it exists.
     */
    focusButton(buttonId) {
        const button = document.getElementById(buttonId);
        if (button) {
            button.focus();
        }
    }
}
