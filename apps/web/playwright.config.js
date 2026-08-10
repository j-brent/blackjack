// @ts-check
const { defineConfig } = require('@playwright/test');

// Override with BJ_PORT to run against a worktree-specific server.
const PORT = process.env.BJ_PORT || '8080';

module.exports = defineConfig({
    testDir: './tests',
    webServer: {
        command: `python -m http.server -d app ${PORT}`,
        url: `http://localhost:${PORT}`,
        // Deliberately never reuse an existing server: a process already
        // bound to this port may be serving a different worktree, which
        // would silently test the wrong code. Fail loudly instead.
        reuseExistingServer: false,
        stdout: 'ignore',
        stderr: 'pipe',
    },
});
