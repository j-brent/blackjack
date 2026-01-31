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
};

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

function createWindow() {
    const win = new BrowserWindow({
        width: 480,
        height: 800,
        minWidth: 360,
        minHeight: 640,
        title: 'Blackjack',
        backgroundColor: '#2a2a2a',
        webPreferences: {
            nodeIntegration: false,
            contextIsolation: true,
        }
    });

    win.setMenuBarVisibility(false);
    win.loadURL('app://app/index.html');
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
