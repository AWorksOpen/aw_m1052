set(shared_src 
    commands.c  
    device.c    
    errors.c    
    mqtt_client.c   
    settings.c  
    warnings.c
)

add_executable(device_example device_expl.c ${shared_src})
target_link_libraries(device_example client network ${SYS_LIBS})

add_executable(gateway_example gateway_expl.c ${shared_src})
target_link_libraries(gateway_example client network ${SYS_LIBS})

add_executable(download_firmware_example download_firmware_expl.c ${shared_src})
target_link_libraries(download_firmware_example client network ${SYS_LIBS})