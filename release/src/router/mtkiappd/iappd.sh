#!/bin/sh

start() {   
	if grep -q 'rlt_wifi_7603e' /proc/modules ; then
	    ralinkiappd -wi rai0 &
		echo 10000 > /proc/sys/net/ipv4/neigh/rai0/base_reachable_time_ms
		echo 1 > /proc/sys/net/ipv4/neigh/rai0/delay_first_probe_time
	else
	    if grep -q 'rai0' /proc/interrupts; then
			ralinkiappd -wi rai0 -wi ra0 &
			echo 10000 > /proc/sys/net/ipv4/neigh/rai0/base_reachable_time_ms
			echo 1 > /proc/sys/net/ipv4/neigh/rai0/delay_first_probe_time
	    else
	    	ralinkiappd -wi rax0 -wi ra0 &
			echo 10000 > /proc/sys/net/ipv4/neigh/rax0/base_reachable_time_ms
			echo 1 > /proc/sys/net/ipv4/neigh/rax0/delay_first_probe_time
	    fi
	fi
	echo 10000 > /proc/sys/net/ipv4/neigh/br0/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/br0/delay_first_probe_time
	echo 10000 > /proc/sys/net/ipv4/neigh/ra0/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/ra0/delay_first_probe_time
	echo 10000 > /proc/sys/net/ipv4/neigh/eth2/base_reachable_time_ms
	echo 1 > /proc/sys/net/ipv4/neigh/eth2/delay_first_probe_time
    iptables -A INPUT -i br0 -p tcp --dport 3517 -j ACCEPT
    iptables -A INPUT -i br0 -p udp --dport 3517 -j ACCEPT 
}





stop() {
    pid=`pidof ralinkiappd`
    if [ "$pid" != "" ]; then
        killall -q  ralinkiappd
		sleep 1
		killall -q  ralinkiappd
		sleep 1
    fi
    
}


case "$1" in
        start)
            start
            ;;

        stop)
            stop
            ;;

        restart)
            stop
            start
            ;;

        *)
            echo $"Usage: $0 {start|stop|restart}"
esac
