from flask import Flask, request, render_template, redirect, url_for, jsonify, flash, make_response
from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user, current_user
from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, SubmitField
from wtforms.validators import DataRequired, Length, Regexp
from flask_limiter import Limiter
from flask_limiter.util import get_remote_address
from flask_socketio import SocketIO, emit
import pymysql
from datetime import datetime
import smtplib
from email.mime.text import MIMEText
from werkzeug.security import generate_password_hash, check_password_hash
import json
import os
import logging

# Thiết lập logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

app = Flask(__name__)
app.secret_key = 'hello247'  # Thay bằng key mạnh
app.config['SESSION_COOKIE_SECURE'] = True
app.config['SESSION_COOKIE_HTTPONLY'] = True
app.config['SESSION_COOKIE_SAMESITE'] = 'Lax'
app.config['PERMANENT_SESSION_LIFETIME'] = 1800

# Khởi tạo Flask-Login
login_manager = LoginManager()
login_manager.init_app(app)
login_manager.login_view = 'login'

# Khởi tạo CSRF protection
from flask_wtf.csrf import CSRFProtect
csrf = CSRFProtect(app)

# Khởi tạo Flask-Limiter
limiter = Limiter(key_func=get_remote_address, app=app)

# Khởi tạo Flask-SocketIO
socketio = SocketIO(app)

# Cấu hình email
EMAIL_ADDRESS = os.getenv('EMAIL_ADDRESS', 'firealarm2478@gmail.com')
EMAIL_PASSWORD = os.getenv('EMAIL_PASSWORD', 'a0909771613z')  # App Password của Gmail
SMTP_SERVER = 'smtp.gmail.com'
SMTP_PORT = 587

# Cấu hình MySQL với Cloud SQL Proxy
if os.getenv('GAE_ENV', '').startswith('standard') or os.getenv('K_SERVICE'):  # Cloud Run/App Engine
    DB_HOST = '/cloudsql/INSTANCE_CONNECTION_NAME'  # Ví dụ: your-project:region:fire-alarm-db
    DB_USER = os.getenv('DB_USER', 'root')
    DB_PASSWORD = os.getenv('DB_PASSWORD', '1')
    DB_NAME = os.getenv('DB_NAME', 'fire_alarm')
else:  # Local test
    DB_HOST = os.getenv('DB_HOST', '34.31.179.71')  # Thay bằng IP thực tế
    DB_USER = os.getenv('DB_USER', 'root')
    DB_PASSWORD = os.getenv('DB_PASSWORD', '1')
    DB_NAME = os.getenv('DB_NAME', 'fire_alarm')

# Hàm kết nối MySQL
def get_db_connection():
    return pymysql.connect(
        host=DB_HOST,
        user=DB_USER,
        password=DB_PASSWORD,
        database=DB_NAME,
        charset='utf8mb4',
        cursorclass=pymysql.cursors.DictCursor
    )

# Lớp User cho Flask-Login
class User(UserMixin):
    def __init__(self, id, username):
        self.id = id
        self.username = username

# Form đăng nhập
class LoginForm(FlaskForm):
    username = StringField('Tên người dùng', validators=[DataRequired(), Length(min=3, max=20)])
    password = PasswordField('Mật khẩu', validators=[DataRequired()])
    submit = SubmitField('Đăng nhập')

# Form đăng ký
class RegisterForm(FlaskForm):
    username = StringField('Tên người dùng', validators=[DataRequired(), Length(min=3, max=20)])
    password = PasswordField('Mật khẩu', validators=[
        DataRequired(),
        Length(min=8, message='Mật khẩu phải dài ít nhất 8 ký tự'),
        Regexp(r'^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{8,}$',
               message='Mật khẩu phải chứa chữ hoa, chữ thường, số và ký tự đặc biệt')
    ])
    submit = SubmitField('Đăng ký')

# Tải user từ database
@login_manager.user_loader
def load_user(user_id):
    conn = get_db_connection()
    with conn.cursor() as c:
        c.execute("SELECT id, username FROM users WHERE id = %s", (user_id,))
        user_data = c.fetchone()
    conn.close()
    if user_data:
        return User(user_data['id'], user_data['username'])
    return None

