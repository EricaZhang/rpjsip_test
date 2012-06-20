#include <pjsua-lib/pjsua.h>
#define THIS_FILE	"APP"


#include <stdlib.h>

#include "pjmanager.h"

static int (*message_status)(int event, void *userdata, int status);
static int (*income_message)(int event, void *userdata, char* from, char* to, char* body, long fromLen, long toLen, long bodyLen);
static void *userData1;
static void *userData2;


/* Callback called by the library upon receiving incoming call */
static void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
			     pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(acc_id);
    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);

    PJ_LOG(3,(THIS_FILE, "Incoming call from %.*s!!",
			 (int)ci.remote_info.slen,
			 ci.remote_info.ptr));

    /* Automatically answer incoming calls with 200/OK */
    pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
static void on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);
    PJ_LOG(3,(THIS_FILE, "Call %d state=%.*s", call_id,
			 (int)ci.state_text.slen,
			 ci.state_text.ptr));
}

/* Callback called by the library when call's media state has changed */
static void on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
	// When media is active, connect call to sound device.
	pjsua_conf_connect(ci.conf_slot, 0);
	pjsua_conf_connect(0, ci.conf_slot);
    }
}

void msg_status_callback(int (*cb)(int, void *, int), void *userdata){
    message_status = cb;
	userData1 = userdata;
}

void income_msg_callback(int (*cb)(int, void *, char *, char *, char *, long, long, long), void *userdata){
    income_message = cb;
	userData2 = userdata;
}

/** callback wrapper function called by pjsip
 * MESSAGE status */
static void on_pager_status(pjsua_call_id call_id, const pj_str_t *to,
                                    const pj_str_t *body, void *user_data,
                                    pjsip_status_code status, const pj_str_t *reason)
{
	if (call_id == -1) {
		PJ_UNUSED_ARG(call_id);
		PJ_UNUSED_ARG(to);
		PJ_UNUSED_ARG(body);
		PJ_UNUSED_ARG(user_data);
		PJ_UNUSED_ARG(reason);
		
		message_status(1, userData1, status);
	}
}

/** callback wrapper function called by pjsip
 * Incoming IM message (i.e. MESSAGE request)!*/
static void on_pager(pjsua_call_id call_id, const pj_str_t *from,
		                     const pj_str_t *to, const pj_str_t *contact,
						     const pj_str_t *mime_type, const pj_str_t *text)
{
	if (call_id == -1) { 
		PJ_UNUSED_ARG(call_id);
		// PJ_UNUSED_ARG(to);
		PJ_UNUSED_ARG(contact);
		PJ_UNUSED_ARG(mime_type);

		income_message(1, userData2, from->ptr, to->ptr, text->ptr, from->slen, to->slen, text->slen);

		// printf("~~~\n%s\n\n", from->ptr);
		// printf("\n%s\n~~~\n", text->ptr);
		// printf ("~~~\n\n %.*s", from->slen, from->ptr);
		// printf ("\n %.*s \n\n~~~\n", text->slen, text->ptr);
	}

}

/* Display error and exit application */
static void error_exit(const char *title, pj_status_t status)
{
    pjsua_perror(THIS_FILE, title, status);
    pjsua_destroy();
    exit(1);
}


int init(  char *domain, char * user, char * passwd, char *proxy)
{
    pjsua_acc_id acc_id;
    pj_status_t status;

    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) error_exit("Error in pjsua_create()", status);


    /* Init pjsua */
    {
	pjsua_config cfg;
	pjsua_logging_config log_cfg;
	pjsua_media_config media_cfg;
	pjsua_config_default(&cfg);
	
	/* initialize pjsua callbacks */
	cfg.cb.on_incoming_call = &on_incoming_call;
	cfg.cb.on_call_media_state = &on_call_media_state;
	cfg.cb.on_call_state = &on_call_state;
	cfg.cb.on_pager = &on_pager;
	cfg.cb.on_pager_status = &on_pager_status;

	pjsua_logging_config_default(&log_cfg);
	log_cfg.console_level = 4;

	pjsua_media_config_default(&media_cfg);

	status = pjsua_init(&cfg, &log_cfg, NULL);
	if (status != PJ_SUCCESS) error_exit("Error in pjsua_init()", status);
    }

    /* Add UDP transport. */
    {
		pjsua_transport_config cfg;
		pjsua_transport_config_default(&cfg);
		cfg.port = 55060;
		status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
		if (status != PJ_SUCCESS)
		{
			error_exit("Error creating transport", status) ;
		}
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) error_exit("Error starting pjsua", status);

    /* Register to SIP server by creating SIP account. */
    {
	char reg_uri[5+strlen(domain)];
	char id[6+strlen(user)+strlen(passwd)];
	char proxy_uri[22+strlen(proxy)];
			
	pjsua_acc_config cfg;
	pjsua_acc_config_default(&cfg);	

	sprintf( reg_uri, "sip:%s", domain);
	sprintf( id, "sip:%s@%s", user, domain);
	sprintf( proxy_uri, "sip:%s;lr;transport=UDP", proxy);
	
	cfg.id = pj_str(id);
    cfg.reg_uri = pj_str(reg_uri);
    cfg.cred_count = 1;
    cfg.cred_info[0].scheme = pj_str((char*)"digest");
    cfg.cred_info[0].realm = pj_str(domain);
    cfg.cred_info[0].username = pj_str(user);
    cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
    cfg.cred_info[0].data = pj_str(passwd);
    cfg.proxy_cnt = 1;

    cfg.proxy[0] = pj_str(proxy_uri);

	status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
	if (status != PJ_SUCCESS) error_exit("Error adding account", status);
    }
	return acc_id;
}

int call( int acc_id, char* to, char* domain )
{
		char str_uri[13+strlen(to)+strlen(domain)];
		pj_str_t uri = pj_str((char*)"");
		pjsua_call_id call_id;
		pj_status_t status = 0;

		snprintf( str_uri, sizeof(str_uri), "sip:%s@%s", to, domain);

		uri = pj_str(str_uri);

	    status = pjsua_call_make_call(acc_id, &uri, 0, NULL, NULL, &call_id);

		if (status != PJ_SUCCESS) 
		{ 
			error_exit("Error making call", status); 
		}
		return call_id;
}

void endCall(int call_id)
{
	pjsua_call_id id = call_id;
	pjsua_call_hangup(id, 0, NULL, NULL);	
}


int sendIm( int acc_id, char* to, char* domain, char* msgbody )
{
		char str_uri[13+strlen(to)+strlen(domain)];
		pj_str_t tmp_uri = pj_str((char*)"");
		pj_str_t msg = pj_str((char*)"");
		pj_status_t status = 0;
		int result = 1;

		snprintf( str_uri, sizeof(str_uri), "sip:%s@%s", to, domain);

		tmp_uri = pj_str(str_uri);
		msg = pj_str(msgbody);
		
		status = pjsua_im_send(acc_id, &tmp_uri, NULL, &msg, NULL, NULL);
		
		if (status != PJ_SUCCESS) 
		{ 
			result = 0;
			error_exit("Error sending im", status);
		}
		return result;
}

void destroy()
{
	pjsua_destroy();
}

