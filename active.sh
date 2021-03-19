#make the Script Shell
#!/bin/sh

comm_=${1}
echo input argv is ${comm_}     #check the input ARGV

#select clinet CNT      1 : use, 0 : not use
con_client_01=0 
con_client_02=0
con_client_03=0
con_client_04=0
con_client_cam=1

#select output server   1 : use, 0 : not use
con_output_server=1

#define IPC file name
server_tmp_name_01="/tmp/IPC_DCU";
server_tmp_name_02="/tmp/IPC_DCU2";
server_tmp_name_03="/tmp/IPC_DCU3";
server_tmp_name_04="/tmp/IPC_DCU4";
server_tmp_name_cam="/tmp/IPC_DCU5";
#define output server IP&port
output_server_name="192.168.5.82 7000";

# server_name="dcuServer  -s /tmp/IPC_DCU     -s /tmp/IPC_DCU2 -o 192.168.3.50 3000"
server_name="dcuServer"
pro_1="interClient -l 192.168.1.60 5000 -p 0 5 0 -r 0 0 0 -c /tmp/IPC_DCU_client -s ${server_tmp_name_01}"
pro_2="interClient -l 192.168.2.60 5001 -p 5 0 0 -r 0 0 90 -c /tmp/IPC_DCU_client2 -s ${server_tmp_name_02}"
pro_3="interClient -l 192.168.3.60 5002 -p 0 -5 0 -r 0 0 180 -c /tmp/IPC_DCU_client3 -s ${server_tmp_name_03}"  #need change
pro_4="interClient -l 192.168.4.60 5003 -p -5 0 0 -r 0 0 270 -c /tmp/IPC_DCU_client4 -s ${server_tmp_name_04}"  #need change

pro_cam="interClient -c /tmp/IPC_DCU_client5 -s ${server_tmp_name_cam}"  #camera

if [ ${con_client_01} == 1 ]
then
        server_name="${server_name} -s ${server_tmp_name_01}"
        echo "[Server] : ${server_name}"
        echo "[client] : ${pro_1}"
fi

if [ ${con_client_02} == 1 ]
then
        server_name="${server_name} -s ${server_tmp_name_02}"
        echo "[Server] : ${server_name}"
        echo "[client] : ${pro_2}"
fi

if [ ${con_client_03} == 1 ]
then
        server_name="${server_name} -s ${server_tmp_name_03}"
        echo "[Server] : ${server_name}"
        echo "[client] : ${pro_3}"
fi

if [ ${con_client_04} == 1 ]
then
        server_name="${server_name} -s ${server_tmp_name_04}"
        echo "[Server] : ${server_name}"
        echo "[client] : ${pro_4}"
fi

if [ ${con_client_cam} == 1 ]
then
        server_name="${server_name} -s ${server_tmp_name_cam}"
        echo "[Server] : ${server_name}"
        echo "[client] : ${pro_cam}"
fi

if [ ${con_output_server} == 1 ]
then
        server_name="${server_name} -o ${output_server_name}"
        echo "[Server] : ${server_name}"
fi

if [ -z ${comm_} ]      #노말한 동작 -- 인자가 없을 경우
then
        echo "program Activate!"
        #------------------프로세스 동작을 확인하여 종료-----------------------------
        ./stop.sh ${server_name}
        if [ ${con_client_01} == 1 ]
        then
                ./stop.sh ${pro_1}
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./stop.sh ${pro_2}
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./stop.sh ${pro_3}
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./stop.sh ${pro_4}
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./stop.sh ${pro_cam}
        fi
        sleep 3s                                #3초 대기
        
        
        if [ ${con_client_01} == 1 ]
        then
                ./interClient/build/${pro_1} &>/dev/null &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./interClient/build/${pro_2} &>/dev/null &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./interClient/build/${pro_3} &>/dev/null &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./interClient/build/${pro_4} &>/dev/null &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./interClient_camera/build/${pro_cam} &>/dev/null &      #프로세스를 백그라운드에서 실행
        fi

        ./interServer/build/${server_name} &>/dev/null &     #server는 그냥 실행

