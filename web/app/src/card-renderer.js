/**
 * Card rendering utilities.
 * Creates DOM elements for playing cards.
 */

/**
 * Map rank enum values to display strings.
 * @param {number} rank - Rank enum value from WASM
 * @returns {string} Display string (A, 2-10, J, Q, K)
 */
export function rankToString(rank) {
    const ranks = {
        1: 'A',
        2: '2',
        3: '3',
        4: '4',
        5: '5',
        6: '6',
        7: '7',
        8: '8',
        9: '9',
        10: '10',
        11: 'J',
        12: 'Q',
        13: 'K'
    };
    return ranks[rank] || '?';
}

/**
 * Map suit enum values to Unicode symbols.
 * @param {number} suit - Suit enum value from WASM
 * @returns {string} Unicode suit symbol
 */
export function suitToSymbol(suit) {
    const suits = {
        0: '♣', // Clubs
        1: '♦', // Diamonds
        2: '♥', // Hearts
        3: '♠'  // Spades
    };
    return suits[suit] || '?';
}

/**
 * Map suit enum values to color class.
 * @param {number} suit - Suit enum value from WASM
 * @returns {string} 'red' or 'black'
 */
export function suitToColorClass(suit) {
    // Diamonds (1) and Hearts (2) are red
    return (suit === 1 || suit === 2) ? 'red' : 'black';
}

/**
 * Get full suit name for ARIA labels.
 * @param {number} suit - Suit enum value from WASM
 * @returns {string} Full suit name
 */
export function suitToName(suit) {
    const suitNames = {
        0: 'Clubs',
        1: 'Diamonds',
        2: 'Hearts',
        3: 'Spades'
    };
    return suitNames[suit] || 'Unknown';
}

/**
 * Get full rank name for ARIA labels.
 * @param {number} rank - Rank enum value from WASM
 * @returns {string} Full rank name
 */
export function rankToName(rank) {
    const rankNames = {
        1: 'Ace',
        2: 'Two',
        3: 'Three',
        4: 'Four',
        5: 'Five',
        6: 'Six',
        7: 'Seven',
        8: 'Eight',
        9: 'Nine',
        10: 'Ten',
        11: 'Jack',
        12: 'Queen',
        13: 'King'
    };
    return rankNames[rank] || 'Unknown';
}

/**
 * Create a face-up card element.
 * @param {Object} card - Card object with rank and suit properties
 * @returns {HTMLDivElement} Card element
 */
export function createCardElement(card) {
    const cardDiv = document.createElement('div');
    cardDiv.className = `card ${suitToColorClass(card.suit)}`;
    cardDiv.setAttribute('role', 'img');
    cardDiv.setAttribute('aria-label', `${rankToName(card.rank)} of ${suitToName(card.suit)}`);

    const rankStr = rankToString(card.rank);
    const suitSymbol = suitToSymbol(card.suit);

    // Top-left rank
    const rankTop = document.createElement('div');
    rankTop.className = 'card-rank-top';
    rankTop.textContent = rankStr;
    cardDiv.appendChild(rankTop);

    // Center suit
    const suitCenter = document.createElement('div');
    suitCenter.className = 'card-suit-center';
    suitCenter.textContent = suitSymbol;
    cardDiv.appendChild(suitCenter);

    // Bottom-right rank (rotated 180°)
    const rankBottom = document.createElement('div');
    rankBottom.className = 'card-rank-bottom';
    rankBottom.textContent = rankStr;
    cardDiv.appendChild(rankBottom);

    return cardDiv;
}

/**
 * Create a face-down card element (hole card).
 * @returns {HTMLDivElement} Face-down card element
 */
export function createFaceDownCard() {
    const cardDiv = document.createElement('div');
    cardDiv.className = 'card face-down';
    cardDiv.setAttribute('role', 'img');
    cardDiv.setAttribute('aria-label', 'Hole card hidden');
    return cardDiv;
}
