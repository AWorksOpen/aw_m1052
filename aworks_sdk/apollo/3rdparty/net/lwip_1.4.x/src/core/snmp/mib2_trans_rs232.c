#include "lwip/opt.h"

#if LWIP_SNMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/snmp.h"
#include "lwip/snmp_asn1.h"
#include "lwip/snmp_structs.h"

#include <string.h>

#define SERIAL_PORT_MAX_NUM         2

typedef struct rs232_port_info {
	s32_t index;
	s32_t type;
	s32_t in_sig_number;
	s32_t out_sig_number;
	s32_t in_speed;
	s32_t out_speed;
	s32_t in_flow_type;
	s32_t out_flow_type;
} rs232_port_info_t;


typedef struct rs232_async_port_info {
	s32_t index;
	s32_t bits;
	s32_t stop_bits;
	s32_t parity;

	/* not support yet */
//	s32_t auto_baud;
//	s32_t parity_errs;
//	s32_t framing_errs;
//	s32_t overrun_errs;
} rs232_ansync_port_info_t;

#define __CHECK_PORT(port) if(port <= 0 || port > SERIAL_PORT_MAX_NUM) return

typedef err_t (*rs232_port_set_vlu_t)(int port, int id, int value);

static rs232_port_set_vlu_t __gpfunc_rs232_port_set_vlu = NULL;

static s32_t                    __g_rs232_port_number = 0;
static rs232_port_info_t        __g_rs232_port_info[SERIAL_PORT_MAX_NUM] = {0};
static rs232_ansync_port_info_t __g_rs232_async_port_info[SERIAL_PORT_MAX_NUM] = {0};


static void rs232_num_get_value(struct obj_def *od, u16_t len, void *value);
static void rs232_num_get_object_def(u8_t ident_len, s32_t *ident, struct obj_def *od);

static void rs232_port_get_object_def (u8_t ident_len, s32_t *ident, struct obj_def *od);
static void rs232_port_get_value (struct obj_def *od, u16_t len, void *value);
static u8_t rs232_port_set_test(struct obj_def *od, u16_t len, void *value);
static void rs232_port_set_value(struct obj_def *od, u16_t len, void *value);

static void rs232_async_port_get_object_def (u8_t ident_len, s32_t *ident, struct obj_def *od);
static void rs232_async_port_get_value (struct obj_def *od, u16_t len, void *value);
static u8_t rs232_async_port_set_test(struct obj_def *od, u16_t len, void *value);
static void rs232_async_port_set_value(struct obj_def *od, u16_t len, void *value);

/* transmission .1.3.6.1.2.1.10 */

/********************************************************************************************************
 * rs232PortTable
********************************************************************************************************/

/* 1.3.6.1.2.1.10.33.2.1.x.x.0 */
const mib_scalar_node trans_rs232_port_scalar = {
	&rs232_port_get_object_def,
	&rs232_port_get_value,
	&rs232_port_set_test,
	&rs232_port_set_value,
	MIB_NODE_SC,
	0
};

#if (SERIAL_PORT_MAX_NUM == 1)
	const s32_t trans_rs232_port_list_ids[SERIAL_PORT_MAX_NUM] = {1};
	struct mib_node* const trans_rs232_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 2)
	const s32_t trans_rs232_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2};
	struct mib_node* const trans_rs232_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 4)
	const s32_t trans_rs232_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2, 3, 4};
	struct mib_node* const trans_rs232_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar,
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 8)
	const s32_t trans_rs232_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2, 3, 4, 5, 6, 7, 8};
	struct mib_node* const trans_rs232_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar,
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar,
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar,
		(struct mib_node*)&trans_rs232_port_scalar, (struct mib_node*)&trans_rs232_port_scalar
	};
#else
#error "SERIAL_PORT_MAX_NUM mast be 1, 2, 4 or 8, Please fix this!"
#endif /* SERIAL_PORT_MAX_NUM */