elif [ ${comm_} == "-terminal" ] || [ ${comm_} == "-t" ]
then
        echo treminal input     #새로운 터미널 창에서 각 printf문 출력을 확인하고자 할때
        #------------------프로세스 동작을 확인하여 종료-----------------------------
        ./stop.sh ${server_name}
        if [ ${con_client_01} == 1 ]
        then
                ./stop.sh ${pro_1}
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./stop.sh ${pro_2}
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./stop.sh ${pro_3}
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./stop.sh ${pro_4}
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./stop.sh ${pro_cam}
        fi
        sleep 3s
        
        if [ ${con_client_01} == 1 ]
        then
                gnome-terminal -- ./interClient/build/${pro_1} &    #프로세스 백그라운드 실행
        fi
        if [ ${con_client_02} == 1 ]
        then
                gnome-terminal -- ./interClient/build/${pro_2} &    # 프로세스 백그라운드 실행
        fi
        if [ ${con_client_03} == 1 ]
        then
                gnome-terminal -- ./interClient/build/${pro_3} &    # 프로세스 백그라운드 실행
        fi
        if [ ${con_client_04} == 1 ]
        then
                gnome-terminal -- ./interClient/build/${pro_4} &    # 프로세스 백그라운드 실행
        fi
        if [ ${con_client_cam} == 1 ]
        then
                gnome-terminal -- ./interClient_camera/build/${pro_cam} &    # 프로세스 백그라운드 실행
        fi
        gnome-terminal -- ./interServer/build/${server_name}    # 서버 실행

elif [ ${comm_} == "-debug" ] || [ ${comm_} == "-d" ]
then
        echo "program Activate - debug mode!"
        #------------------프로세스 동작을 확인하여 종료-----------------------------
        ./stop.sh ${server_name}
        if [ ${con_client_01} == 1 ]
        then
                ./stop.sh ${pro_1}
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./stop.sh ${pro_2}
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./stop.sh ${pro_3}
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./stop.sh ${pro_4}
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./stop.sh ${pro_cam}
        fi
        sleep 3s                                #3초 대기
        
        
        if [ ${con_client_01} == 1 ]
        then
                ./interClient/build/${pro_1} &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./interClient/build/${pro_2} &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./interClient/build/${pro_3} &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./interClient/build/${pro_4} &      #프로세스를 백그라운드에서 실행
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./interClient_camera/build/${pro_cam} &      #프로세스를 백그라운드에서 실행
        fi

        ./interServer/build/${server_name}              #server는 그냥 실행

elif [ ${comm_} == "-h" ] || [ ${comm_} == "-help" ]            # help 명령어
then
        echo *******CARNAVICOM LiDAR DCU COMMAND List************
        echo --- NULL : 
        echo            activate all Program
        echo ----------------------------------------------------
        echo --- -terminal / -t : 
        echo            activate all Program each Terminal window
        echo -----------------------------------------------------
        echo --- -stop / -s : 
        echo            stop/kill Program
        echo -----------------------------------------------------
        
elif [ ${comm_} == "-stop" ] || [ ${comm_} == "-s" ]    #프로세스 정지 명령어
then
        echo kill All Program
        #------------------프로세스 동작을 확인하여 종료-----------------------------
        ./stop.sh ${server_name}        #stop.sh를 호출하여 해당 프로세스를 종료
        if [ ${con_client_01} == 1 ]
        then
                ./stop.sh ${pro_1}
        fi
        if [ ${con_client_02} == 1 ]
        then
                ./stop.sh ${pro_2}
        fi
        if [ ${con_client_03} == 1 ]
        then
                ./stop.sh ${pro_3}
        fi
        if [ ${con_client_04} == 1 ]
        then
                ./stop.sh ${pro_4}
        fi
        if [ ${con_client_cam} == 1 ]
        then
                ./stop.sh ${pro_cam}
        fi
else
        echo Please input other Command
        echo comand [-h/-help]
fi      #if문 종료
