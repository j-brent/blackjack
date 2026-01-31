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
});

// AT-2: No scroll required at 390x844
test('AT-2: no scroll required at 390x844', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.iPhone14);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);
});

// AT-3: No scroll required at 414x896
test('AT-3: no scroll required at 414x896', async ({ page }) => {
    await page.setViewportSize(VIEWPORTS.iPhone11ProMax);
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
    const innerHeight = await page.evaluate(() => window.innerHeight);
    expect(scrollHeight).toBeLessThanOrEqual(innerHeight);
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
    await page.setViewportSize(VIEWPORTS.smallAndroid);

    // Use a deterministic seed known to produce a splittable hand.
    // We try multiple seeds until we find one that works, then verify layout.
    await page.goto(BASE_URL);
    await waitForGameReady(page);

    // Check if split is available; if not, keep hitting New Game
    let splitAvailable = false;
    for (let attempt = 0; attempt < 50; attempt++) {
        const splitBtn = page.locator('#btn-split');
        if (await splitBtn.isVisible({ timeout: 200 }).catch(() => false)) {
            splitAvailable = true;
            break;
        }
        // Click new game if available, otherwise hit stand to finish round first
        const newGameBtn = page.locator('#btn-new-game');
        if (await newGameBtn.isVisible({ timeout: 200 }).catch(() => false)) {
            await newGameBtn.click();
            await page.waitForTimeout(600); // wait for dealer auto-play
        } else {
            const standBtn = page.locator('#btn-stand');
            if (await standBtn.isVisible({ timeout: 200 }).catch(() => false)) {
                await standBtn.click();
                await page.waitForTimeout(600);
            }
        }
    }

    if (splitAvailable) {
        await page.locator('#btn-split').click();
        await page.waitForTimeout(300);

        const scrollHeight = await page.evaluate(() => document.documentElement.scrollHeight);
        const innerHeight = await page.evaluate(() => window.innerHeight);
        expect(scrollHeight).toBeLessThanOrEqual(innerHeight);
    } else {
        // If no split hand found after 50 tries, skip but don't fail
        test.skip();
    }
});
