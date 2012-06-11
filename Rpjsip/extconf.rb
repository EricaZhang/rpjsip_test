# Loads mkmf which is used to make makefiles for Ruby extensions
require 'mkmf'

# Give it a name
extension_name = 'rpjsip'

# The destination
dir_config(extension_name)

# ld config //please check the target and include all libraries
# targetName = '-x86_64-unknown-linux-gnu'
# $LDFLAGS = "-lpjsua#{targetName} -lpjsip-ua#{targetName} -lpjsip-simple#{targetName} -lpjsip#{targetName} -lpjmedia-codec#{targetName} -lpjmedia#{targetName} -lpjmedia-audiodev#{targetName} -lpjnath#{targetName} -lpjlib-util#{targetName} -lresample#{targetName} -lmilenage#{targetName} -lsrtp#{targetName} -lgsmcodec#{targetName} -lspeex#{targetName} -lilbccodec#{targetName} -lg7221codec#{targetName} -lportaudio#{targetName}  -lpj#{targetName} -lm -lpthread -lcrypto -lssl  "
targetName = '-i386-apple-darwin11.4.0'
$LDFLAGS = "-lpjsua#{targetName} -lpjsip-ua#{targetName} -lpjsip-simple#{targetName} -lpjsip#{targetName} -lpjmedia-codec#{targetName} -lpjmedia#{targetName} -lpjmedia-audiodev#{targetName} -lpjnath#{targetName} -lpjlib-util#{targetName} -lresample#{targetName} -lmilenage#{targetName} -lsrtp#{targetName} -lgsmcodec#{targetName} -lspeex#{targetName} -lilbccodec#{targetName} -lg7221codec#{targetName} -lportaudio#{targetName}  -lpj#{targetName} -lm -lpthread  -framework CoreAudio -framework CoreServices -framework AudioUnit -framework AudioToolbox -lcrypto -lssl  "  

# Do the work
if have_library "pjsua#{targetName}" then
  create_makefile(extension_name)
else
  puts "No pjsip library found"
end