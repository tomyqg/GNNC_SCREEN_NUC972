################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../resource/MAIN_BACKGROUND_bmp.cpp \
../resource/Microsoft_YaHei_20.cpp \
../resource/RTC_bmp.cpp \
../resource/background1_bmp.cpp \
../resource/clock_bmp.cpp \
../resource/editbox_bmp.cpp \
../resource/editbox_focus_bmp.cpp \
../resource/enter_bmp.cpp \
../resource/enter_focus_bmp.cpp \
../resource/enter_push_bmp.cpp \
../resource/key_bmp.cpp \
../resource/logoGNNC_bmp.cpp \
../resource/setting_bmp.cpp \
../resource/shield_bmp.cpp \
../resource/siren_bmp.cpp \
../resource/songti_8.cpp \
../resource/system_bmp.cpp \
../resource/usb_bmp.cpp \
../resource/worldwide_bmp.cpp 

OBJS += \
./resource/MAIN_BACKGROUND_bmp.o \
./resource/Microsoft_YaHei_20.o \
./resource/RTC_bmp.o \
./resource/background1_bmp.o \
./resource/clock_bmp.o \
./resource/editbox_bmp.o \
./resource/editbox_focus_bmp.o \
./resource/enter_bmp.o \
./resource/enter_focus_bmp.o \
./resource/enter_push_bmp.o \
./resource/key_bmp.o \
./resource/logoGNNC_bmp.o \
./resource/setting_bmp.o \
./resource/shield_bmp.o \
./resource/siren_bmp.o \
./resource/songti_8.o \
./resource/system_bmp.o \
./resource/usb_bmp.o \
./resource/worldwide_bmp.o 

CPP_DEPS += \
./resource/MAIN_BACKGROUND_bmp.d \
./resource/Microsoft_YaHei_20.d \
./resource/RTC_bmp.d \
./resource/background1_bmp.d \
./resource/clock_bmp.d \
./resource/editbox_bmp.d \
./resource/editbox_focus_bmp.d \
./resource/enter_bmp.d \
./resource/enter_focus_bmp.d \
./resource/enter_push_bmp.d \
./resource/key_bmp.d \
./resource/logoGNNC_bmp.d \
./resource/setting_bmp.d \
./resource/shield_bmp.d \
./resource/siren_bmp.d \
./resource/songti_8.d \
./resource/system_bmp.d \
./resource/usb_bmp.d \
./resource/worldwide_bmp.d 


# Each subdirectory must supply rules for building sources it contributes
resource/%.o: ../resource/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-none-linux-gnueabi-g++ -DUSE_EPOLL -DENABLE_MODBUS_DEBUG=0 -DUSE_OLD_CODE=0 -DUSE_OLD_CQ -DENABLE_DB_DEBUG=0 -DUSE_TCP_SEQUENS_NUM=1 -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/mosquitto/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_ssl/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_sqlite3/include" -I"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_mysql/include" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