# Khởi tạo database
def init_db():
    conn = get_db_connection()
    with conn.cursor() as c:
        c.execute('''CREATE TABLE IF NOT EXISTS alerts 
                     (id INT AUTO_INCREMENT PRIMARY KEY, 
                      status VARCHAR(255), 
                      timestamp VARCHAR(255))''')
        c.execute('''CREATE TABLE IF NOT EXISTS users 
                     (id INT AUTO_INCREMENT PRIMARY KEY, 
                      username VARCHAR(255) UNIQUE, 
                      password VARCHAR(255))''')
    conn.commit()
    conn.close()

# Gửi email thông báo
def send_email(status, timestamp):
    if not EMAIL_ADDRESS:
        logger.error("EMAIL_ADDRESS is not set. Cannot send email.")
        return
    msg = MIMEText(f'Fire Alarm Alert!\nStatus: {status}\nTime: {timestamp}')
    msg['Subject'] = 'Fire Alarm Notification'
    msg['From'] = EMAIL_ADDRESS
    msg['To'] = EMAIL_ADDRESS
    try:
        with smtplib.SMTP(SMTP_SERVER, SMTP_PORT, timeout=10) as server:
            server.starttls()
            server.login(EMAIL_ADDRESS, EMAIL_PASSWORD)
            server.send_message(msg)
        logger.info(f"Email sent successfully to {EMAIL_ADDRESS}")
    except Exception as e:
        logger.error(f"Error sending email: {e}")

# Phân tích nguy cơ cháy
def analyze_risk():
    conn = get_db_connection()
    with conn.cursor() as c:
        c.execute("SELECT status FROM alerts WHERE timestamp >= NOW() - INTERVAL 1 DAY")
        alerts = c.fetchall()
    conn.close()
    total = len(alerts)
    fire_count = sum(1 for alert in alerts if alert['status'] == 'fire')
    risk = fire_count / total if total > 0 else 0
    if risk > 0.5:
        return 'Cao', 'danger'
    elif risk > 0.2:
        return 'Trung bình', 'warning'
    else:
        return 'Thấp', 'success'

# Khởi tạo database
init_db()

# Route đăng nhập
@app.route('/login', methods=['GET', 'POST'])
@limiter.limit("10 per minute")
def login():
    if current_user.is_authenticated:
        return redirect(url_for('index'))
    form = LoginForm()
    if form.validate_on_submit():
        username = form.username.data
        password = form.password.data
        conn = get_db_connection()
        with conn.cursor() as c:
            c.execute("SELECT id, username, password FROM users WHERE username = %s", (username,))
            user = c.fetchone()
        conn.close()
        if user and check_password_hash(user['password'], password):
            login_user(User(user['id'], user['username']))
            return redirect(url_for('index'))
        flash('Tên người dùng hoặc mật khẩu không đúng')
    response = make_response(render_template('login.html', form=form))
    response.headers['Cache-Control'] = 'no-store, no-cache'
    return response

# Route đăng xuất
@app.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('login'))

# Route đăng ký
@app.route('/register', methods=['GET', 'POST'])
@limiter.limit("5 per hour")
def register():
    form = RegisterForm()
    if form.validate_on_submit():
        username = form.username.data
        password = generate_password_hash(form.password.data, method='pbkdf2:sha256')
        conn = get_db_connection()
        try:
            with conn.cursor() as c:
                c.execute("INSERT INTO users (username, password) VALUES (%s, %s)", (username, password))
            conn.commit()
            flash('Đăng ký thành công! Vui lòng đăng nhập.')
            return redirect(url_for('login'))
        except pymysql.err.IntegrityError:
            flash('Tên người dùng đã tồn tại')
        finally:
            conn.close()
    response = make_response(render_template('login.html', form=form, register=True))
    response.headers['Cache-Control'] = 'no-store, no-cache'
    return response

# Route nhận dữ liệu từ ESP32
@app.route('/data', methods=['POST'])
@csrf.exempt
@limiter.limit("10 per minute")
def receive_data():
    try:
        data = request.get_json()
        status = data.get('status', 'unknown')
        conn = get_db_connection()
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        with conn.cursor() as c:
            c.execute("INSERT INTO alerts (status, timestamp) VALUES (%s, %s)", (status, timestamp))
        conn.commit()
        conn.close()
        if status == 'fire':
            send_email(status, timestamp)
        socketio.emit('alert', {'status': status, 'timestamp': timestamp})
        return jsonify({'message': 'Data received', 'status': status}), 200
    except Exception as e:
        logger.error(f"Error in receive_data: {e}")
        return jsonify({'error': str(e)}), 500

