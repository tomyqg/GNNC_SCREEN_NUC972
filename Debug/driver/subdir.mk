################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/CircularQueue.c \
../driver/cJSON.c 

CPP_SRCS += \
../driver/db_opt.cpp \
../driver/modbus_opt.cpp \
../driver/mqtt_opt.cpp \
../driver/run_log.cpp \
../driver/serial_opt.cpp \
../driver/tcp_client.cpp 

OBJS += \
./driver/CircularQueue.o \
./driver/cJSON.o \
./driver/db_opt.o \
./driver/modbus_opt.o \
./driver/mqtt_opt.o \
./driver/run_log.o \
./driver/serial_opt.o \
./driver/tcp_client.o 

C_DEPS += \
./driver/CircularQueue.d \
./driver/cJSON.d 

CPP_DEPS += \
./driver/db_opt.d \
./driver/modbus_opt.d \
./driver/mqtt_opt.d \
./driver/run_log.d \
./driver/serial_opt.d \
./driver/tcp_client.d 


# Each subdirectory must supply rules for building sources it contributes
driver/%.o: ../driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-linux-gnueabi-gcc -DUSE_EPOLL -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_mysql/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_sqlite3/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_ssl/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/mosquitto/include" -O0 -g3 -Wall -c -fmessage-length=0 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

driver/%.o: ../driver/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-none-linux-gnueabi-g++ -DUSE_EPOLL -DENABLE_MODBUS_DEBUG=0 -DUSE_OLD_CODE=0 -DUSE_OLD_CQ -DENABLE_DB_DEBUG=0 -DUSE_TCP_SEQUENS_NUM=1 -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/mosquitto/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_ssl/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_sqlite3/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_mysql/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


