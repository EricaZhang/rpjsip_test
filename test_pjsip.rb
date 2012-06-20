require 'rubygems'
require 'rpjsip'

include Rpjsip

account_id = pjsip_init( {:proxy => "127.0.0.1",
                          :domain => "127.0.0.1", 
                          :user => "test", 
                          :passwd => "test" }) { puts "\n\n\n\n------------> init\n\n\n\n" }

# need audio library to make call
# call_id = make_call( {:account_id => account_id , 
#                       :to => "demo", 
#                       :domain => "127.0.0.1" })  { puts "\n\n\n\n------------> calling!\n\n\n\n" } 
                    
im_result = send_im( {:account_id => account_id , 
                      :to => "demo", 
                      :domain => "127.0.0.1",
                      :msgbody => " testing MSG 123 " })  { puts "\n\n\n\n------------> sending im!\n\n\n\n" } 


status_cb = Proc.new {|event, userdata, status|
   puts "\n>>>\n>>>>>>\n>>>>>>>>>\n #{status}: event is #{event}, userdata is #{userdata} \n>>>>>>>>>\n>>>>>>\n>>>\n"
}
msg_status_proc(status_cb, "TEST--ING")

inmsg_cb = Proc.new {|event, userdata, from, to, body|
    puts "\n>>>\n>>>>>>\n>>>>>>>>> \nfrom: #{from}\nto: #{to}\n#{body}\n>>>>>>>>>\n>>>>>>\n>>>\n"
}
income_msg_proc(inmsg_cb, "TEST**ING")


sleep(5)

# end_call call_id

pjsip_destroy
