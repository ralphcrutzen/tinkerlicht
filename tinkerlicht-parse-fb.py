#!/usr/bin/python

import time
import string
from unidecode import unidecode
from random import randint
import sys
sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient as bridgeclient
from twython import Twython, TwythonError
import facebook
import colors
from screams import screams
import tokens


bridge = bridgeclient()

page_access_token = tokens.facebook_page_access_token
graph = facebook.GraphAPI(page_access_token)
profile = graph.get_object('tinkerlicht')
posts = graph.get_connections(profile['id'], 'tagged', fields = 'id,message,from')

twitter = Twython(
    tokens.twitter_app_key,
    tokens.twitter_app_secret,
    tokens.twitter_oauth_token,
    tokens.twitter_oath_token_secret
)

hashtag = '#tinkerlicht:'


def getCommandFromTweet(text):
    # Remove all characters at the beginning, including the hashtag
    iHashtag = text.find(hashtag)
    if iHashtag == -1:
        return 'xHasht'
    text = text[iHashtag + len(hashtag):]

    # Remove non-alphanumeric characters in front
    while len(text) != 0 and not text[0].isalnum():
        text = text[1:]

    if len(text) == 0:
        return 'xColor'

    # Remove everything from the first non-alphanumeric character
    # Only one dash (seperating two colors) is allowed
    i = 0
    indexEnd = 0
    foundIndex = False
    foundDash = False
    while i < len(text) and not foundIndex:
        if not (text[i].isalnum() or (text[i] == '-' and not foundDash)):
            indexEnd = i
            foundIndex = True
        if text[i] == '-':
            foundDash = True
        i = i + 1
    if indexEnd > 0:
        text = text[:indexEnd]
        # If last character is the only dash, remove it
        if text[-1] == '-':
            text = text[:len(text) - 1]

    return text.lower()


def getCommandFromMessage(text):
    s = ''.join(ch for ch in text if ch not in set(string.punctuation))
    words  = s.lower().split()
    for word in words:
        if isHtmlColor(word) or isHexColor(word) or word == 'random':
            return word
            break


def isHexColor(text):
    if len(text) != 6:
        return False
    for i in range(0, 6):
        if ((text[i] < 'A' or text[i] > 'F') and
            (text[i] < 'a' or text[i] > 'f') and
            (text[i] < '0' or text[i] > '9')):
            return False
    return True


def isHtmlColor(text):
    try:
        colors.htmlColors[text]
    except:
        return False
    return True


def isDoubleColor(text):
    indexDash = text.find('-')
    if indexDash == -1:
        return False
    return True


def getColor1(text):
    indexDash = text.find('-')
    return text[:indexDash]


def getColor2(text):
    indexDash = text.find('-')
    return text[indexDash + 1:]


def toHex(text):
    if isHexColor(text):
        return text
    if text == 'random':
        randColor = ''
        for i in range(0, 6):
            randColor = randColor + '0123456789abcdef'[randint(0, 15)]
        return randColor
    if isHtmlColor(text):
        return colors.htmlColors[text]
    return 'xColor'


def changeSingleColor(command):
    if command == 'xHasht':
        status = user + ' Bah. Een syntax error! ' + \
            'Kijk eens op http://blog.crutzen.eu/2015/04/26/tinkerlicht/ (' + \
            getDateTime() + ')'
        tweetMessage(status)
    else:
        hexColor = toHex(command)
        if command == 'random':
            name = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
            hexColor = toHex(name)
        if hexColor != 'xColor':
            bridge.put('color1', hexColor)
            bridge.put('color2', hexColor)
            bridge.put('newColor', '1')
            if command == 'random':
                status = screams[randint(0, len(screams))] + ' ' + user + \
                    ' geeft het @TinkerLicht de willekeurige kleur ' + name + \
                    '.'
            else:
                status = screams[randint(0, len(screams))] + ' ' + user + \
                    ' maakt het @TinkerLicht ' + command + '.'
            bridge.put('message', status)
        else:
            status = user + ' ' + command + ' is geen geldige kleur. ' + \
                'Kijk eens op ' + \
                'https://en.wikipedia.org/wiki/List_of_colors_%28compact%29 (' + \
                getDateTime() + ')'
            tweetMessage(status)
            graph.put_object(parent_object=profile['id'], connection_name='feed', \
                message=status)