/* 1.3.6.1.2.1.10.33.2.1.x */
static struct mib_array_node trans_rs232_port_list = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	SERIAL_PORT_MAX_NUM,
	trans_rs232_port_list_ids,
	trans_rs232_port_list_nodes
};


const s32_t trans_rs232_port_ids[8] = { 1, 2, 3, 4, 5, 6, 7, 8};
struct mib_node* const trans_rs232_port_nodes[8] = {
	(struct mib_node*)&trans_rs232_port_list, (struct mib_node*)&trans_rs232_port_list,
	(struct mib_node*)&trans_rs232_port_list, (struct mib_node*)&trans_rs232_port_list,
	(struct mib_node*)&trans_rs232_port_list, (struct mib_node*)&trans_rs232_port_list,
	(struct mib_node*)&trans_rs232_port_list, (struct mib_node*)&trans_rs232_port_list
};

/* 1.3.6.1.2.1.10.33.2.1 */
const struct mib_array_node trans_rs232_port_table = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	8,
	trans_rs232_port_ids,
	trans_rs232_port_nodes
};

const s32_t trans_rs232_port_entry_ids[1] = { 1 };
struct mib_node* const trans_rs232_port_entry_nodes[1] = {
	(struct mib_node*)&trans_rs232_port_table
};


/* 1.3.6.1.2.1.10.33.2 */
const struct mib_array_node trans_rs232_port_entry = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	1,
	trans_rs232_port_entry_ids,
	trans_rs232_port_entry_nodes
};

/********************************************************************************************************
 * rs232AsyncPortTable
********************************************************************************************************/
/* 1.3.6.1.2.1.10.33.3.1.x.x.0 */
const mib_scalar_node trans_rs232_async_port_scalar = {
	&rs232_async_port_get_object_def,
	&rs232_async_port_get_value,
	&rs232_async_port_set_test,
	&rs232_async_port_set_value,
	MIB_NODE_SC,
	0
};

#if (SERIAL_PORT_MAX_NUM == 1)
	const s32_t trans_rs232_async_port_list_ids[SERIAL_PORT_MAX_NUM] = {1};
	struct mib_node* const trans_rs232_async_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_async_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 2)
	const s32_t trans_rs232_async_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2};
	struct mib_node* const trans_rs232_async_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 4)
	const s32_t trans_rs232_async_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2, 3, 4};
	struct mib_node* const trans_rs232_async_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar,
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar
	};
#elif (SERIAL_PORT_MAX_NUM == 8)
	const s32_t trans_rs232_async_port_list_ids[SERIAL_PORT_MAX_NUM] = { 1, 2, 3, 4, 5, 6, 7, 8};
	struct mib_node* const trans_rs232_async_port_list_nodes[SERIAL_PORT_MAX_NUM] = {
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar,
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar,
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar,
		(struct mib_node*)&trans_rs232_async_port_scalar, (struct mib_node*)&trans_rs232_async_port_scalar
	};
#else
#error "SERIAL_PORT_MAX_NUM mast be 1, 2, 4 or 8, Please fix this!"
#endif /* SERIAL_PORT_MAX_NUM */


/* 1.3.6.1.2.1.10.33.3.1.x */
static struct mib_array_node trans_rs232_async_port_list = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	SERIAL_PORT_MAX_NUM,
	trans_rs232_async_port_list_ids,
	trans_rs232_async_port_list_nodes
};

const s32_t trans_rs232_async_port_ids[4] = { 1, 2, 3, 4};
struct mib_node* const trans_rs232_async_port_nodes[4] = {
	(struct mib_node*)&trans_rs232_async_port_list, (struct mib_node*)&trans_rs232_async_port_list,
	(struct mib_node*)&trans_rs232_async_port_list, (struct mib_node*)&trans_rs232_async_port_list
};

/* 1.3.6.1.2.1.10.33.3.1 */
const struct mib_array_node trans_rs232_async_port_table = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	4,
	trans_rs232_async_port_ids,
	trans_rs232_async_port_nodes
};

