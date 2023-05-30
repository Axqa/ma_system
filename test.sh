#!/bin/bash

counter=0  

server="rcssserver"
monitor="rcssmonitor"
starter="./build/starter"
team_obs="./start.sh -t ObsTeam -a 6"
opponent="./start.sh -t Enemy"

log_file="./logs/log"
conf_file="./configs/conf"

serv_log_src="../logs"
serv_log_dest="./server_logs"

file_count=$(ls -1 "./configs" | wc -l)

while [ $counter -lt $file_count ]; do
  
  log="-l $log_file$counter.txt"
  conf="-c $conf_file$counter.conf" 

  # start server
  $server &
  # $monitor &
  sleep 2
  # echo $server 
  # start obs team
  $team_obs $log $conf &
  # echo $team_obs $log $conf
  sleep 1
  # start enemy team
  $opponent &
  # echo $opponent 

  # 10 secs for team init
  sleep 10

  $starter &
  # echo $starter 

  # 110 secs for match
  sleep 110

  # stop server
  killall $server
  # killall $monitor
  # echo "killall $server"
  sleep 5

  newest_file=$(ls -t "$serv_log_src" | head -1)

  # echo "cp $serv_log_src/$newest_file $serv_log_dest"
  # echo "mv $serv_log_dest/$newest_file $serv_log_dest/log$counter.rcg"
  cp "$serv_log_src/$newest_file" "$serv_log_dest"
  mv "$serv_log_dest/$newest_file" "$serv_log_dest/log$counter.rcg"

  # wait for teams to disconnect
  sleep 5

  find "./" -type f -name "*.rcl" -delete

  ((counter++))

done
