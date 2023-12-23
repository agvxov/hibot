#!/sbin/openrc-run

command_args="irc.rizon.net:6665 \#/g/test"

name="hibot"
pidfile="/var/run/${name}.pid"
supervisor="supervise-daemon"

command="syscmd(`printf $(pwd)')/${name}"

reload() {
      ebegin "Reloading ${RC_SVCNAME}"
      ${supervisor} ${name} --pidfile "${pidfile}" -- ${command_args}
      eend $?
}
