/*
 * project : erlix
 * author : kdr2
 *
 */
#include <stdlib.h>
#include <string.h>

#include "erlix_term.h"
#include "erlix_node.h"
#include "erlix_connection.h"

VALUE erlix_cErlixPid;

extern const char *erl_thisnodename(void);
extern short erl_thiscreation(void);
extern ErlixNode *erlix_node;

VALUE erlix_pid_alloc(VALUE klass){
    ErlixTerm* pid;
    VALUE obj;
    pid=new_erlix_term();
    pid->type=TYPE_PID;
    obj=Data_Wrap_Struct(klass,0,free_erlix_term,pid);
    return obj;
}

static VALUE erlix_pid_init(VALUE self,VALUE econn){
    ErlixTerm* pid;
    ErlixConnection* conn;
    if(erlix_node==NULL){
        rb_raise(rb_eException,"call Erlix::Node.init(name,cookie) first please!");
        return Qnil;
    }

    if((TYPE(econn)!=T_DATA || RDATA(econn)->dfree!=(RUBY_DATA_FUNC)free_erlix_connection)){
        rb_raise(rb_eTypeError,"the argument is not an Erlix::Connection");
    }
    Data_Get_Struct(self,ErlixTerm,pid);
    Data_Get_Struct(econn,ErlixConnection,conn);
    pid->term=erl_mk_pid(erl_thisnodename(),conn->sock_fd,rand()%16,erl_thiscreation());
    return self;
}

static VALUE erlix_pid_inspect(VALUE self){
    VALUE ret=rb_str_new2("#<Erlix::Pid:");
    ID concat=rb_intern("concat");
    rb_funcall(ret,concat,1,rb_funcall(self,rb_intern("to_s"),0));
    rb_funcall(ret,concat,1,rb_str_new2(">"));
    return ret;
}

static VALUE erlix_pid_etype(VALUE self){
    return rb_str_new2("pid");
}

void init_erlix_pid(VALUE erlix){
    erlix_cErlixPid=rb_define_class_under(erlix,"Pid",rb_cObject);

    rb_define_alloc_func(erlix_cErlixPid,erlix_pid_alloc);
    rb_define_method(erlix_cErlixPid,"initialize",erlix_pid_init,1);
    rb_define_method(erlix_cErlixPid,"inspect",erlix_pid_inspect,0);
    rb_define_method(erlix_cErlixPid,"etype",erlix_pid_etype,0);

    rb_include_module(erlix_cErlixPid,erlix_mErlixTerm);
}


