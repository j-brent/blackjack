/**
 * Card rendering utilities.
 * Creates SVG elements for playing cards.
 */

const SVG_NS = 'http://www.w3.org/2000/svg';

/**
 * Themable card tokens. Sizes are unitless viewBox user units (see
 * .claude/learnings css.md g6h7i8j9 — font-size stays an SVG attribute so
 * no px enters the stylesheet). Cached per theme; the cache key is the
 * active data-theme, so switching themes re-reads automatically.
 */
let cardTokens = null;
let cardTokensTheme = null;

function getCardTokens() {
    const theme = document.documentElement.dataset.theme || '';
    if (cardTokens && cardTokensTheme === theme) {
        return cardTokens;
    }

    const styles = getComputedStyle(document.documentElement);
    const read = (name, fallback) => styles.getPropertyValue(name).trim() || fallback;

    cardTokens = {
        rankSize: read('--card-rank-size', '56'),
        suitSize: read('--card-suit-size', '56'),
        suitCenterSize: read('--card-suit-center-size', '120'),
        backMarkSize: read('--card-back-mark-size', '48'),
        // CSS string value. Custom properties reach us unparsed, so strip the
        // quotes and decode any unicode escape ("\2660" would otherwise be
        // drawn on the card literally).
        backMark: read('--card-back-mark', '??')
            .replace(/^["']|["']$/g, '')
            .replace(/\\([0-9a-fA-F]{1,6})\s?/g,
                (_, hex) => String.fromCodePoint(parseInt(hex, 16)))
    };
    cardTokensTheme = theme;
    return cardTokens;
}

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
        0: '\u2663', // ♣ Clubs
        1: '\u2666', // ♦ Diamonds
        2: '\u2665', // ♥ Hearts
        3: '\u2660'  // ♠ Spades
    };
    return suits[suit] || '?';
}

/**
 * Map suit enum values to color class.
 * @param {number} suit - Suit enum value from WASM
 * @returns {string} 'red' or 'black'
 */
export function suitToColorClass(suit) {
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
 * Create an SVG text element.
 */
function svgText(cls, x, y, content, attrs) {
    const el = document.createElementNS(SVG_NS, 'text');
    el.setAttribute('class', cls);
    el.setAttribute('x', x);
    el.setAttribute('y', y);
    el.textContent = content;
    if (attrs) {
        for (const [k, v] of Object.entries(attrs)) {
            el.setAttribute(k, v);
        }
    }
    return el;
}

/**
 * Create a face-up card SVG element.
 * @param {Object} card - Card object with rank and suit properties
 * @returns {SVGSVGElement} Card SVG element
 */
export function createCardElement(card) {
    const svg = document.createElementNS(SVG_NS, 'svg');
    svg.setAttribute('class', `card ${suitToColorClass(card.suit)}`);
    svg.setAttribute('viewBox', '0 0 250 350');
    svg.setAttribute('role', 'img');
    svg.setAttribute('aria-label', `${rankToName(card.rank)} of ${suitToName(card.suit)}`);

    // Card background
    const rect = document.createElementNS(SVG_NS, 'rect');
    rect.setAttribute('class', 'card-bg');
    rect.setAttribute('width', '250');
    rect.setAttribute('height', '350');
    rect.setAttribute('rx', '12');
    svg.appendChild(rect);

    const rankStr = rankToString(card.rank);
    const suitSymbol = suitToSymbol(card.suit);
    const tokens = getCardTokens();

    // Top-left rank and suit
    svg.appendChild(svgText('card-rank-top', '15', '62', rankStr, { 'font-size': tokens.rankSize }));
    svg.appendChild(svgText('card-suit-top', '15', '110', suitSymbol, { 'font-size': tokens.suitSize }));

    // Center suit
    svg.appendChild(svgText('card-suit-center', '125', '200', suitSymbol, {
        'text-anchor': 'middle',
        'dominant-baseline': 'central',
        'font-size': tokens.suitCenterSize
    }));

    // Bottom-right rank and suit (rotated 180° around card center)
    // Uses same coords as top-left, rotated around (125,175) so text
    // extends inward after rotation — stays within viewBox.
    svg.appendChild(svgText('card-rank-bottom', '15', '62', rankStr, {
        'transform': 'rotate(180, 125, 175)',
        'font-size': tokens.rankSize
    }));
    svg.appendChild(svgText('card-suit-bottom', '15', '110', suitSymbol, {
        'transform': 'rotate(180, 125, 175)',
        'font-size': tokens.suitSize
    }));

    return svg;
}

/**
 * Create a face-down card SVG element (hole card).
 * @returns {SVGSVGElement} Face-down card SVG element
 */
export function createFaceDownCard() {
    const svg = document.createElementNS(SVG_NS, 'svg');
    svg.setAttribute('class', 'card face-down');
    svg.setAttribute('viewBox', '0 0 250 350');
    svg.setAttribute('role', 'img');
    svg.setAttribute('aria-label', 'Hole card hidden');

    // Defs for stripe pattern
    const defs = document.createElementNS(SVG_NS, 'defs');
    const pattern = document.createElementNS(SVG_NS, 'pattern');
    pattern.setAttribute('id', 'card-back-stripes');
    pattern.setAttribute('width', '20');
    pattern.setAttribute('height', '20');
    pattern.setAttribute('patternUnits', 'userSpaceOnUse');
    pattern.setAttribute('patternTransform', 'rotate(45)');

    const stripe1 = document.createElementNS(SVG_NS, 'rect');
    stripe1.setAttribute('width', '10');
    stripe1.setAttribute('height', '20');
    stripe1.setAttribute('fill', 'var(--card-back-stripe, #1a4d80)');

    const stripe2 = document.createElementNS(SVG_NS, 'rect');
    stripe2.setAttribute('x', '10');
    stripe2.setAttribute('width', '10');
    stripe2.setAttribute('height', '20');
    stripe2.setAttribute('fill', 'var(--card-back-stripe-alt, #2a5d90)');

    pattern.appendChild(stripe1);
    pattern.appendChild(stripe2);
    defs.appendChild(pattern);
    svg.appendChild(defs);

    // Card background. Fill and stroke come from CSS (--card-back-*), which
    // wins over any SVG presentation attribute anyway (css.md e4f5g6h7).
    const rect = document.createElementNS(SVG_NS, 'rect');
    rect.setAttribute('class', 'card-bg');
    rect.setAttribute('width', '250');
    rect.setAttribute('height', '350');
    rect.setAttribute('rx', '12');
    svg.appendChild(rect);

    // Center mark
    const tokens = getCardTokens();
    svg.appendChild(svgText('card-back-mark', '125', '175', tokens.backMark, {
        'text-anchor': 'middle',
        'dominant-baseline': 'central',
        'font-size': tokens.backMarkSize
    }));

    return svg;
}
