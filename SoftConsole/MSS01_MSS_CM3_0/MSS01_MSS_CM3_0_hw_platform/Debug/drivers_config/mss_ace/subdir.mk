################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers_config/mss_ace/ace_config.c 

OBJS += \
./drivers_config/mss_ace/ace_config.o 

C_DEPS += \
./drivers_config/mss_ace/ace_config.d 


# Each subdirectory must supply rules for building sources it contributes
drivers_config/mss_ace/%.o: ../drivers_config/mss_ace/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DACTEL_STDIO_THRU_UART -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\CMSIS -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\CMSIS\startup_gcc -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_ace -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_gpio -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_nvm -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_nvm\drivers -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_nvm\drivers\F2DSS_NVM -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_pdma -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_rtc -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_rtc\drivers -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_rtc\drivers\mss_rtc -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers\mss_timer -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers_config -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\drivers_config\mss_ace -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\hal -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\hal\CortexM3 -IC:\Users\mmanceor\Desktop\VisionAssistance\SoftConsole\MSS01_MSS_CM3_0\MSS01_MSS_CM3_0_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


