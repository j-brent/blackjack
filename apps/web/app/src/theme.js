/**
 * Theme selection — persists the chosen design and drives the picker menu.
 *
 * The theme is stamped onto <html data-theme> by an inline script in
 * index.html before first paint; this module only handles changes made
 * after load.
 */

export const THEMES = [
    { id: 'noir-club', label: 'Noir Club' },
    { id: 'midnight-tuxedo', label: 'Midnight Tuxedo' },
    { id: 'champagne-playbill', label: 'Champagne Playbill' },
    { id: 'monte-carlo-gold', label: 'Monte Carlo Gold' }
];

export const DEFAULT_THEME = THEMES[0].id;

const STORAGE_KEY = 'bj-theme';

function isKnown(id) {
    return THEMES.some(theme => theme.id === id);
}

/**
 * Currently applied theme id.
 */
export function getTheme() {
    const current = document.documentElement.dataset.theme;
    return isKnown(current) ? current : DEFAULT_THEME;
}

/**
 * Apply a theme and persist it. Unknown ids fall back to the default.
 */
export function applyTheme(id) {
    const theme = isKnown(id) ? id : DEFAULT_THEME;
    document.documentElement.dataset.theme = theme;

    // Private browsing and disabled storage both throw; the theme still
    // applies for this session.
    try {
        localStorage.setItem(STORAGE_KEY, theme);
    } catch (error) {
        // Non-fatal — persistence is a nicety, not a requirement.
    }

    return theme;
}

/**
 * Wire up the picker button and menu.
 */
export function setupThemePicker() {
    const button = document.getElementById('theme-button');
    const menu = document.getElementById('theme-menu');
    if (!button || !menu) {
        return;
    }

    const items = [...menu.querySelectorAll('[data-theme]')];

    function syncChecked() {
        const current = getTheme();
        items.forEach(item => {
            item.setAttribute('aria-checked', String(item.dataset.theme === current));
        });
    }

    function openMenu() {
        menu.hidden = false;
        button.setAttribute('aria-expanded', 'true');
        const checked = items.find(item => item.getAttribute('aria-checked') === 'true');
        (checked || items[0]).focus();
    }

    function closeMenu({ refocus = true } = {}) {
        if (menu.hidden) {
            return;
        }
        menu.hidden = true;
        button.setAttribute('aria-expanded', 'false');
        if (refocus) {
            button.focus();
        }
    }

    button.addEventListener('click', () => {
        if (menu.hidden) {
            openMenu();
        } else {
            closeMenu();
        }
    });

    menu.addEventListener('click', event => {
        const item = event.target.closest('[data-theme]');
        if (!item) {
            return;
        }
        applyTheme(item.dataset.theme);
        syncChecked();
        closeMenu();
    });

    // Escape closes; clicking or tabbing away closes without stealing focus.
    document.addEventListener('keydown', event => {
        if (event.key === 'Escape' && !menu.hidden) {
            event.preventDefault();
            event.stopPropagation();
            closeMenu();
        }
    });

    document.addEventListener('pointerdown', event => {
        if (!menu.hidden && !event.target.closest('#theme-picker')) {
            closeMenu({ refocus: false });
        }
    });

    // Deliberately no focusin handler: the game moves focus to an action
    // button after every deal, which would slam the menu shut a moment
    // after it opened. Pointer-away and Escape are enough.

    syncChecked();
}
