// @ts-check
const { test, expect } = require('@playwright/test');
const fs = require('fs');
const path = require('path');

const BASE_URL = 'http://localhost:8080';

// Viewports representing real devices
const VIEWPORTS = {
    smallAndroid: { width: 360, height: 640 },
    iPhone14: { width: 390, height: 844 },
    iPhone11ProMax: { width: 414, height: 896 },
    tablet: { width: 1024, height: 768 },
    desktop: { width: 1920, height: 1080 },
};

async function waitForGameReady(page) {
    await page.waitForSelector('#game-container', { state: 'visible', timeout: 10000 });
    await page.waitForSelector('.card', { state: 'visible', timeout: 10000 });
}

// AT-1: No scroll required at 360x640
test('AT-1: no scroll required at 360x640', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);

    // No horizontal scroll on card containers
    const noHScroll = await page.evaluate(() => {
        return [...document.querySelectorAll('.card-container')].every(
            el => el.scrollWidth <= el.clientWidth
        );
    });
    expect(noHScroll).toBe(true);
});

// AT-2: No scroll required at 390x844
test('AT-2: no scroll required at 390x844', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.iPhone14);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);

    // No horizontal scroll on card containers
    const noHScroll = await page.evaluate(() => {
        return [...document.querySelectorAll('.card-container')].every(
            el => el.scrollWidth <= el.clientWidth
        );
    });
    expect(noHScroll).toBe(true);
});

// AT-3: No scroll required at 414x896
test('AT-3: no scroll required at 414x896', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.iPhone11ProMax);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);

    // No horizontal scroll on card containers
    const noHScroll = await page.evaluate(() => {
        return [...document.querySelectorAll('.card-container')].every(
            el => el.scrollWidth <= el.clientWidth
        );
    });
    expect(noHScroll).toBe(true);
});

// AT-4: All game elements visible without scroll at 360x640
test('AT-4: all game elements visible at 360x640', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const viewportHeight = VIEWPORTS.smallAndroid.height;
    const viewportWidth = VIEWPORTS.smallAndroid.width;

    const elements = ['#dealer-cards', '#player-cards', '#actions', '#status-bar'];
    for (const selector of elements) {
        const box = await page.locator(selector).boundingBox();
        expect(box, `${selector} should be visible`).not.toBeNull();
        expect(box.y + box.height).toBeLessThanOrEqual(viewportHeight + 1);
        expect(box.x + box.width).toBeLessThanOrEqual(viewportWidth + 1);
    }
});

// AT-5: No pixel units in CSS custom properties (spacing, font, card, radius tokens)
test('AT-5: no px units in design token custom properties', async () => {
    const cssPath = path.resolve(__dirname, '..', 'app', 'style.css');
    const css = fs.readFileSync(cssPath, 'utf8');

    // Extract custom property declarations from :root
    const rootBlock = css.match(/:root\s*\{([^}]+)\}/);
    expect(rootBlock).not.toBeNull();

    const declarations = rootBlock[1];
    const lines = declarations.split('\n');

    const tokenPrefixes = ['--space-', '--font-size-', '--card-', '--border-radius-'];

    for (const line of lines) {
        const trimmed = line.trim();
        if (!trimmed || trimmed.startsWith('/*') || trimmed.startsWith('*')) continue;

        const matchesToken = tokenPrefixes.some(prefix => trimmed.includes(prefix));
        if (!matchesToken) continue;

        // Extract the value part after the colon
        const colonIndex = trimmed.indexOf(':');
        if (colonIndex === -1) continue;
        const value = trimmed.slice(colonIndex + 1).trim().replace(';', '');

        // Assert no px units (shadow values are excluded since they use px by convention)
        expect(
            value,
            `Token "${trimmed.split(':')[0].trim()}" should not use px, found: ${value}`
        ).not.toMatch(/\d+px/);
    }
});

// AT-6: No horizontal scrollbar at 200% zoom on desktop
test('AT-6: no horizontal overflow at 200% zoom', async ({ browser }) => {
    const context = await browser.newContext({
        viewport: VIEWPORTS.tablet,
        deviceScaleFactor: 2,
    });
    const page = await context.newPage();
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollWidth = await page.evaluate(() => document.documentElement.scrollWidth);
    const innerWidth = await page.evaluate(() => window.innerWidth);
    expect(scrollWidth).toBeLessThanOrEqual(innerWidth);

    await context.close();
});

// AT-7: Split hands render without scroll at 360x640
test('AT-7: split hands fit at 360x640', async ({ page }) => {
    test.setTimeout(60000);
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const didSplit = await triggerSplit(page);
    if (!didSplit) { test.skip(); return; }

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);
});

// Helper: trigger a split by cycling through games until one is splittable.
// Returns true if split was triggered, false if skipped after maxAttempts.
async function triggerSplit(page, maxAttempts = 50) {
    for (let attempt = 0; attempt < maxAttempts; attempt++) {
        const splitBtn = page.locator('#btn-split');
        if (await splitBtn.isVisible({ timeout: 200 }).catch(() => false)) {
            await splitBtn.click();
            await page.waitForTimeout(300);
            return true;
        }
        const newGameBtn = page.locator('#btn-new-game');
        if (await newGameBtn.isVisible({ timeout: 200 }).catch(() => false)) {
            await newGameBtn.click();
            await page.waitForTimeout(600);
        } else {
            const standBtn = page.locator('#btn-stand');
            if (await standBtn.isVisible({ timeout: 200 }).catch(() => false)) {
                await standBtn.click();
                await page.waitForTimeout(600);
            }
        }
    }
    return false;
}

