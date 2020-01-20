################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GuiLite.cpp \
../src/UIcode.cpp \
../src/guilite_opt.cpp \
../src/key_button.cpp \
../src/main.cpp \
../src/parameter_setting.cpp \
../src/timer_manager.cpp 

OBJS += \
./src/GuiLite.o \
./src/UIcode.o \
./src/guilite_opt.o \
./src/key_button.o \
./src/main.o \
./src/parameter_setting.o \
./src/timer_manager.o 

CPP_DEPS += \
./src/GuiLite.d \
./src/UIcode.d \
./src/guilite_opt.d \
./src/key_button.d \
./src/main.d \
./src/parameter_setting.d \
./src/timer_manager.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-none-linux-gnueabi-g++ -DUSE_EPOLL -DENABLE_MODBUS_DEBUG=0 -DUSE_OLD_CODE=0 -DUSE_OLD_CQ -DENABLE_DB_DEBUG=0 -DUSE_TCP_SEQUENS_NUM=1 -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/mosquitto/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_ssl/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_sqlite3/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_mysql/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