# Route trang chính
@app.route('/')
@login_required
def index():
    conn = get_db_connection()
    with conn.cursor() as c:
        c.execute("SELECT status, timestamp FROM alerts ORDER BY id DESC LIMIT 1")
        latest = c.fetchone()
        latest_status = latest['status'] if latest else 'unknown'
        latest_timestamp = latest['timestamp'] if latest else 'N/A'

        c.execute("SELECT status, COUNT(*) FROM alerts WHERE timestamp >= NOW() - INTERVAL 1 DAY GROUP BY status")
        chart_data = c.fetchall()
        chart_labels = [row['status'] for row in chart_data]
        chart_values = [row['COUNT(*)'] for row in chart_data]

        c.execute("SELECT HOUR(timestamp) as hour, COUNT(*) FROM alerts WHERE timestamp >= NOW() - INTERVAL 1 DAY AND status = 'fire' GROUP BY HOUR(timestamp)")
        trend_data = c.fetchall()
        trend_labels = [f"{int(row['hour']):02d}:00" for row in trend_data]
        trend_values = [row['COUNT(*)'] for row in trend_data]
    conn.close()
    risk_level, risk_color = analyze_risk()
    response = make_response(render_template('index.html',
                                             status=latest_status,
                                             timestamp=latest_timestamp,
                                             chart_labels=json.dumps(chart_labels),
                                             chart_values=json.dumps(chart_values),
                                             trend_labels=json.dumps(trend_labels),
                                             trend_values=json.dumps(trend_values),
                                             risk_level=risk_level,
                                             risk_color=risk_color))
    response.headers['Cache-Control'] = 'no-store, no-cache'
    return response

# Route lịch sử
@app.route('/history', methods=['GET'])
@login_required
def history():
    page = request.args.get('page', 1, type=int)
    per_page = 10
    search_status = request.args.get('status', '')
    search_start = request.args.get('start', '')
    search_end = request.args.get('end', '')

    conn = get_db_connection()
    query = "SELECT status, timestamp FROM alerts WHERE 1=1"
    params = []

    if search_status:
        query += " AND status = %s"
        params.append(search_status)
    if search_start:
        query += " AND timestamp >= %s"
        params.append(search_start)
    if search_end:
        query += " AND timestamp <= %s"
        params.append(search_end)

    # Phân trang thủ công
    offset = (page - 1) * per_page
    query += " ORDER BY timestamp DESC LIMIT %s OFFSET %s"
    params.extend([per_page, offset])

    with conn.cursor() as c:
        c.execute(query, params)
        history = c.fetchall()

        count_query = "SELECT COUNT(*) FROM alerts WHERE 1=1" + query.split("WHERE 1=1")[1].split("LIMIT")[0]
        c.execute(count_query, params[:-2])
        total = c.fetchone()['COUNT(*)']
    conn.close()
    total_pages = (total + per_page - 1) // per_page
    return render_template('history.html', history=history, page=page, total_pages=total_pages,
                           search_status=search_status, search_start=search_start, search_end=search_end)

# API lịch sử
@app.route('/api/history', methods=['GET'])
@login_required
def api_history():
    start_time = request.args.get('start')
    end_time = request.args.get('end')
    conn = get_db_connection()
    query = "SELECT status, timestamp FROM alerts WHERE 1=1"
    params = []
    if start_time:
        query += " AND timestamp >= %s"
        params.append(start_time)
    if end_time:
        query += " AND timestamp <= %s"
        params.append(end_time)
    query += " ORDER BY timestamp DESC"
    with conn.cursor() as c:
        c.execute(query, params)
        data = c.fetchall()
    conn.close()
    return jsonify([{'status': row['status'], 'timestamp': row['timestamp'].strftime('%Y-%m-%d %H:%M:%S')} for row in data])

# WebSocket event
@socketio.on('connect')
def handle_connect():
    logger.info('Client connected')

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 8080))
    app.run(host="0.0.0.0", port=port)


