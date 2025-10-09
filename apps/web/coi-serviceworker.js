// Cross-Origin Isolation Service Worker
// 用于在静态文件服务器上启用 SharedArrayBuffer 支持
// 通过拦截请求并添加必要的 COOP/COEP 头部来实现跨域隔离

self.addEventListener('install', (event) => {
    console.log('[COI SW] Service Worker 安装中...');
    self.skipWaiting();
});

self.addEventListener('activate', (event) => {
    console.log('[COI SW] Service Worker 激活中...');
    event.waitUntil(clients.claim());
});

self.addEventListener('fetch', (event) => {
    const { request } = event;

    // 只处理同源请求
    if (request.url.startsWith(self.location.origin)) {
        event.respondWith(
            fetch(request)
                .then((response) => {
                    // 如果是导航请求或文档请求，添加 COOP/COEP 头部
                    if (request.mode === 'navigate' || request.destination === 'document' ||
                        request.destination === 'worker' || request.destination === 'sharedworker') {

                        const newHeaders = new Headers(response.headers);
                        newHeaders.set('Cross-Origin-Opener-Policy', 'same-origin');
                        newHeaders.set('Cross-Origin-Embedder-Policy', 'require-corp');

                        return new Response(response.body, {
                            status: response.status,
                            statusText: response.statusText,
                            headers: newHeaders
                        });
                    }

                    // 对于其他资源，确保允许跨域
                    const newHeaders = new Headers(response.headers);
                    if (!newHeaders.has('Cross-Origin-Resource-Policy')) {
                        newHeaders.set('Cross-Origin-Resource-Policy', 'cross-origin');
                    }

                    return new Response(response.body, {
                        status: response.status,
                        statusText: response.statusText,
                        headers: newHeaders
                    });
                })
                .catch((error) => {
                    console.error('[COI SW] 请求失败:', error);
                    return new Response('Service Worker fetch error', {
                        status: 500,
                        statusText: 'Service Worker Error'
                    });
                })
        );
    }
});
