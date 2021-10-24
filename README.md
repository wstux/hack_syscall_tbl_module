hack_syscall_tbl_module
=======================

Hack_syscall_tbl is a LKM rootkit for Linux Kernels 4.x(>=4.4.0)/5.x

Features
--
- Sending s signal 42 to 32742 pid make the module become (in)visible;

Install
--
Compile
```
make
```
Load the module(as root)
```
insmod build/hack_syscall_tbl_module.ko
```

Uninstall
--
Remove the module(as root)
```
rmmod hack_syscall_tbl_module
```

References
--
Wikipedia Rootkit
https://en.wikipedia.org/wiki/Rootkit

Linux Device Drivers
http://lwn.net/Kernel/LDD3/

LKM HACKING
https://web.archive.org/web/20140701183221/https://www.thc.org/papers/LKM_HACKING.html

Memset's blog
http://memset.wordpress.com/

Linux on-the-fly kernel patching without LKM
http://phrack.org/issues/58/7.html

WRITING A SIMPLE ROOTKIT FOR LINUX
https://web.archive.org/web/20160620231623/http://big-daddy.fr/repository/Documentation/Hacking/Security/Malware/Rootkits/writing-rootkit.txt

Linux Cross Reference
http://lxr.free-electrons.com/

zizzu0 LinuxKernelModules
https://github.com/zizzu0/LinuxKernelModules/

Linux Rootkits: New Methods for Kernel 5.7+
https://xcellerator.github.io/posts/linux_rootkits_11/

m0nad Diamorphine
https://github.com/m0nad/Diamorphine