const s32_t trans_rs232_async_port_entry_ids[1] = { 1 };
struct mib_node* const trans_rs232_async_port_entry_nodes[1] = {
	(struct mib_node*)&trans_rs232_async_port_table
};

/* 1.3.6.1.2.1.10.33.3 */
const struct mib_array_node trans_rs232_async_port_entry = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	1,
	trans_rs232_async_port_entry_ids,
	trans_rs232_async_port_entry_nodes
};

/* 1.3.6.1.2.1.10.33.1 */
const mib_scalar_node trans_rs232_scalar = {
	&rs232_num_get_object_def,
	&rs232_num_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_SC,
	0
};

// todo:
const s32_t trans_rs232_ids[3] = { 1, 2, 3/*, 4 */};
struct mib_node* const trans_rs232_nodes[3] = {
	(struct mib_node*)&trans_rs232_scalar, (struct mib_node*)&trans_rs232_port_entry,
	(struct mib_node*)&trans_rs232_async_port_entry
};

/* 1.3.6.1.2.1.10.33 */
const struct mib_array_node trans_rs232_table = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	3,
	trans_rs232_ids,
	trans_rs232_nodes
};

const s32_t trans_ids[1] = { 33 };
struct mib_node* const trans_nodes[1] = {
	(struct mib_node*)&trans_rs232_table
};

/* 1.3.6.1.2.1.10 */
const struct mib_array_node trans = {
	&noleafs_get_object_def,
	&noleafs_get_value,
	&noleafs_set_test,
	&noleafs_set_value,
	MIB_NODE_AR,
	1,
	trans_ids,
	trans_nodes
};

static void rs232_num_get_object_def (u8_t ident_len, s32_t *ident, struct obj_def *od)
{
	/* return to object name, adding index depth (1) */
	ident_len += 1;
	ident -= 1;
	if (ident_len == 2) {
		od->id_inst_len = ident_len;
		od->id_inst_ptr = ident;

		od->instance = MIB_OBJECT_SCALAR;
		od->access = MIB_OBJECT_READ_ONLY;
		od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
		od->v_len = sizeof(s32_t);
	} else {
		LWIP_DEBUGF(SNMP_MIB_DEBUG,("interfaces_get_object_def: no scalar\n"));
		od->instance = MIB_OBJECT_NONE;
	}
}

static void rs232_num_get_value (struct obj_def *od, u16_t len, void *value)
{
	LWIP_UNUSED_ARG(len);
	if (od->id_inst_ptr[0] == 1) {

		s32_t *sint_ptr = (s32_t*)value;
		*sint_ptr = __g_rs232_port_number;
	}
}


static void rs232_port_get_object_def (u8_t ident_len, s32_t *ident, struct obj_def *od)
{
	u8_t id;

	/* return to object name, adding index depth (1) */
	ident_len += 2;
	ident -= 2;
	if (ident_len == 3) {
		od->id_inst_len = ident_len - 1;
		od->id_inst_ptr = ident;

		LWIP_ASSERT("invalid id", (ident[0] >= 0) && (ident[0] <= 0xff));
		id = (u8_t)ident[0];
		LWIP_DEBUGF(SNMP_MIB_DEBUG,("get_object_def system.%"U16_F".0\n",(u16_t)id));
		switch (id) {

		  case 1: /* rs232PortIndex */
//		  case 2: /* rs232PortType */
		  case 3: /* rs232PortInSigNumber */
		  case 4: /* rs232PortOutSigNumber */
//		  case 5: /* rs232PortInSpeed */
//		  case 6: /* rs232PortOutSpeed */
		  case 7: /* rs232PortInFlowType */
		  case 8: /* rs232PortOutFlowType */
			od->instance = MIB_OBJECT_SCALAR;
			od->access   = MIB_OBJECT_READ_ONLY;
			od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
			od->v_len = sizeof(s32_t);
			break;
		  case 2: /* rs232PortType */
		  case 5: /* rs232PortInSpeed */
		  case 6: /* rs232PortOutSpeed */
			od->instance = MIB_OBJECT_SCALAR;
			od->access   = MIB_OBJECT_READ_WRITE;
			od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
			od->v_len = sizeof(s32_t);
		    break;
		  default:
			LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_object_def: no such object\n"));
			od->instance = MIB_OBJECT_NONE;
			break;
		};
	} else {
		LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_object_def: no scalar\n"));
		od->instance = MIB_OBJECT_NONE;
	}
}

