from flask import Flask, request, jsonify

app = Flask(__name__)


@app.route('/warning', methods=['POST'])  # POST 방식으로 /warning 경로 설정
def warning():
    message = request.form.get('message')  # ESP32에서 전송하는 message 값
    timestamp = request.form.get('timestamp')  # ESP32에서 전송하는 timestamp 값

    if message and timestamp:
        print(f"Received message: {message} at {timestamp}")
        return jsonify({'status': 'success', 'message': 'Data received'}), 200
    else:
        return jsonify({'status': 'error', 'message': 'Missing data'}), 400


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # 로컬 네트워크에서 서버 실행
