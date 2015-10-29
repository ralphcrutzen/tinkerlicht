#!/usr/bin/python

import os
import sys
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient
from twython import Twython, TwythonError
import facebook
import tokens

page_access_token = tokens.facebook_page_access_token
graph = facebook.GraphAPI(page_access_token)
profile = graph.get_object('tinkerlicht')

bridge = bridgeclient()

twitter = Twython(
    tokens.twitter_app_key,
    tokens.twitter_app_secret,
    tokens.twitter_oauth_token,
    tokens.twitter_oath_token_secret
)

os.system('/usr/bin/fswebcam --no-banner -r1024x768 \
    --save /mnt/sda1/arduino/photo.jpg')
status = bridge.get('message').encode('utf-8')
photo = open('/mnt/sda1/arduino/photo.jpg', 'rb')
try:
    twitter.update_status_with_media(status=status, media=photo)
except TwythonError as e:
    print e
photo.close()

graph.put_photo(image=open('/mnt/sda1/arduino/photo.jpg'), album_path=profile['id'] + "/picture", message=status)
