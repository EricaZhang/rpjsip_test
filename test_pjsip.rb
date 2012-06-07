require 'rubygems'
require 'rpjsip'

include Rpjsip

account_id = pjsip_init :proxy => "127.0.0.1",
                          :domain => "127.0.0.1", 
                          :user => "demo", 
                          :passwd => "demo" 
                        
call_id = make_call ( {:account_id => account_id , 
                    :number => "test", 
                    :domain => "127.0.0.1" })  { puts "\n\n\n\n------------> calling!\n\n\n\n" } 
                    
im_result = send_im  ( {:account_id => account_id , 
                    :to => "test", 
                    :domain => "127.0.0.1",
                    :msgbody => " testing MSG 123 " })  { puts "\n\n\n\n------------> sending im!\n\n\n\n" } 
                    
sleep(15)

end_call call_id

pjsip_destroy