static void rs232_port_get_value (struct obj_def *od, u16_t len, void *value)
{
	u8_t id, port;
	s32_t *sint_ptr = (s32_t*)value;

	LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
	id = (u8_t)od->id_inst_ptr[0];
	port = (u8_t)od->id_inst_ptr[1];

	__CHECK_PORT(port);

	switch (id) {

	case 1: /* rs232PortIndex */
		*sint_ptr = port;
	  break;
	case 2: /* rs232PortType */
		*sint_ptr = __g_rs232_port_info[port - 1].type;
	  break;
	case 3: /* rs232PortInSigNumber */
		*sint_ptr = __g_rs232_port_info[port - 1].in_sig_number;
	  break;
	case 4: /* rs232PortOutSigNumber */
		*sint_ptr = __g_rs232_port_info[port - 1].out_sig_number;
	  break;
	case 5: /* rs232PortInSpeed */
		*sint_ptr = __g_rs232_port_info[port - 1].in_speed;
	  break;
	case 6: /* rs232PortOutSpeed */
		*sint_ptr = __g_rs232_port_info[port - 1].out_speed;
	  break;
	case 7: /* rs232PortInFlowType */
		*sint_ptr = __g_rs232_port_info[port - 1].in_flow_type;
	  break;
	case 8: /* rs232PortOutFlowType */
		*sint_ptr = __g_rs232_port_info[port - 1].out_flow_type;
	  break;

	default:
	  LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_value(): unknown id: %d\n", id));
	  break;
	}
}

static u8_t
rs232_port_set_test(struct obj_def *od, u16_t len, void *value)
{
  u8_t id, set_ok, port;

  LWIP_UNUSED_ARG(value);
  set_ok = 0;
  LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
  id = (u8_t)od->id_inst_ptr[0];
  port = (u8_t)od->id_inst_ptr[1];

  __CHECK_PORT(port) set_ok;

  switch (id)
  {
	case 1: /* rs232PortIndex */
	case 3: /* rs232PortInSigNumber */
	case 4: /* rs232PortOutSigNumber */
	case 7: /* rs232PortInFlowType */
	case 8: /* rs232PortOutFlowType */
	  break;

	case 2: /* rs232PortType */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
		    set_ok = 1;
		}
	  break;
	case 5: /* rs232PortInSpeed */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
		    set_ok = 1;
		}
	  break;
	case 6: /* rs232PortOutSpeed */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
		    set_ok = 1;
		}
      break;
    default:
      LWIP_DEBUGF(SNMP_MIB_DEBUG,("rs232_port_set_test(): unknown id: %d\n", id));
      break;
  };
  return set_ok;
}

static void
rs232_port_set_value(struct obj_def *od, u16_t len, void *value)
{
  u8_t id, port;

  LWIP_ASSERT("invalid len", len <= 0xff);
  LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
  id = (u8_t)od->id_inst_ptr[0];
  port = (u8_t)od->id_inst_ptr[1];

  __CHECK_PORT(port);

  switch (id)
  {
	case 1: /* rs232PortIndex */
	case 3: /* rs232PortInSigNumber */
	case 4: /* rs232PortOutSigNumber */
	case 7: /* rs232PortInFlowType */
	case 8: /* rs232PortOutFlowType */
	  break;

	case 2: /* rs232PortType */
	case 5: /* rs232PortInSpeed */
	case 6: /* rs232PortOutSpeed */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
			__gpfunc_rs232_port_set_vlu(port, id, *(int *)value);
		}
      break;
  default:
    LWIP_DEBUGF(SNMP_MIB_DEBUG,("rs232_port_set_value(): unknown id: %d\n", id));
    break;
  };
}


