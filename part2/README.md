# Implementing the /dev/fourinarow

## Compile the device
```
make all
```

## Clean up the device installation
```
make clean
```

## Install the device
```
sudo insmod fourinarow.ko
```

## Remove the device
```
sudo rmmod fourinarow
```

### Approach
In my approach I used a miscdevice and took a lot of inspiration from the just like in the last part of the project I used the source code for /dev/rtc as reference.
For all outputs I used constant pointers to make it so that I don't have to dynamically allocate any outputs. 

### Testing
```
echo -e 'RESET $\n' > /dev/fourinarow
cat /dev/fourinarow

echo -e 'DROPC $\n' > /dev/fourinarow
cat /dev/fourinarow

echo -e 'CTURN\n' > /dev/fourinarow
cat /dev/fourinarow

echo -e 'BOARD\n' > /dev/fourinarow
cat /dev/fourinarow
```