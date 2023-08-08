fat.img:
	mkfs.fat -C -f 1 -F 16 -r 112 -s 1 fat.img 4096
	sudo mkdir -p /mnt/fatdisk
	sudo mount -o loop fat.img /mnt/fatdisk
	sudo cp -r tree/* /mnt/fatdisk
	sudo umount /mnt/fatdisk

clean:
	rm -f fat.img
