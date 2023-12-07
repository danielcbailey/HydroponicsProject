const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function(app) {
    app.use(
        '/api',
        createProxyMiddleware({
            target: 'http://dpcooper.me:8088',
            changeOrigin: false,
        })
    );
};