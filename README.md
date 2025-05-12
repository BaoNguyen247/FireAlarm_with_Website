# Fire Alarm System - Flask Web Application on Google Cloud
Live Demo: https://firewebsite-226337333515.us-central1.run.app (login required but you can easy create your acount and login) ("Have fun!").
## Project Overview
The Fire Alarm System is a secure, real-time web application built with Flask, Python, Docker, Google Cloud SQL, and Google Cloud Run. This project provides a web interface to process and visualize fire detection data, store alert history in a MySQL database, and send email notifications for fire events. The application leverages HTTPS for secure communication and is deployed on Google Cloud Run for scalability.

- Developed a Flask-based web application to handle real-time fire detection data and display alert history via an HTTPS-enabled interface (`/history`).
- Implemented a Python backend with Flask to manage HTTPS requests, integrate with Google Cloud SQL, and automate email notifications.
- Containerized the application using Docker and deployed it on Google Cloud Run with optimized scalability.
- Configured secure database connectivity with Google Cloud SQL using `pymysql` for efficient data storage and retrieval.

## Features
- Real-time processing and visualization of fire detection data through a secure web interface.
- Storage of alert history in Google Cloud SQL with support for over 1,000 records.
- Automated email notifications sent within 5 seconds of a fire event detection.
- HTTPS and SSL encryption for secure data transmission and user authentication.
- Scalable deployment on Google Cloud Run, handling up to 100 concurrent requests.

## Technologies
- **Programming Language**: Python
- **Framework**: Flask
- **Database**: MySQL (Google Cloud SQL)
- **Containerization**: Docker
- **Cloud Platform**: Google Cloud Run
- **Frontend**: HTML/CSS
- **Security**: HTTPS, SSL
- **Libraries**: `pymysql`, `gunicorn`

## System Architecture
- **Web Application (Flask Backend)**:
  - Handles HTTPS requests and processes fire detection data.
  - Integrates with Google Cloud SQL for alert storage.
- **Database (Google Cloud SQL)**:
  - Stores alert history with efficient indexing and connection pooling.
- **Deployment (Google Cloud Run)**:
  - Containerized with Docker, deployed with environment variable configuration.
- **Notification System**:
  - Sends email alerts using Python scripts.

## Prerequisites
- Python 3.9+
- Docker
- Google Cloud SDK
- Google Cloud account with Cloud SQL and Cloud Run enabled
- MySQL client (for local testing)


