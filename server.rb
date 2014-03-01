#!/usr/bin/ruby
require 'webrick'
include WEBrick


# Very simple http based server to allow
# web based control over the iosolution
#
class IOServlet < HTTPServlet::AbstractServlet
  
  @@instance = nil
  @@instance_creation_mutex = Mutex.new

  def initialize(server)
    super(server)
    @usb ="pc"
    @boot_mode ="boot_a"
    @reset = "0"
  end

  def self.get_instance(server, *options)
    @@instance_creation_mutex.synchronize { 
      if not @@instance then
        @@instance = self.new(server, *options)
      end
    }
    return @@instance
  end

  def do_GET(req, res)
    @boot_mode  = req.query['boot_mode']  if req.query['boot_mode'] 
    @reset = req.query['reset']  if req.query['reset'] 
    @usb = req.query['usb']  if req.query['usb'] 
    
    out = 0;

    if @boot_mode == "main" then
    	out += 1
    else
    	out += 2
    end

    if @reset == "1" then
    	out += 4
    end

    if @usb == "pc" then
    	out += 0xF0
    end
    #Here we run the system command to perform state changes
    `./main #{out}`
    res.body = "boot = #{@boot_mode} , reset = #{@reset} , usb = #{@usb}\n"
    res['Content-Type'] = "text/plain"
  end
end

server = HTTPServer.new( :Port => 2000 )
server.mount("/io", IOServlet)


trap("INT"){ server.shutdown }
server.start
#typical usage:
##!/usr/bin/ruby
##require 'net/http'
##require 'uri'
##Net::HTTP.get_print URI.parse('http://localhost:2000/io/?boot_mode=main')
