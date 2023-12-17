#!/sbin/openrc-run

command_args="irc.rizon.net:6665 #/g/test"

name="hibot"
command="syscmd(`printf $(pwd)')/${name}"
pidfile="/var/run/${name}.pid"

start() {
  ebegin "Starting $name"
  start-stop-daemon --start --background --make-pidfile --pidfile "$pidfile" --exec "$command" -- $command_args
  eend $?
}

stop() {
  ebegin "Stopping $name"
  start-stop-daemon --stop --pidfile "$pidfile"
  eend $?
}
