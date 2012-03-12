#    Copyright (C) 2012 Denis 'GNUtoo' Carikli
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gps
import gobject
import glib
import sys
import os

class Watcher:
	def __init__(self,startstop):
		self.daemon = gps.gps(host="localhost",port=2947,mode=gps.WATCH_ENABLE|gps.WATCH_RAW|gps.WATCH_NMEA, verbose=0)
		gobject.io_add_watch(self.daemon.sock, gobject.IO_IN, self.handle_response)
		gobject.timeout_add(2000,self.check_counter)
		self.prev = 0
		self.counter = 0
		self.start = True if startstop == "start" else False
	def handle_response(self,source,condition):
		self.daemon.read()
		#print self.daemon.response.strip()
		self.counter += 1
		return True
	def check_counter(self):
		#print "COUNT:" +  str(self.prev) + " -> " + str(self.counter)
		if (self.prev > 0) and (self.prev == self.counter):
			#print "GPS was off"
			self.startstop(False,self.start)
		elif (self.prev > 0) and (self.counter > self.prev):
			#print "GPS was on"
			self.startstop(True,self.start)
		else:
			self.prev = self.counter
			return True
	def startstop(self,state,start):
		if (state != start):
			#print "echo 0 >/sys/devices/virtual/gpio/gpio145/value"
			#print "echo 1 >/sys/devices/virtual/gpio/gpio145/value"
			gpio145 = os.open("/sys/devices/virtual/gpio/gpio145/value", os.O_WRONLY)
			err = os.write(gpio145,"0")
			if (err != 1):
				print "error number {0} in writing 0 to gpio 145".format(err)
			err = os.write(gpio145,"1")
			if (err != 1):
				print "error number {0} in writing 0 to gpio 145".format(err)
			os.close(gpio145)
		sys.exit(0)

if __name__ == '__main__':
	startstop = sys.argv[1]
	if (startstop !=  "start") and (startstop !=  "stop"):
		print "Usage:" + str(sys.argv[0]) + " <start|stop>"
		sys.exit(1)
	w = Watcher(startstop)
	mainloop = glib.MainLoop()
	mainloop.run()
