#!/bin/sh
cd ../www
for i in $(find *.dict)
do
	if [ "$i" = "CN.dict" ];then
		echo "Softcenter=软件中心" >> $i
		echo "Softcenter_tool=工具箱" >> $i
	elif [ "$i" = "TW.dict" ];then
		echo "Softcenter=軟體中心" >> $i
		echo "Softcenter_tool=工具箱" >> $i
	else
		echo "Softcenter=Softcenter" >> $i
		echo "Softcenter_tool=Tools" >> $i
	fi
done

