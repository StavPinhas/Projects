#!/bin/bash


if [[ $# -ne 1 ]]; then #check for valid num of args
	echo "Wrong number of arguments" >&2 
	exit 1
fi
input=$(cat -)	
valid_rules=$(cat $1 | uniq | sed 's:#.*::g' | sed '/^$/d' | sed 's/ //g') 	
valid_adresses=""
#check every packet of rules and save the valid adresses
for rule in $valid_rules; do
	IFS=',' read -ra rules <<< "$rule"
	adress=$( echo $input |  sed 's/src-ip/\nsrc-ip/g' |\
	./firewall.exe "${rules[0]}" 2>/dev/null |\
	./firewall.exe "${rules[1]}" 2>/dev/null |\
	./firewall.exe "${rules[2]}" 2>/dev/null |\
	./firewall.exe "${rules[3]}" 2>/dev/null ) 

	valid_adresses+=$(echo $adress | sort | uniq |\
	sed 's:#.*$::g'| sed '/^$/d' | sed 's/ //g')
	
done  

	echo "$valid_adresses" | sed 's/src-ip/\nsrc-ip/g' | sed '1{/^$/d;}' | sort 

