require 'rubygems'
require 'rpjsip'

include Rpjsip

account_id = pjsip_init( {:proxy => "223.255.138.226",
                          :domain => "223.255.138.226", 
                          :user => "test", 
                          :passwd => "test" }) { puts "\n\n\n\n------------> init\n\n\n\n" }

# call_id = make_call( {:account_id => account_id, 
#                       :to_id => "oscar", 
#                       :domain => "223.255.138.226" })  { puts "\n\n\n\n------------> calling!\n\n\n\n" } 

status_cb = Proc.new {|event, userdata, status|
   puts "\n>>>\n>>>>>>\n>>>>>>>>>\n #{status}: event is #{event}, userdata is #{userdata} \n>>>>>>>>>\n>>>>>>\n>>>\n"
}
msg_status_proc(status_cb, "TEST--ING")

# def status_cb(event, userdata, status)
#    puts "\n>>>\n>>>>>>\n>>>>>>>>>\n #{status}: event is #{event}, userdata is #{userdata} \n>>>>>>>>>\n>>>>>>\n>>>\n"
# end
# msg_status_symbol(:status_cb, "TEST--ING")

def end_pjsip
  pjsip_destroy
end

$quitsip = false

inmsg_cb = Proc.new {|event, userdata, from, to, body|
    puts "\n>>>\n>>>>>>\n>>>>>>>>> \nfrom: #{from}\nto: #{to}\n#{body}\n>>>>>>>>>\n>>>>>>\n>>>\n"
    if (body == "done")
      # pjsip_destroy
      # exit
      $quitsip = true

    end
}
income_msg_proc(inmsg_cb, "TEST**ING")


im_result = send_im( {:account_id => account_id, 
                      :to_id => "oscar", 
                      :domain => "223.255.138.226",
                      :msgbody => " testing MSG 123 " })  { puts "\n\n\n\n------------> sending im!\n\n\n\n" } 




# def inmsg_cb(event, userdata, from, to, body)
#   puts "\n>>>\n>>>>>>\n>>>>>>>>> \nfrom: #{from}\nto: #{to}\n#{body}\n>>>>>>>>>\n>>>>>>\n>>>\n"
# end
# income_msg_symbol(:inmsg_cb, "TEST**ING")

# sleep(5)

while $quitsip==false
end

end_pjsip

# end_call call_id
# pjsip_destroy
