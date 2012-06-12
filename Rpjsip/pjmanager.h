int init(char *domain, char *user, char *passwd, char *proxy);

void destroy();

int call( int acc_id, char* number, char* domain );

int sendIm( int acc_id, char* to, char* domain, char* msgbody );

void endCall(int call_id);

void msg_status_callback(int (*cb)(int, void *), void *userdata);

void income_msg_callback(int (*cb)(int, void *), void *userdata);