static void rs232_async_port_get_object_def (u8_t ident_len, s32_t *ident, struct obj_def *od)
{
	u8_t id;

	/* return to object name, adding index depth (1) */
	ident_len += 2;
	ident -= 2;
	if (ident_len == 3) {
		od->id_inst_len = ident_len - 1;
		od->id_inst_ptr = ident;

		LWIP_ASSERT("invalid id", (ident[0] >= 0) && (ident[0] <= 0xff));
		id = (u8_t)ident[0];
		LWIP_DEBUGF(SNMP_MIB_DEBUG,("get_object_def system.%"U16_F".0\n",(u16_t)id));
		switch (id) {

		  case 1: /* rs232AsyncPortIndex */
		  case 2: /* rs232AsyncPortBits */
		  case 3: /* rs232AsyncPortStopBits */
		  case 4: /* rs232AsyncPortParity */
//		  case 5: /* rs232AsyncPortAutobaud */
//		  case 6: /* rs232AsyncPortParityErrs */
//		  case 7: /* rs232AsyncPortFramingErrs */
//		  case 8: /* rs232AsyncPortOverrunErrs */
			od->instance = MIB_OBJECT_SCALAR;
			od->access   = MIB_OBJECT_READ_ONLY;
			od->asn_type = (SNMP_ASN1_UNIV | SNMP_ASN1_PRIMIT | SNMP_ASN1_INTEG);
			od->v_len = sizeof(s32_t);
			break;
		  default:
			LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_object_def: no such object\n"));
			od->instance = MIB_OBJECT_NONE;
			break;
		};
	} else {
		LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_object_def: no scalar\n"));
		od->instance = MIB_OBJECT_NONE;
	}
}

static void rs232_async_port_get_value (struct obj_def *od, u16_t len, void *value)
{
	u8_t id, port;
	s32_t *sint_ptr = (s32_t*)value;

	LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
	id = (u8_t)od->id_inst_ptr[0];
	port = (u8_t)od->id_inst_ptr[1];

	__CHECK_PORT(port);

	switch (id) {

	case 1: /* rs232AsyncPortIndex */
		*sint_ptr = port;
	  break;
	case 2: /* rs232AsyncPortBits */
		*sint_ptr = __g_rs232_async_port_info[port - 1].bits;
	  break;
	case 3: /* rs232AsyncPortStopBits */
		*sint_ptr = __g_rs232_async_port_info[port - 1].stop_bits;
	  break;
	case 4: /* rs232AsyncPortParity */
		*sint_ptr = __g_rs232_async_port_info[port - 1].parity;
	  break;

	default:
	  LWIP_DEBUGF(SNMP_MIB_DEBUG,("system_get_value(): unknown id: %d\n", id));
	  break;
	}
}

static u8_t
rs232_async_port_set_test(struct obj_def *od, u16_t len, void *value)
{
  u8_t id, set_ok, port;

  LWIP_UNUSED_ARG(value);
  set_ok = 0;
  LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
  id = (u8_t)od->id_inst_ptr[0];
  port = (u8_t)od->id_inst_ptr[1];

  __CHECK_PORT(port) set_ok;

  switch (id)
  {
    case 1: /* rs232AsyncPortIndex */
    	break;

    case 2: /* rs232AsyncPortBits */
    case 3: /* rs232AsyncPortStopBits */
    case 4: /* rs232AsyncPortParity */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
		    set_ok = 1;
		}

	  break;
	case 5: /* rs232AsyncPortAutobaud */
	case 6: /* rs232AsyncPortParityErrs */
	case 7: /* rs232AsyncPortFramingErrs */
	case 8: /* rs232AsyncPortOverrunErrs */
      break;
    default:
      LWIP_DEBUGF(SNMP_MIB_DEBUG,("rs232_async_port_set_test(): unknown id: %d\n", id));
      break;
  };
  return set_ok;
}

