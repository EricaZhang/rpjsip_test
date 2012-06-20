// include the Ruby headers 
#include "ruby.h"

// include pjmanager header
#include <pjmanager.h>

//util
char * string(VALUE v);

int  integer(VALUE v);

VALUE hash_get(VALUE hash, char * key);

// void * get_value( VALUE hash, char *key, void * (*c)(VALUE) );
void * get_value( VALUE hash, char *key, char *type );
int get_int_value( VALUE hash, char *key );

VALUE execBlock(VALUE v);

// Defining a space for information and references about the module to be stored internally
VALUE Rpjsip = Qnil;

// Prototype for the initialization method - called by ruby
void Init_rpjsip();

// functions prototypes - must be prefixed by method_
VALUE method_pjsip_init(VALUE self, VALUE hash);

VALUE method_pjsip_destroy(VALUE self);

VALUE method_make_call(VALUE self, VALUE hash);

VALUE method_end_call(VALUE self, VALUE call_id);

VALUE method_send_im(VALUE self, VALUE hash);

int msg_status_int_cb(int event, void *userdata, int status);

VALUE method_msg_status_ext(VALUE obj, VALUE cb, VALUE userdata);

int income_msg_int_cb(int event, void *userdata, char *from, char *to, char *body, long fromLen, long toLen, long bodyLen);

VALUE method_income_msg_ext(VALUE obj, VALUE cb, VALUE userdata);

// initializes the module
void Init_rpjsip() {
	Rpjsip = rb_define_module("Rpjsip");
	rb_define_method(Rpjsip, "pjsip_init", method_pjsip_init, 1);
	rb_define_method(Rpjsip, "pjsip_destroy", method_pjsip_destroy, 0);	
	rb_define_method(Rpjsip, "make_call", method_make_call, 1);	
	rb_define_method(Rpjsip, "end_call", method_end_call, 1 );
	rb_define_method(Rpjsip, "send_im", method_send_im, 1);	

	rb_define_method(Rpjsip, "msg_status_proc", method_msg_status_ext, 2);	
	rb_define_method(Rpjsip, "income_msg_proc", method_income_msg_ext, 2);	
	
}

VALUE method_pjsip_init(VALUE self, VALUE hash)
{	
	// char * domain = get_value(hash, (char*)":domain", string);
	// char * user = get_value(hash, (char*)":user", string);
	// char * passwd = get_value(hash, (char*)":passwd", string);
	// char * proxy = get_value(hash, (char*)":proxy", string);
	char * domain = get_value(hash, (char*)":domain", (char*)"string");
	char * user = get_value(hash, (char*)":user", (char*)"string");
	char * passwd = get_value(hash, (char*)":passwd", (char*)"string");
	char * proxy = get_value(hash, (char*)":proxy", (char*)"string");
	
	return INT2NUM(init(domain, user, passwd, proxy));
}

VALUE method_make_call(VALUE self, VALUE hash)
{
	//VALUE acc_id, volatile VALUE number, VALUE domain

	// int  acc_id = get_value( hash, (char*)":account_id", integer );
	// char * to_id = get_value(hash, (char*)":to_id", string);
	// char * domain = get_value(hash, (char*)":domain", string);
	int acc_id = get_int_value( hash, (char*)":account_id" );
	char * to_id = get_value(hash, (char*)":to_id", (char*)"string");
	char * domain = get_value(hash, (char*)":domain", (char*)"string");
	
	VALUE v =  INT2NUM(call( acc_id, to_id, domain ));
	execBlock(self);
	return v;	
}

VALUE method_send_im(VALUE self, VALUE hash)
{
	//VALUE acc_id, volatile VALUE to, VALUE domain, VALUE msgbody
	
	// int  acc_id = get_value( hash, (char*)":account_id", integer );
	// char * to_id = get_value(hash, (char*)":to_id", string);
	// char * domain = get_value(hash, (char*)":domain", string);
	// char * msgbody = get_value(hash, (char*)":msgbody", string);
	int acc_id = get_int_value( hash, (char*)":account_id");
	char * to_id = get_value(hash, (char*)":to_id", (char*)"string");
	char * domain = get_value(hash, (char*)":domain", (char*)"string");
	char * msgbody = get_value(hash, (char*)":msgbody", (char*)"string");
	
	VALUE v =  INT2NUM(sendIm( acc_id, to_id, domain, msgbody ));
	execBlock(self);
	return v;	
}



