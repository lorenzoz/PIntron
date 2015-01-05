#!/bin/bash -e
####
# Copyright (C) 2015  Gianluca Della Vedova
#
# Distributed under the terms of the GNU Affero General Public License (AGPL)
#
#
# This file is part of PIntron.
#
# PIntron is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PIntron is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with PIntron.  If not, see <http://www.gnu.org/licenses/>.
#
####
#
# Some parts are originally from baseimage-docker (https://github.com/phusion/baseimage-docker)
# Copyright (c) 2013-2014 Phusion
#
####
set -x

# allow ssh to root
test -f /root/config/root_key.pub && cat /root/config/root_key.pub >> /root/.ssh/authorized_keys


# Create the user
adduser --disabled-password --gecos "PIntron" pintron
usermod -L pintron
mkdir -p /home/pintron/.ssh
chmod 700 /home/pintron/.ssh
mkdir -p /home/pintron/input
mkdir -p /home/pintron/results
# Get the web servers public keys
test -f /root/config/web_keys.pub && cp /root/config/web_keys.pub /home/pintron/.ssh/authorized_keys
test -f config/web_keys_url && curl -L $(cat config/web_keys_url) >>  /home/pintron/.ssh/authorized_keys
chown --recursive pintron:pintron /home/pintron/
