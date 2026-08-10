const { app, BrowserWindow, protocol, net } = require('electron');
const path = require('path');
const fs = require('fs');

const MIME_TYPES = {
    '.html': 'text/html',
    '.css': 'text/css',
    '.js': 'application/javascript',
    '.mjs': 'application/javascript',
    '.wasm': 'application/wasm',
    '.json': 'application/json',
    '.svg': 'image/svg+xml',
    '.png': 'image/png',
    '.ico': 'image/x-icon',
    '.woff2': 'font/woff2',
};

// Window background painted before the renderer loads. Matches each theme's
// --bg so launching does not flash a colour the chosen design never uses.
const THEME_BACKGROUNDS = {
    'noir-club': '#26231f',
    'midnight-tuxedo': '#211e1a',
    'champagne-playbill': '#242019',
    'monte-carlo-gold': '#1f1c17',
};
const DEFAULT_THEME = 'noir-club';

function getAppRoot() {
    if (app.isPackaged) {
        return path.join(process.resourcesPath, 'app');
    }
    return path.join(__dirname, '..', 'web', 'app');
}

// Must register before app.ready
protocol.registerSchemesAsPrivileged([{
    scheme: 'app',
    privileges: {
        standard: true,
        secure: true,
        supportFetchAPI: true,
        corsEnabled: true,
    }
}]);

// The renderer owns the theme (in localStorage), which the main process
// cannot read before the window exists. So we mirror the theme to userData
// after each load and use it to paint the *next* launch.
function themeCachePath() {
    return path.join(app.getPath('userData'), 'theme.json');
}

function readCachedTheme() {
    try {
        const { theme } = JSON.parse(fs.readFileSync(themeCachePath(), 'utf8'));
        return THEME_BACKGROUNDS[theme] ? theme : DEFAULT_THEME;
    } catch {
        return DEFAULT_THEME;
    }
}

function cacheTheme(theme) {
    if (!THEME_BACKGROUNDS[theme]) {
        return;
    }
    try {
        fs.writeFileSync(themeCachePath(), JSON.stringify({ theme }), 'utf8');
    } catch {
        // Non-fatal: the next launch just falls back to the default colour.
    }
}

function createWindow() {
    const win = new BrowserWindow({
        width: 480,
        height: 800,
        minWidth: 360,
        minHeight: 640,
        title: 'Blackjack',
        backgroundColor: THEME_BACKGROUNDS[readCachedTheme()],
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true,
        }
    });

    win.setMenuBarVisibility(false);
    win.loadURL('app://app/index.html');

    const captureTheme = async () => {
        if (win.isDestroyed()) {
            return;
        }
        try {
            cacheTheme(await win.webContents.executeJavaScript(
                'document.documentElement.dataset.theme'
            ));
        } catch {
            // Ignore — only affects the next launch's background colour.
        }
    };

    // On load, and again on blur so a theme picked mid-session is not lost
    // when the app is closed.
    win.webContents.on('did-finish-load', captureTheme);
    win.on('blur', captureTheme);
}

app.whenReady().then(() => {
    const appRoot = getAppRoot();

    protocol.handle('app', async (request) => {
        const url = new URL(request.url);
        const filePath = path.join(appRoot, decodeURIComponent(url.pathname));
        const resolved = path.resolve(filePath);

        if (!resolved.startsWith(path.resolve(appRoot))) {
            return new Response('Forbidden', { status: 403 });
        }

        const ext = path.extname(resolved).toLowerCase();
        const mimeType = MIME_TYPES[ext] || 'application/octet-stream';

        try {
            const data = await fs.promises.readFile(resolved);
            return new Response(data, {
                headers: { 'Content-Type': mimeType }
            });
        } catch {
            return new Response('Not Found', { status: 404 });
        }
    });

    createWindow();
});

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
        createWindow();
    }
});