def changeDoubleColor(command):
    color1 = getColor1(command)
    hexColor1 = toHex(color1)
    if color1 == 'random':
        name1 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
        hexColor1 = toHex(name1)
    color2 = getColor2(command)
    hexColor2 = toHex(color2)
    if color2 == 'random':
        name2 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
        hexColor2 = toHex(name2)
    if hexColor1 != 'xColor' and hexColor2 != 'xColor':
        bridge.put('color1', hexColor1)
        bridge.put('color2', hexColor2)
        bridge.put('newColor', '1')
        status = screams[randint(0, len(screams))] + ' ' + user + \
            ' maakt het @TinkerLicht '
        if color1 == 'random':
            status = status + 'de willekeurige kleur ' + name1
        else:
            status = status + color1
        status = status + ' en '
        if color2 == 'random':
            status = status + 'de willekeurige kleur ' + name2 + '.'
        else:
            status = status + color2 + '.'
        bridge.put('message', status)
    else:
        tweetMessage(user + ' ' + command + ' is geen geldige kleuren' + \
            'combinatie. Kijk eens op ' + \
            'https://en.wikipedia.org/wiki/List_of_colors_%28compact%29 (' + \
            getDateTime() + ')')


def tweetScream():
    m = '.' + user + ' ' + screams[randint(0, len(screams))]
    tweetMessage(m)


def tweetStartup():
    name1 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
    name2 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
    hexColor1 = toHex(name1)
    hexColor2 = toHex(name2)
    bridge.put('color1', hexColor1)
    bridge.put('color2', hexColor2)
    bridge.put('newColor', '1')
    m = screams[randint(0, len(screams))] + ' TinkerLicht is weer online ' +\
        'met de kleuren ' + name1 + ' en ' + name2 +\
        '. Meer info: http://tinkerlicht.crutzen.eu'
    bridge.put('message', m)


def tweetMessage(status):
    try:
        twitter.update_status(status=status)
    except TwythonError as e:
        print e


def tweetAuto():
    name1 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
    name2 = colors.htmlColors.keys()[randint(0, len(colors.htmlColors))]
    hexColor1 = toHex(name1)
    hexColor2 = toHex(name2)
    bridge.put('color1', hexColor1)
    bridge.put('color2', hexColor2)
    bridge.put('newColor', '1')
    m = screams[randint(0, len(screams))] + ' Automatisch ingesteld op ' +\
        'de kleuren ' + name1 + ' en ' + name2 + '.'
    bridge.put('message', m)


def getDateTime():
    return time.strftime("%d-%m-%y %H:%M:%S")


"""
Twitter
"""

try:
    result = twitter.search(q='#tinkerlicht', count=1, include_entities='false')
except TwythonError as e:
    print e

if result:
    tweet = result['statuses'][0]['text'].lower()
    print tweet
    user = result['statuses'][0]['user']['screen_name']
    user = '@' + user
    print user
else:
    bridge.put('prevTweet', ' ')

if bridge.get('prevTweet') == ' ':
    tweetStartup()
    bridge.put('prevTweet', tweet)

if bridge.get('auto') == '1':
    tweetAuto()
    bridge.put('auto', '0')
else:
    if tweet != bridge.get('prevTweet'):
        bridge.put('prevTweet', tweet)
        command = getCommandFromTweet(tweet)
        print command
        if command == 'scream':
            tweetScream()
        else:
            if isDoubleColor(command):
                changeDoubleColor(command)
            else:
                changeSingleColor(command)

"""
Facebook
"""

message = unidecode(posts['data'][0]['message']).lower()
user = unidecode(posts['data'][0]['from']['name'])
if bridge.get('prevMessage') == ' ':
    bridge.put('prevMessage', message)
else:
    if message != bridge.get('prevMessage'):
        bridge.put('prevMessage', message)
        command = getCommandFromMessage(message)
        changeSingleColor(command)
