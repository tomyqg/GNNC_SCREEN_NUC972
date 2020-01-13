################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include resource/subdir.mk
-include driver/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: GNNC_SCREEN

# Tool invocations
GNNC_SCREEN: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-none-linux-gnueabi-g++ -std=gnu++11 -mfloat-abi=soft -L"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib" -L"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_ssl" -L"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/mosquitto" -L"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_sqlite3" -L"/home/aron566/eclipse-workspace/GNNC_SCREEN/lib/lib_mysql" -o "GNNC_SCREEN" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) GNNC_SCREEN
	-@echo ' '

post-build:
	-@echo '----start  copy  binary----'
	-cp /home/aron566/eclipse-workspace/GNNC_SCREEN/Debug/GNNC_SCREEN /home/aron566/nfs/
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