VALUE execBlock(VALUE v)
{
	if (rb_block_given_p())
	{
		return  rb_yield(v);
	}
	return Qnil;
}

VALUE method_end_call(VALUE self, VALUE call_id)
{
	endCall( NUM2INT(call_id) );
	return Qnil;
}

VALUE method_pjsip_destroy(VALUE self)
{
	destroy();
	return Qnil;
}

char * string(VALUE v)
{
	return (char*)rb_string_value_cstr(&v);
}

int integer(VALUE v){
	return NUM2INT(v);
}

VALUE hash_get(VALUE hash, char * key)
{
	return rb_hash_aref(hash,rb_eval_string(key) );
}

// void * get_value( VALUE hash, char *key, void * (*converter)(VALUE) )
void * get_value( VALUE hash, char *key, char *type )
{
	VALUE v = hash_get(hash, key);
	if(v  == Qnil){
		rb_raise( rb_eException, "Symbol %s not found.", key );
	}
	// return converter(v);
	
	if (strcmp(type, (char*)"string") == 0){	
		return string(v);
	}
}

int get_int_value( VALUE hash, char *key )
{
	VALUE v = hash_get(hash, key);
	if(v  == Qnil){
		rb_raise( rb_eException, "Symbol %s not found.", key );
	}
	// return converter(v);
	return integer(v);
}



int msg_status_int_cb(int event, void *userdata, int status) {
    VALUE passthrough = (VALUE)userdata;
    VALUE cb;
    VALUE cbdata;

    cb = rb_ary_entry(passthrough, 0);
    cbdata = rb_ary_entry(passthrough, 1);

    // rb_funcall(rb_class_of(cb), rb_to_id(cb), 3, INT2NUM(event), cbdata, INT2NUM(status));
	rb_funcall(cb, rb_intern("call"), 3, INT2NUM(event), cbdata, INT2NUM(status));

    return 0;
}

VALUE method_msg_status_ext(VALUE obj, VALUE cb, VALUE userdata) {
    VALUE passthrough;

    if (rb_class_of(cb) != rb_cProc)
        rb_raise(rb_eTypeError, "Expected Symbol callback");

    passthrough = rb_ary_new();
    rb_ary_store(passthrough, 0, cb);
    rb_ary_store(passthrough, 1, userdata);

    msg_status_callback(msg_status_int_cb, (void *)passthrough);

}


int income_msg_int_cb(int event, void *userdata, char *from, char *to, char *body, long fromLen, long toLen, long bodyLen) {
    VALUE passthrough = (VALUE)userdata;
    VALUE cb;
    VALUE cbdata;

    cb = rb_ary_entry(passthrough, 0);
    cbdata = rb_ary_entry(passthrough, 1);

    // rb_funcall(rb_class_of(cb), rb_to_id(cb), 5, INT2NUM(event), cbdata, rb_str_new(from, fromLen), rb_str_new(to, toLen), rb_str_new(body, bodyLen));
	rb_funcall(cb, rb_intern("call"), 5, INT2NUM(event), cbdata, rb_tainted_str_new(from, fromLen), rb_tainted_str_new(to, toLen), rb_tainted_str_new(body, bodyLen));

    return 0;
}

VALUE method_income_msg_ext(VALUE obj, VALUE cb, VALUE userdata) {
    VALUE passthrough;

    if (rb_class_of(cb) != rb_cProc)
        rb_raise(rb_eTypeError, "Expected Proc callback");

    passthrough = rb_ary_new();
    rb_ary_store(passthrough, 0, cb);
    rb_ary_store(passthrough, 1, userdata);

    income_msg_callback(income_msg_int_cb, (void *)passthrough);
	
}
