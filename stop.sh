#스크립트 쉘을 만듦니다.
#!/bin/sh

program=${1}

#백그라운드 동작 중인지 확인
# ps -ef을 이용해서 원하는 프로세스 정보를 얻는다.
var1=$(ps -ef | grep ${program})
# pid를 얻는다. (공백으로 잘라서, 두번째 argument)
second1=$(echo ${var1} | cut -d " " -f2)
find_grep=$(echo ${var1} | cut -d " " -f8)
# pid가 존재할 경우 프로세스를 kill 한다.
# -n 스트링은, 문자열 길이가 0 이 아닐 경우 true를 리턴한다.

if [ -n "${second1}" ]
then
        if [ ${find_grep} != "grep" ]   #grep을 찾는다면 이는 grep 명령어에 의해 실행된 프로세스
        then
        result1=$(kill -9 ${second1})
        echo process is killed.
        fi
else
        echo running process not found.
fi
#------------------프로세스 동작을 확인하여 종료-----------------------------