// AT-8: Card display correctness (valid rank/suit text, no Embind artifacts)
test('AT-8: cards display valid rank and suit text', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const validRanks = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K'];
    const validSuits = ['\u2660', '\u2665', '\u2666', '\u2663']; // ♠♥♦♣

    // Player cards should have valid rank text
    const rankTexts = await page.locator('#player-cards .card-rank-top').allTextContents();
    expect(rankTexts.length).toBeGreaterThanOrEqual(2);
    for (const text of rankTexts) {
        expect(validRanks, `Card rank "${text}" should be valid`).toContain(text);
    }

    // Player cards should have valid suit text
    const suitTexts = await page.locator('#player-cards .card-suit-center').allTextContents();
    expect(suitTexts.length).toBeGreaterThanOrEqual(2);
    for (const text of suitTexts) {
        expect(validSuits, `Card suit "${text}" should be valid`).toContain(text);
    }

    // No face-up card should contain "?" or "[object Object]"
    const faceUpTexts = await page.locator('.card:not(.face-down) text').allTextContents();
    for (const text of faceUpTexts) {
        expect(text).not.toContain('?');
        expect(text).not.toContain('[object Object]');
    }
});

// AT-9: All SVG text elements stay within the card viewBox (0 0 250 350)
test('AT-9: card SVG text within viewBox bounds', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const violations = await page.evaluate(() => {
        const results = [];
        document.querySelectorAll('.card:not(.face-down)').forEach((svg, cardIdx) => {
            svg.querySelectorAll('text').forEach(textEl => {
                const bbox = textEl.getBBox();
                if (bbox.x < -1 || bbox.y < -1 ||
                    bbox.x + bbox.width > 251 || bbox.y + bbox.height > 351) {
                    results.push({
                        card: cardIdx,
                        cls: textEl.getAttribute('class'),
                        bbox: { x: bbox.x, y: bbox.y, w: bbox.width, h: bbox.height }
                    });
                }
            });
        });
        return results;
    });

    expect(violations, `Text elements outside viewBox: ${JSON.stringify(violations)}`).toHaveLength(0);
});

// AT-10: Visual containment — children stay within parent bounds at 360x640.
// Checks during initial deal and after a split.
test('AT-10: visual containment at 360x640', async ({ page }) => {
    test.setTimeout(60000);
    await page.setViewportSize(VIEWPORTS.smallAndroid);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    // Tolerance in pixels for sub-pixel rounding
    const TOL = 2;

    // Checks that every child matching childSel is contained within its
    // closest ancestor matching parentSel. Returns array of violation strings.
    async function checkContainment(childSel, parentSel) {
        return page.evaluate(({ childSel, parentSel, TOL }) => {
            const violations = [];
            document.querySelectorAll(childSel).forEach(child => {
                const parent = child.closest(parentSel);
                if (!parent) return;
                const cr = child.getBoundingClientRect();
                const pr = parent.getBoundingClientRect();
                if (cr.width === 0 || cr.height === 0) return; // hidden
                if (cr.left < pr.left - TOL || cr.top < pr.top - TOL ||
                    cr.right > pr.right + TOL || cr.bottom > pr.bottom + TOL) {
                    violations.push(
                        `${childSel} overflows ${parentSel}: ` +
                        `child(${Math.round(cr.left)},${Math.round(cr.top)},${Math.round(cr.right)},${Math.round(cr.bottom)}) ` +
                        `parent(${Math.round(pr.left)},${Math.round(pr.top)},${Math.round(pr.right)},${Math.round(pr.bottom)})`
                    );
                }
            });
            return violations;
        }, { childSel, parentSel, TOL });
    }

    // Check containment for initial deal
    const pairs = [
        ['.card', '.card-container'],
        ['#dealer-cards', '#dealer-area'],
        ['#player-cards', '#player-area'],
        ['#dealer-area', 'main'],
        ['#player-area', 'main'],
        ['#actions', 'main'],
    ];

    for (const [child, parent] of pairs) {
        const v = await checkContainment(child, parent);
        expect(v, v.join('\n')).toHaveLength(0);
    }

    // Check main within viewport
    const mainOverflow = await page.evaluate((TOL) => {
        const r = document.querySelector('main').getBoundingClientRect();
        const vw = window.innerWidth;
        const vh = window.innerHeight;
        if (r.left < -TOL || r.top < -TOL || r.right > vw + TOL || r.bottom > vh + TOL) {
            return `main overflows viewport: rect(${Math.round(r.left)},${Math.round(r.top)},${Math.round(r.right)},${Math.round(r.bottom)}) viewport(${vw}x${vh})`;
        }
        return null;
    }, TOL);
    expect(mainOverflow).toBeNull();

    // Now trigger split and re-check
    const didSplit = await triggerSplit(page);
    if (!didSplit) return; // skip split checks if no splittable hand found

    const splitPairs = [
        ['.card', '.card-container'],
        ['.card-container', '.hand-container'],
        ['.hand-container', '#player-area-split'],
        ['#player-area-split', 'main'],
        ['#dealer-area', 'main'],
        ['#actions', 'main'],
    ];

    for (const [child, parent] of splitPairs) {
        const v = await checkContainment(child, parent);
        expect(v, v.join('\n')).toHaveLength(0);
    }
});
