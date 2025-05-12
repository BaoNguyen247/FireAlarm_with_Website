# Dockerfile
FROM python:3.9-slim

WORKDIR /app

# Cài đặt các phụ thuộc hệ thống
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Sao chép requirements trước để tận dụng cache Docker
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Sao chép toàn bộ ứng dụng
COPY . .

# Thiết lập biến môi trường
ENV FLASK_APP=app.py
ENV FLASK_ENV=production
ENV PORT=8080

# Mở cổng
EXPOSE 8080

# Chạy ứng dụng
CMD ["gunicorn", "--bind", "0.0.0.0:8080", "--workers", "4", "--worker-class", "gevent", "app:app"]