static void
rs232_async_port_set_value(struct obj_def *od, u16_t len, void *value)
{
  u8_t id, port;

  LWIP_ASSERT("invalid len", len <= 0xff);
  LWIP_ASSERT("invalid id", (od->id_inst_ptr[0] >= 0) && (od->id_inst_ptr[0] <= 0xff));
  id = (u8_t)od->id_inst_ptr[0];
  port = (u8_t)od->id_inst_ptr[1];

  __CHECK_PORT(port);

  switch (id)
  {
  case 1: /* rs232AsyncPortIndex */
  	break;

  case 2: /* rs232AsyncPortBits */
  case 3: /* rs232AsyncPortStopBits */
  case 4: /* rs232AsyncPortParity */
		if (NULL != __gpfunc_rs232_port_set_vlu) {
			__gpfunc_rs232_port_set_vlu(port, id + 8, *(int *)value);
		}
	  break;

  case 5: /* rs232AsyncPortAutobaud */
  case 6: /* rs232AsyncPortParityErrs */
  case 7: /* rs232AsyncPortFramingErrs */
  case 8: /* rs232AsyncPortOverrunErrs */
     break;
  default:
    LWIP_DEBUGF(SNMP_MIB_DEBUG,("rs232_async_port_set_value(): unknown id: %d\n", id));
    break;
  };
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void snmp_set_rs232_number (s32_t num)
{
	if (num > SERIAL_PORT_MAX_NUM) {
		num = SERIAL_PORT_MAX_NUM;
	}
	__g_rs232_port_number = num;
	trans_rs232_port_list.maxlength = num;
	trans_rs232_async_port_list.maxlength = num;
}

/* port±àºÅ´Ó1¿ªÊ¼ */
void snmp_set_rs232_type (s32_t port, s32_t type)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].type = type;
}

void snmp_set_rs232_insig_num (s32_t port, s32_t num)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].in_sig_number = num;
}

void snmp_set_rs232_outsig_num (s32_t port, s32_t num)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].out_sig_number = num;
}

void snmp_set_rs232_inspeed (s32_t port, s32_t speed)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].in_speed = speed;
}

void snmp_set_rs232_outspeed (s32_t port, s32_t speed)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].out_speed = speed;
}

void snmp_set_rs232_inflowtype (s32_t port, s32_t flow_type)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].in_flow_type = flow_type;
}

void snmp_set_rs232_outflowtype (s32_t port, s32_t flow_type)
{
	__CHECK_PORT(port);
	__g_rs232_port_info[port - 1].out_flow_type = flow_type;
}

void snmp_set_rs232_bits (s32_t port, s32_t bits)
{
	__CHECK_PORT(port);
	__g_rs232_async_port_info[port - 1].bits = bits;
}

void snmp_set_rs232_stop_bits (s32_t port, s32_t stop_bits)
{
	__CHECK_PORT(port);
	__g_rs232_async_port_info[port - 1].stop_bits = stop_bits;
}

void snmp_set_rs232_parity (s32_t port, s32_t parity)
{
	__CHECK_PORT(port);
	__g_rs232_async_port_info[port - 1].parity = parity;
}

/**
 * @ id :
 * r       1  : rs232PortIndex
 * rw      2  : rs232PortType
 * r       3  : rs232PortInSigNumber
 * r       4  : rs232PortOutSigNumber
 * rw      5  : rs232PortInSpeed
 * rw      6  : rs232PortOutSpeed
 * r       7  : rs232PortInFlowType
 * r       8  : rs232PortOutFlowType
 *
 * r       9  : rs232AsyncPortIndex
 * rw      10 : rs232AsyncPortBits
 * rw      11 : rs232AsyncPortStopBits
 * rw      12 : rs232AsyncPortParity
 * r       13 : rs232AsyncPortAutobaud
 * r       14 : rs232AsyncPortParityErrs
 * r       15 : rs232AsyncPortFramingErrs
 * r       16 : rs232AsyncPortOverrunErrs
 */
void snmp_rs232_set_value_cb_register (err_t (*pfunc_cb)(int port, int id, int value))
{
	__gpfunc_rs232_port_set_vlu = pfunc_cb;
}

#endif /* LWIP_SNMP */
