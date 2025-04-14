make clean all && ndisasm -b64 -o0x10000 tiny.calm && ls -l tiny.* && (./tiny.calm ; echo $?)
sudo dmesg -c
