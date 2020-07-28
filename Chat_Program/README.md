# Chatting Program
## 1. intro
Networking Programming Term Project로 Multiple Chatting Server & Client Program을 구현하였다.
Port Number는 3500으로 고정하였고 select method를 사용하였다.

### 1) Server Implementation
* Client 접속 시 해당 Client에게 welcome message를 출ㄹ력하고 기존에 접속 중인 Client에게 접속 message를 전송
* Multiple Client login 처리 및 Client가 보낸 message를 다른 Client에게도 전송.
* System message '@show' 는 해당 Client에게 현재 접속된 Client 수와 ID 명을 출력.
* System message '@exit' 는 헤당 Client와 접속을 종료.

### 2) Client Implementation
* Server IP와 Chatting ID는 인수로 전송할 것.
* 실행 방법 : chat_client 127.0.0.1 통돌이
* 본인이 원하는 방법으로 구현 할 것.

## 2. Flow Chart


## 3. Result

/home/winelab/test-2020-07-28_21.05.47.mkv
