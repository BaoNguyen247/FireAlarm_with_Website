const socket = io();
socket.on('alert', function(data) {
    const alertDiv = document.createElement('div');
    alertDiv.className = `alert alert-${data.status === 'fire' ? 'danger' : 'success'} alert-realtime`;
    alertDiv.innerHTML = `Cảnh báo: ${data.status.toUpperCase()} lúc ${data.timestamp} <button type="button" class="btn-close" data-bs-dismiss="alert"></button>`;
    document.body.appendChild(alertDiv);
    setTimeout(() => alertDiv.remove(), 5000);
});