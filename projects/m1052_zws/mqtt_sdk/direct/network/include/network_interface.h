/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file network_interface.h
 * @brief Network interface definition for MQTT client.
 *
 * Defines an interface to the TLS layer to be used by the MQTT client.
 * Starting point for porting the SDK to the networking layer of a new platform.
 */

#ifndef __NETWORK_INTERFACE_H_
#define __NETWORK_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <zlg_iot_error.h>
#include "timer_interface.h"

/**
 * @brief Network Type
 *
 * Defines a type for the network struct.  See structure definition below.
 */
typedef struct Network Network;

/**
 * @brief TLS Connection Parameters
 *
 * Defines a type containing TLS specific parameters to be passed down to the
 * TLS networking layer to create a TLS secured socket.
 */
typedef struct {
	char *pRootCALocation;                ///< Pointer to string containing the filename (including path) of the root CA file.
	char *pDeviceCertLocation;            ///< Pointer to string containing the filename (including path) of the device certificate.
	char *pDevicePrivateKeyLocation;    ///< Pointer to string containing the filename (including path) of the device private key file.
	char *pDestinationURL;                ///< Pointer to string containing the endpoint of the MQTT service.
	uint16_t DestinationPort;            ///< Integer defining the connection port of the MQTT service.
	uint32_t timeout_ms;                ///< Unsigned integer defining the TLS handshake timeout value in milliseconds.
	bool ServerVerificationFlag;        ///< Boolean.  True = perform server certificate hostname validation.  False = skip validation \b NOT recommended.
  char *pIFRNName;                    ///< Bind this socket to a particular device like “eth0”, as specified in the passed interface name.
} TLSConnectParams;

/**
 * @brief Network Structure
 *
 * Structure for defining a network connection.
 */
struct Network {
	int (*getSocket)(Network *);

	IoT_Error_t (*connect)(Network *, TLSConnectParams *);

	IoT_Error_t (*read)(Network *, unsigned char *, size_t, Timer *, size_t *);    ///< Function pointer pointing to the network function to read from the network
	IoT_Error_t (*readAny)(Network *, unsigned char *, size_t, size_t *);    ///< Function pointer pointing to the network function to read from the network
	IoT_Error_t (*write)(Network *, unsigned char *, size_t, Timer *, size_t *);    ///< Function pointer pointing to the network function to write to the network
	IoT_Error_t (*disconnect)(Network *);    ///< Function pointer pointing to the network function to disconnect from the network
	IoT_Error_t (*isConnected)(Network *);    ///< Function pointer pointing to the network function to check if TLS is connected
	IoT_Error_t (*destroy)(Network *);        ///< Function pointer pointing to the network function to destroy the network object

	TLSConnectParams tlsConnectParams;        ///< TLSConnect params structure containing the common connection parameters
	void* tlsDataParams;            ///< TLSData params structure containing the connection data parameters that are specific to the library being used
};

/**
 * @brief Initialize the TLS implementation
 *
 * Perform any initialization required by the TLS layer.
 * Connects the interface to implementation by setting up
 * the network layer function pointers to platform implementations.
 *
 * @param pNetwork - Pointer to a Network struct defining the network interface.
 * @param pRootCALocation - Path of the location of the Root CA
 * @param pDeviceCertLocation - Path to the location of the Device Cert
 * @param pDevicyPrivateKeyLocation - Path to the location of the device private key file
 * @param pDestinationURL - The target endpoint to connect to
 * @param DestinationPort - The port on the target to connect to
 * @param timeout_ms - The value to use for timeout of operation
 * @param ServerVerificationFlag - used to decide whether server verification is needed or not
 *
 * @return IoT_Error_t - successful initialization or TLS error
 */
IoT_Error_t iot_tls_init_ex(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
						 char *pDevicePrivateKeyLocation, char *pDestinationURL,
						 uint16_t DestinationPort, uint32_t timeout_ms, bool ServerVerificationFlag);
IoT_Error_t iot_tls_init(Network *pNetwork, const char *host, uint16_t port, uint32_t timeout_ms);

IoT_Error_t iot_socket_init(Network *pNetwork, const char *host, uint16_t port, const char *interface_name, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif //__NETWORK_INTERFACE_H_
