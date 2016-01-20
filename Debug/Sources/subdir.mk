################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/UART1_Half_Duplex.c 

OBJS += \
./Sources/UART1_Half_Duplex.o 

C_DEPS += \
./Sources/UART1_Half_Duplex.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"CPU_MKL26Z128VLH4" -I"../Sources" -I"../Project_Settings/Startup_Code" -I"../SDK/platform/CMSIS/Include" -I"../SDK/platform/devices" -I"../SDK/platform/devices/MKL26Z4/include" -I"C:\Freescale\KSDK_1.3.0/platform/utilities/inc" -I"C:\Freescale\KSDK_1.3.0/platform/utilities/src" -I"C:\Freescale\KSDK_1.3.0/platform/system/inc" -I"C:\Freescale\KSDK_1.3.0/platform/drivers/src/rtc" -I"C:\Freescale\KSDK_1.3.0/platform/system/src/clock" -I"C:\Freescale\KSDK_1.3.0/platform/system/src/clock/MKL26Z4" -I"C:/Users/Aonghus/MyWorkSpace.kds/GSM_GPRS Testing/Board" -I"C:\Freescale\KSDK_1.3.0\platform\devices\MKL26Z4\startup" -I"C:\Freescale\KSDK_1.3.0\platform\osa\inc" -I"C:\Freescale\KSDK_1.3.0\platform\hal\inc" -I"C:\Freescale\KSDK_1.3.0\platform\drivers\inc" -I"C:\Freescale\KSDK_1.3.0\platform\system\inc" -I"C:\Freescale\KSDK_1.3.0" -I"C:\Users\Aonghus\MyWorkSpace.kds\Copy of Working_Echo\Board" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


