## How to Use

### How to build .ko with specific linux-version

User should assign linux root path and driver aim to balal...

You can update those variables in `${JLSemi_Driver}/config.mk`


```makefile

# if you want to build jl31xx only
driver_

# if you want to build jl31xx and jl21xx both
# driver_aims = jl31xx jl21xx

# assign linux path

linux_root_path = /path/to/linux-source-5.4.x/
```


### How to build driver into your kernel

Step1: Move source codes into driver dir.


You can find source codes in xxxxxxx/.


Step2: Update makefile in xxxx

Step3: Update makefile in xxxx


Step4: build with xxxx


Reference:

You can find more details in `linux/driver_xxxxx.html`
