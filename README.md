BGNL
====
Battleship Game Network Library
====

NHN NEXT 남세현, 박민수 학생이 만든 배틀쉽게임 네트워크 라이브러리 및 서버입니다.

네트워크 라이브러리
====
BattleshipNetworkLib 폴더의 Example 프로젝트를 참고해주세요.

서버
====
EasyGameServer 기반으로 만들어진 게임 서버이며
(https://github.com/zeliard/EasyGameServer)
Direct2D로 접속한 사람들의 이름과, 직접 대전을 붙일 수 있는 UI가 만들어져 있습니다.

자동으로 대전을 붙이기 위해서는 AutoMatcher클래스를,
선택적으로 대전을 붙이면서 UI를 이용하시려면 BattleBoard클래스를 활성화시켜 주시면 됩니다.
(현재는 BattleBoard 클래스가 활성화 되어 있습니다.)

학생들에게 공지사항
====
10.73.42.117로 접속하세요

사용 방법이 여러가지로 바뀌었으니 새로 받으시고 ppt를 읽어주세요

수정사항이 있어서 패치했습니다. 다들  다시 다운받아주세요.

예시 코드는 Example.cpp입니다. 이전과 다르게 컴파일하면 돌아갑니다. 더미클라 프로젝트는 삭제하고 이것으로 대체합니다.

좌표는 ('a','1')~('h','8') , ('A','1')~('H','8') 혹은 (0,0)~(7,7) 입니다.

Q. AttackResult에 있는 Short형은 뭔가요?
A. PacketType.h 에 보시면 AttackResult값들이 나와있습니다. 네트워크에서는 Enum값을 바로 보내지 못하기 때문에, Short형에 넣어서 보낸 것입니다. 그러므로 해당 Enum값 보시고 사용하시면 됩니다.
