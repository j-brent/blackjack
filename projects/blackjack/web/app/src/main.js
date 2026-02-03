/**
 * Main entry point for the Blackjack web application.
 */

import createModule from '../build/blackjack.js';
import { GameController } from './game-controller.js';
import { UIRenderer } from './ui-renderer.js';

let gameController = null;
let uiRenderer = null;
let isProcessing = false;

/**
 * Initialize the application.
 */
async function init() {
    uiRenderer = new UIRenderer();
    uiRenderer.showLoading();

    try {
        // Load WASM module
        const Module = await createModule();

        // Create game instance
        const game = new Module.Game();
        gameController = new GameController(Module, game);

        // Setup event listeners
        setupEventListeners();

        // Show game UI
        uiRenderer.showGame();

        // Auto-deal on first load
        performAction('deal');

    } catch (error) {
        console.error('Failed to initialize game:', error);
        uiRenderer.showError();
    }
}

/**
 * Setup event listeners for buttons and keyboard shortcuts.
 */
function setupEventListeners() {
    // Reload button in error state
    const reloadButton = document.getElementById('reload-button');
    if (reloadButton) {
        reloadButton.addEventListener('click', () => {
            window.location.reload();
        });
    }

    // Delegate button clicks from actions container
    const actionsContainer = document.getElementById('actions');
    actionsContainer.addEventListener('click', (event) => {
        if (event.target.tagName === 'BUTTON') {
            const action = event.target.dataset.action;
            if (action) {
                performAction(action);
            }
        }
    });

    // Keyboard shortcuts
    document.addEventListener('keydown', (event) => {
        // Ignore if processing or if typing in an input (if any were added later)
        if (isProcessing || event.target.tagName === 'INPUT') {
            return;
        }

        const key = event.key.toLowerCase();
        let action = null;

        switch (key) {
            case 'h':
                if (isButtonVisible('btn-hit')) {
                    action = 'hit';
                }
                break;
            case 's':
                if (isButtonVisible('btn-stand')) {
                    action = 'stand';
                }
                break;
            case 'p':
                if (isButtonVisible('btn-split')) {
                    action = 'split';
                }
                break;
            case 'n':
                if (isButtonVisible('btn-new-game')) {
                    action = 'new-game';
                }
                break;
        }

        if (action) {
            event.preventDefault();
            performAction(action);
        }
    });
}

/**
 * Check if a button is currently visible and enabled.
 */
function isButtonVisible(buttonId) {
    const button = document.getElementById(buttonId);
    return button && !button.hidden && !button.disabled;
}

/**
 * Perform a game action.
 */
async function performAction(action) {
    if (isProcessing || !gameController) {
        return;
    }

    isProcessing = true;
    disableAllButtons();

    try {
        let result = null;

        switch (action) {
            case 'deal':
                result = gameController.deal();
                break;
            case 'hit':
                result = gameController.hit();
                break;
            case 'stand':
                result = gameController.stand();
                break;
            case 'split':
                result = gameController.split();
                break;
            case 'new-game':
                result = gameController.deal();
                break;
        }

        // Get updated state
        const gameState = gameController.getState();

        // Check if we need to auto-play dealer
        if (gameState.state === 'DealerTurn') {
            // Render current state first
            uiRenderer.render(gameState);

            // Brief delay to show state change
            await sleep(500);

            // Play dealer turn
            gameController.playDealer();

            // Get final state after dealer plays
            const finalState = gameController.getState();
            uiRenderer.render(finalState);

            // Focus on New Game button
            setTimeout(() => {
                uiRenderer.focusButton('btn-new-game');
            }, 100);
        } else {
            // Normal state update
            uiRenderer.render(gameState);

            // Focus management
            if (gameState.state === 'PlayerTurn') {
                // Focus on Hit button by default
                setTimeout(() => {
                    uiRenderer.focusButton('btn-hit');
                }, 100);
            } else if (gameState.state === 'RoundOver') {
                setTimeout(() => {
                    uiRenderer.focusButton('btn-new-game');
                }, 100);
            }
        }

    } catch (error) {
        console.error('Error performing action:', error);
    } finally {
        isProcessing = false;
        enableAllButtons();
    }
}

/**
 * Disable all action buttons.
 */
function disableAllButtons() {
    const buttons = document.querySelectorAll('#actions button');
    buttons.forEach(button => {
        button.disabled = true;
    });
}

/**
 * Enable all action buttons.
 */
function enableAllButtons() {
    const buttons = document.querySelectorAll('#actions button');
    buttons.forEach(button => {
        button.disabled = false;
    });
}

/**
 * Sleep utility for delays.
 */
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// Start the application when DOM is ready
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
} else {
    init();
}
