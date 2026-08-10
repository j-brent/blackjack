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

        // Cache action buttons (static in HTML, toggled via hidden attribute)
        this.btnHit = document.getElementById('btn-hit');
        this.btnStand = document.getElementById('btn-stand');
        this.btnSplit = document.getElementById('btn-split');
        this.btnNewGame = document.getElementById('btn-new-game');
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

        // Build render order: hole card (index 1) first so it's underneath
        // in DOM stacking, up card (index 0) second so it's on top, then
        // any additional hits in natural order.
        const renderOrder = [];
        if (dealerHand.cards.length >= 2) {
            renderOrder.push(1, 0);
            for (let i = 2; i < dealerHand.cards.length; i++) {
                renderOrder.push(i);
            }
        } else {
            for (let i = 0; i < dealerHand.cards.length; i++) {
                renderOrder.push(i);
            }
        }

        renderOrder.forEach(index => {
            let cardElement;
            if (index === 1 && !gameState.isDealerHoleCardVisible) {
                cardElement = createFaceDownCard();
            } else {
                cardElement = createCardElement(dealerHand.cards[index]);
            }
            this.dealerCards.appendChild(cardElement);
        });

        this.applyCardOverlap(this.dealerCards);

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

        hand.cards.forEach(card => {
            const cardElement = createCardElement(card);
            this.playerCards.appendChild(cardElement);
        });

        this.applyCardOverlap(this.playerCards);

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

            cardContainer.innerHTML = '';
            hand.cards.forEach(card => {
                const cardElement = createCardElement(card);
                cardContainer.appendChild(cardElement);
            });

            this.applyCardOverlap(cardContainer);

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
     * Apply diagonal cascade overlap to cards in a container.
     * Each card after the first is offset right (showing only the
     * top-left rank/suit corner of the card beneath) and down.
     * The last card is fully visible. Tunable via --card-overlap-visible
     * and --card-cascade-down.
     */
    applyCardOverlap(container) {
        const cards = container.querySelectorAll('.card');
        const n = cards.length;

        for (const card of cards) {
            card.style.marginLeft = '';
            card.style.marginTop = '';
        }

        if (n <= 1) return;

        const cardWidth = cards[0].getBoundingClientRect().width;
        const containerFontSize = parseFloat(getComputedStyle(container).fontSize);

        // Visible strip per overlapped card (--card-overlap-visible, em-based)
        const overlapEm = parseFloat(
            getComputedStyle(document.documentElement).getPropertyValue('--card-overlap-visible')
        ) || 1.5;
        const overlapVisible = overlapEm * containerFontSize;

        // Vertical step per card (--card-cascade-down, em-based)
        const cascadeEm = parseFloat(
            getComputedStyle(document.documentElement).getPropertyValue('--card-cascade-down')
        ) || 1;
        const cascadeDown = cascadeEm * containerFontSize;

        const overlap = cardWidth - overlapVisible;
        for (let i = 1; i < n; i++) {
            cards[i].style.marginLeft = `-${overlap}px`;
            cards[i].style.marginTop = `${cascadeDown}px`;
        }
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
     * Render action buttons by toggling visibility.
     */
    renderActions(gameState) {
        const state = gameState.state;
        const availableActions = gameState.availableActions;

        // Hide all buttons first
        this.btnHit.hidden = true;
        this.btnStand.hidden = true;
        this.btnSplit.hidden = true;
        this.btnNewGame.hidden = true;

        if (state === 'PlayerTurn') {
            if (availableActions.includes('Hit')) {
                this.btnHit.hidden = false;
            }
            if (availableActions.includes('Stand')) {
                this.btnStand.hidden = false;
            }
            if (availableActions.includes('Split')) {
                this.btnSplit.hidden = false;
            }
        } else if (state === 'RoundOver') {
            this.btnNewGame.hidden = false;
        }
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
